
#include <fmt_update.h> // in build binary dir
#include "fmt_flash.h"
#include "fmt_flash_port.h"
#include <stdbool.h>

#define CHUNKS_PER_PAGE_MAX  (FLASH_PAGE_SIZE / IMAGE_PART_MAX_SIZE)
#define PAGES_PER_SECTOR_MAX (FMT_IMAGE_DOWNLOAD_SECTOR_SIZE / FLASH_PAGE_SIZE)
#define NO_CHUNKS_PROCESSED ((1 << CHUNKS_PER_PAGE_MAX) - 1)

static uint32_t chunksPending = NO_CHUNKS_PROCESSED;
static uint32_t activePage = 0;
static uint8_t pageBuffer[FLASH_PAGE_SIZE];

// Static function prototypes.
static void sendPageStatus(uint32_t pageIndex, PageStatusEnum status);
static bool imageDataMsgValid(ImageData *msg);
static inline bool allChunksProcessed(void);
static void prepForNewPage(PageStatusEnum thisPageStatus);
static void processChunk(ImageData *msg);
static void processPage(void);

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

static inline bool allChunksProcessed(void)
{
  return !chunksPending;
}

static void prepForNewPage(PageStatusEnum thisPageStatus)
{
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
      pageBuffer + msg->chunkIndex * IMAGE_PART_MAX_SIZE,
      msg->payload.bytes,
      msg->payload.size);
}

static void processPage(void)
{
  if (activePage == 0)
  {
    hal_flash_erase(FMT_IMAGE_DOWNLOAD_ADDRESS, FMT_IMAGE_DOWNLOAD_SECTOR_SIZE);
  }
  hal_flash_write(
      FMT_IMAGE_DOWNLOAD_ADDRESS + activePage * FLASH_PAGE_SIZE,
      pageBuffer,
      FLASH_PAGE_SIZE);
}
