
#include <fmt_update.h> // in build binary dir
#include "fmt_comms.h"  // fmt_sendMsg
#include "fmt_flash.h"
#include <stdbool.h>

#define CHUNKS_PER_PAGE_MAX (UPDATE_PAGE_SIZE / IMAGE_PART_MAX_SIZE)
#define PAGES_PER_SECTOR_MAX (FMT_IMAGE_DOWNLOAD_SECTOR_SIZE / UPDATE_PAGE_SIZE)
#define NO_CHUNKS_PROCESSED ((1 << CHUNKS_PER_PAGE_MAX) - 1)

static uint32_t chunksPending = NO_CHUNKS_PROCESSED;
static uint32_t activePage = 0;
static uint8_t pageBuffer[UPDATE_PAGE_SIZE];
static callback_t downloadStartCb = NULL;
static callback_t downloadCompleteCb = NULL;

// Static function prototypes.
static void sendPageStatus(uint32_t pageIndex, PageStatusEnum status);
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
      processPage();
      prepForNewPage(PageStatusEnum_WRITE_SUCCESS);
      if (msg.pageIndex == (msg.pageCount - 1))
      {
        if (downloadCompleteCb)
          downloadCompleteCb();
      }
    }
    return true;
  }
  else
  {
    prepForNewPage(PageStatusEnum_WRITE_FAIL);
    return false;
  }
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

/** imageDataMsgValid enforces the following policy on ImageData messages:
 * only accept a new page if there are no unprocessed chunks on the active page.
 */
static bool imageDataMsgValid(ImageData *msg)
{
  // TODO: consider adding a policy akin to pageIndex that chunkCountInPage
  // can't change once one chunk has been received for the active page.

  // (chunkIndexOk && dataSizeOk) implies no buffer overflow.
  // This chunk must be still pending (no repeats).
  bool chunkIndexOk =
      msg->chunkIndex < CHUNKS_PER_PAGE_MAX &&
      chunksPending & (1 << msg->chunkIndex);

  bool chunkIsLast = msg->chunkIndex == (msg->chunkCountInPage - 1);
  bool dataSizeExactlyMax = msg->payload.size == IMAGE_PART_MAX_SIZE;

  // Only the last chunk is allowed to be shorter than the max size.
  bool dataSizeOk =
      msg->payload.size <= IMAGE_PART_MAX_SIZE &&
      (dataSizeExactlyMax || chunkIsLast);

  // all messages must have the same pageIndex until all chunks are processed.
  bool pageIndexOk =
      (msg->pageIndex == activePage || chunksPending == NO_CHUNKS_PROCESSED) &&
      (msg->pageIndex < PAGES_PER_SECTOR_MAX);

  return chunkIndexOk && dataSizeOk && pageIndexOk;
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

  // Clear all bits with position greater than the count of chunks expected.
  chunksPending &= ((1 << msg->chunkCountInPage) - 1);

  memcpy(
      pageBuffer + msg->chunkIndex * IMAGE_PART_MAX_SIZE,
      msg->payload.bytes,
      msg->payload.size);
}

static void processPage(void)
{
  if (activePage == 0)
  {
    // This might erase one or more sectors in this partition, so preceeds write.
    if (downloadStartCb)
      downloadStartCb();
    fmt_flash_erase(FMT_IMAGE_DOWNLOAD_ADDRESS, FMT_IMAGE_DOWNLOAD_SECTOR_SIZE);
  }
  fmt_flash_write(
      FMT_IMAGE_DOWNLOAD_ADDRESS + activePage * UPDATE_PAGE_SIZE,
      pageBuffer,
      UPDATE_PAGE_SIZE);
}
