
#include <fmt_update.h> // in build binary dir
#include "fmt_comms.h"  // fmt_sendMsg
#include "fmt_flash.h"
#include <stdbool.h>
#include <cmsis_gcc.h>

static void sendPageStatus(uint32_t pageIndex, PageStatusEnum status)
{
  fmt_sendMsg((const Top){
      .which_sub = Top_PageStatus_tag,
      .sub = {
          .PageStatus = {
              .pageIndex = pageIndex,
              .status = status}}});
}

#if !FMT_UPDATE_SUPPORTED
bool handleImageData(ImageData msg)
{
  sendPageStatus(msg.pageIndex, PageStatusEnum_WRITE_FAIL);
  return false;
}

#else // FW update is supported.

#define CHUNKS_PER_PAGE_MAX (UPDATE_PAGE_SIZE / DATA_MSG_PAYLOAD_SIZE_MAX)
#define PAGES_COUNT_MAX (FMT_IMAGE_DOWNLOAD_PARTITION_SIZE / UPDATE_PAGE_SIZE)
#define NO_CHUNKS_PROCESSED ((1 << CHUNKS_PER_PAGE_MAX) - 1)

static uint32_t chunksPending = NO_CHUNKS_PROCESSED;
static uint32_t activePage = 0;
static uint8_t pageBuffer[UPDATE_PAGE_SIZE];
static callback_t downloadStartCb = NULL;
static callback_t downloadCompleteCb = NULL;

// Static function prototypes.
static bool imageDataMsgValid(ImageData *msg);
static bool allChunksProcessed(void);
static void prepForNewPage(PageStatusEnum thisPageStatus);
static void processChunk(ImageData *msg);
static void processPage(void);

void fmt_setFirstPageReceivedCallback(callback_t onDownloadStart)
{
  downloadStartCb = onDownloadStart;
}
void fmt_setDownloadFinishCallback(callback_t onDownloadComplete)
{
  downloadCompleteCb = onDownloadComplete;
}

bool handleImageData(ImageData msg)
{
  if (imageDataMsgValid(&msg))
  {
    processChunk(&msg);
    if (allChunksProcessed())
    {
      if (downloadStartCb && activePage == 0)
        downloadStartCb();
      processPage();
      prepForNewPage(PageStatusEnum_WRITE_SUCCESS);
      if (downloadCompleteCb && activePage == (msg.pageCount - 1))
        downloadCompleteCb();
    }
    return true;
  }
  else
  {
    prepForNewPage(PageStatusEnum_WRITE_FAIL);
    return false;
  }
}

/** imageDataMsgValid enforces the following policy on ImageData messages:
 * only accept a new page if there are no unprocessed chunks on the active page.
 */
static bool imageDataMsgValid(ImageData *msg)
{
  // TODO: consider adding a policy akin to pageIndex that chunkCountInPage
  // can't change once one chunk has been received for the active page.

  union err_u
  {
    struct errBits_s
    {
      uint32_t chunkIndexTooBig : 1;
      uint32_t chunkIndexAlreadyReceived : 1;
      uint32_t shortChunkThatIsntLast : 1;
      uint32_t payloadSizeTooBig : 1;
      uint32_t prematureNewPage : 1;
      uint32_t pageIndexTooBig : 1;
    } b; // b for bits
    uint32_t overall;
  };

  union err_u err = {0};

  // (chunkIndexOk && dataSizeOk) implies no buffer overflow.
  // This chunk must be still pending (no repeats).
  err.b.chunkIndexTooBig = msg->chunkIndex >= CHUNKS_PER_PAGE_MAX;
  err.b.chunkIndexAlreadyReceived = !(chunksPending & (1 << msg->chunkIndex));

  bool chunkNotLast = msg->chunkIndex != (msg->chunkCountInPage - 1);
  bool shortChunk = msg->payload.size < DATA_MSG_PAYLOAD_SIZE_MAX;
  err.b.shortChunkThatIsntLast = shortChunk && chunkNotLast;

  // Only the last chunk is allowed to be shorter than the max size.
  err.b.payloadSizeTooBig = msg->payload.size > DATA_MSG_PAYLOAD_SIZE_MAX;

  // all messages must have the same pageIndex until all chunks are processed.
  err.b.prematureNewPage =
      msg->pageIndex != activePage && chunksPending != NO_CHUNKS_PROCESSED;
  err.b.pageIndexTooBig = msg->pageIndex >= PAGES_COUNT_MAX;

  if (err.overall)
  {
    __BKPT(6);
  }

  return err.overall == 0;
}

static bool allChunksProcessed(void)
{
  return !chunksPending;
}

static void prepForNewPage(PageStatusEnum thisPageStatus)
{
  chunksPending = NO_CHUNKS_PROCESSED; // signals a page-change is allowed.
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

  // Clear all b with position greater than the count of chunks expected.
  chunksPending &= ((1 << msg->chunkCountInPage) - 1);

  memcpy(
      pageBuffer + msg->chunkIndex * DATA_MSG_PAYLOAD_SIZE_MAX,
      msg->payload.bytes,
      msg->payload.size);
}

static void processPage(void)
{

  uint32_t writeAddress =
      FMT_IMAGE_DOWNLOAD_ADDRESS + (activePage * UPDATE_PAGE_SIZE);

  fmt_flash_write(writeAddress, pageBuffer, UPDATE_PAGE_SIZE);
}

#endif // FMT_UPDATE_SUPPORTED
