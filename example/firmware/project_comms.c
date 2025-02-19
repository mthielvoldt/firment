/** Comms module
 * Concerns:
 * - Executes system actions associated with valid inbound messages.
 * - Constructs and queues outbound messages.
 * - Does NOT do CRC validation - all inbound messages are assumed CRC-valid.
 *
 * Action types triggered by inbound messages:
 * - change a parameter (triggers next action)
 * - reporting value of requested parameter
 * - general-purpose command (responses vary)
 *
 * Actions types assocated with outbound messages:
 * - periodic telemetry (unrequested)
 * - conditional-triggered alerts
 */
#include "project_comms.h"
#include "control.h"
#include <fmt_log.h>
#include <fw_update.h>
#include <fmt_flash.h>

#define CALLS_PER_FULL_ROTATION 1000U

// optimization: stop storing the config.
static portPin_t led;

void comm_init(portPin_t ledPin)
{
  led = ledPin;
  XMC_GPIO_Init(led.port, led.pin, &gpOutPinConfig);
}

void comm_handleTelemetry(void)
{
  static uint32_t count = 0;
  static uint32_t rotations = 0;

  // execute periodic telemetry sends in round-robin.
  switch (count++)
  {
  case 0:
  {
    rotations++;
    telem_t telem = ctl_getTelem();
    fmt_sendMsg((const Top){
        .which_sub = Top_WaveformTlm_tag,
        .sub = {
            .WaveformTlm = {
                .currentMa = telem.currentMa,
                .voltageV = telem.voltage}}});

    break;
  }
  case 200:
  {
    // fmt_sendLog(LOG_INFO, "A test. ", rotations);
    break;
  }
  case 400:
    break;
  case CALLS_PER_FULL_ROTATION:
    count = 0;
    break;
  }
}

void handleWaveformCtl(WaveformCtl msg)
{
  static float count = 0;
  count++;
  XMC_GPIO_ToggleOutput(led.port, led.pin);
  fmt_sendLog(LOG_INFO, "WaveformCtl rx cnt: ", count);

  const waveShape_t toControlShape[] = {
      [WaveShape_UNSPECIFIED] = WAVE_SHAPE_DC,
      [WaveShape_SINE] = WAVE_SHAPE_SINE,
      [WaveShape_SQUARE] = WAVE_SHAPE_SQUARE,
      [WaveShape_SAWTOOTH] = WAVE_SHAPE_SAWTOOTH,
      [WaveShape_DC] = WAVE_SHAPE_DC,
  };
  // opportunity here to limit input and notify if command exceeds limits.
  const waveCfg_t cfg = {
      .id = msg.channel,
      .amplitudeV = msg.enabled ? msg.amplitudeV : 0.0F,
      .frequencyHz = msg.frequencyHz,
      .offsetV = msg.enabled ? msg.offsetV : 0.0F,
      .shape = toControlShape[msg.shape]};
  ctl_setWaveform(cfg);
}

void handleReset(Reset msg)
{
  NVIC_SystemReset();
}

// filter chunksPending so only as many bits as chunks expected can be 1.
#define NO_CHUNKS_PROCESSED ((1 << CHUNKS_PER_PAGE_MAX) - 1)
static uint32_t chunksPending = NO_CHUNKS_PROCESSED;
static uint32_t activePage = 0;
static uint8_t pageBuffer[FLASH_PAGE_SIZE];
static uint8_t *writeAdrs = pageBuffer;

static void sendPageStatus(uint32_t pageIndex, PageStatusEnum status)
{
  fmt_sendMsg((const Top){
      .which_sub = Top_PageStatus_tag,
      .sub = {
          .PageStatus = {
              .pageIndex = pageIndex,
              .status = status}}});
}

/** imageDataMsgValid enforces the following policy on ImageData messages:
 * only accept a new page if there are no unprocessed chunks on the active page.
 */
static bool imageDataMsgValid(ImageData *msg)
{
  // TODO: consider adding a policy akin to pageIndex that chunkCountInPage 
  // can't change once one chunk has been received for the active page.

  // (chunkIndexOk && dataSizeOk) implies no buffer overflow.
  bool chunkIndexOk = msg->chunkIndex < CHUNKS_PER_PAGE_MAX;

  bool chunkIsLast = msg->chunkIndex == (CHUNKS_PER_PAGE_MAX - 1);
  bool dataSizeExactlyMax = msg->payload.size == IMAGE_CHUNK_MAX_SIZE;

  // Only the last chunk is allowed to be shorter than the max size.
  bool dataSizeOk =
      msg->payload.size <= IMAGE_CHUNK_MAX_SIZE &&
      (dataSizeExactlyMax || chunkIsLast);

  // all messages must have the same pageIndex until all chunks are processed.
  bool pageIndexOk =
      (msg->pageIndex == activePage || chunksPending == NO_CHUNKS_PROCESSED) &&
      (msg->pageIndex < PAGES_PER_SECTOR_MAX);

  return chunkIndexOk && dataSizeOk && pageIndexOk;
}

static inline bool allChunksProcessed(void)
{
  return !chunksPending;
}

static void prepForNewPage(PageStatusEnum thisPageStatus)
{
  writeAdrs = pageBuffer;
  chunksPending = NO_CHUNKS_PROCESSED;  // signals a page-change is allowed.
  // should be last; ungates new messages being sent.
  sendPageStatus(activePage, thisPageStatus);
}

static void processChunk(ImageData *msg)
{
  // Note: pageIndex != activePage only when no chunks have been processed yet.
  activePage = msg->pageIndex;

  // Each bit in chunksPending tracks the status of a chunk with a given index.
  // Signal this chunk has been processed by clearing the bit corresponding to
  // its chunkIndex.
  chunksPending ^= (1 << msg->chunkIndex);

  // Clear all bits with position greater than the count of chunks expected.
  chunksPending &= ((1 << msg->chunkCountInPage) - 1);

  memcpy(
      pageBuffer + msg->chunkIndex * 32,
      msg->payload.bytes,
      msg->payload.size);

  writeAdrs += msg->payload.size;
}

static void processPage(void)
{
  if (activePage == 0)
  {
    // hal_flash_erase(IMAGE_RX_ADDRESS, IMAGE_RX_SECTOR_SIZE);
  }
  hal_flash_write(
      IMAGE_RX_ADDRESS + activePage * FLASH_PAGE_SIZE,
      pageBuffer,
      FLASH_PAGE_SIZE);
}

void handleImageData(ImageData msg)
{
  if (imageDataMsgValid(&msg))
  {
    processChunk(&msg);
    if (allChunksProcessed())
    {
      processPage();
      prepForNewPage(PageStatusEnum_WRITE_SUCCESS);
    }
  }
  else
  {
    prepForNewPage(PageStatusEnum_WRITE_FAIL);
  }
}
