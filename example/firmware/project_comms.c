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

static void sendPageStatus(uint32_t pageIndex, PageStatusEnum status)
{
  fmt_sendMsg((const Top){
      .which_sub = Top_PageStatus_tag,
      .sub = {
          .PageStatus = {
              .pageIndex = pageIndex,
              .status = status}}});
}

void handleImageData(ImageData msg)
{
  static uint32_t chunkRxCount = 0;
  static uint32_t activePage = 0;
  static uint8_t pageBuffer[FLASH_PAGE_SIZE];
  static uint8_t *writeAdrs = pageBuffer;

  bool bufferOverflow =
    (writeAdrs + msg.payload.size) > (pageBuffer + FLASH_PAGE_SIZE);
  bool wrongPage = msg.pageIndex != activePage;
  bool badPageCount = 
    msg.pageCount > (IMAGE_RX_SECTOR_SIZE / FLASH_PAGE_SIZE) || !msg.pageCount;

  // Error: check for pageIndex getting out-of-sync with active page.
  if (bufferOverflow || wrongPage || badPageCount)
  {
    sendPageStatus(activePage, PageStatusEnum_WRITE_FAIL);
    activePage = 0;
    writeAdrs = pageBuffer;
    chunkRxCount = 0;
    return;
  }

  memcpy(writeAdrs, msg.payload.bytes, msg.payload.size);
  writeAdrs += msg.payload.size;

  if (++chunkRxCount == msg.chunkCountInPage)
  {
    // If we're ready to write the first page, erase the sector. 
    if (activePage == 0)
    {
      hal_flash_erase(IMAGE_RX_ADDRESS, IMAGE_RX_SECTOR_SIZE);
    }
    hal_flash_write(
        IMAGE_RX_ADDRESS + activePage * FLASH_PAGE_SIZE,
        pageBuffer,
        FLASH_PAGE_SIZE);
    writeAdrs = pageBuffer;
    chunkRxCount = 0;
    if (++activePage == msg.pageCount)
    {
      activePage = 0;
    }
    sendPageStatus(activePage, PageStatusEnum_WRITE_SUCCESS);
  }
}
