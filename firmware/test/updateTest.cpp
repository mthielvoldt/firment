#include <CppUTest/TestHarness.h>

/**
 * 1. #include the CUT header.
 * 2. Does CUT have an init function (something that needs setup for operation)?
 *    if so, do next 4.
 *  - Test init() returns ok for basic proper inputs.
 *  - Test init() returns fail for each improper input that's required.
 *  - #include the config prod main passes to init(), refactoring as req'd.
 *  - Test init() returns ok for the config prod main passes.
 *
 * 3. Find the modules this depends on, and swap in spies as needed.
 *
 */
extern "C"
{
#include <fmt_update.h>
#include <fmt_flash_port.h>
#include "stub_comms.h"
}
#define TOO_GREAT 1000

TEST_GROUP(fmt_update)
{
  int onFirstPageCount, onLastPageCount;
  ImageData msg;
  Top rxMsg;
  void setup()
  {
    onFirstPageCount = onLastPageCount = 0;
    // Insert fmt_comms spy.
    UT_PTR_SET(fmt_getMsg, fmt_getMsg_test);
    UT_PTR_SET(fmt_sendMsg, fmt_sendMsg_test);
    
    // Send an invalid message to reset the download.
    msg.chunkIndex = TOO_GREAT;
    handleImageData(msg);

    // Replace good data
    msg = (ImageData){
      .pageIndex = 0,
      .pageCount = 0,
      .chunkIndex = 0,
      .chunkCountInPage = 0,
      .payload = {.size = sizeof(msg.payload.bytes), .bytes = {1, 2, 3}}};
  }
  void teardown()
  {
  }
  void onLastPageSaved(void)
  {
    onLastPageCount++;
  }
  void onFirstPageReceived(void)
  {
    onFirstPageCount++;
  }
};

TEST(fmt_update, invalidImageData_triggersStatusFail)
{
  CHECK_TRUE(fmt_getMsg(&rxMsg));
  CHECK_EQUAL(Top_PageStatus_tag, rxMsg.which_sub);
  CHECK_EQUAL(0, rxMsg.sub.PageStatus.pageIndex);
  CHECK_EQUAL(PageStatusEnum_WRITE_FAIL, rxMsg.sub.PageStatus.status);
}

TEST(fmt_update, firstOfSeveralChunks_returnsOk)
{
  msg.chunkCountInPage = 8;
  msg.pageCount = 1;
  CHECK_TRUE(handleImageData(msg));
}

TEST(fmt_update, singleMessageImage_NoCallbacks_returnsOk)
{
  msg.chunkCountInPage = 1;
  msg.pageCount = 1;
  CHECK_TRUE(handleImageData(msg));
}

// TEST(fmt_update, singleMessageImage_firsPageCallback_calledOnce)
// {
// }

// TEST(fmt_update, badImageData_triggersFailMessage)
// {
// }