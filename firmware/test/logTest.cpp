#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_log.h>
#include "stub_comms.h"
}

TEST_GROUP(fmt_log)
{
  const char *textIn;
  int numIn;
  Top msgSent = {0};
  logLevel_t level;

  void setup()
  {
    // set some defaults.
    numIn = 5000;
    textIn = "Hello, Firment.";
    fmt_setLogLevel(LOG_VERBOSE);
    UT_PTR_SET(fmt_getMsg, fmt_getMsg_test);
    UT_PTR_SET(fmt_sendMsg, fmt_sendMsg_test);
  }

  void teardown()
  {
    msgSent = (Top){0};
    fmt_sendMsg(msgSent); // clear the stored message.
  }

  void checkSentEqualsInput(void)
  {
    fmt_getMsg(&msgSent);
    ENUMS_EQUAL_INT(Top_Log_tag, msgSent.which_sub);
    STRCMP_EQUAL(textIn, msgSent.sub.Log.text);
    LONGS_EQUAL(msgSent.sub.Log.value, numIn);
  }

  void checkLogNotSent(void)
  {
    fmt_getMsg(&msgSent);
    // msgSent having sub type 0 indicates no log was sent.
    ENUMS_EQUAL_INT(0, msgSent.which_sub);
  }
};

// set up pre-conditions for test
// run the CUT.
// validate results
// clean up

TEST(fmt_log, sendLog_msgWellFormed)
{
  // just use default text and num.
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  checkSentEqualsInput();
}

TEST(fmt_log, sendLog_numberNegative)
{
  numIn = -5000;
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  checkSentEqualsInput();
}

TEST(fmt_log, sendLog_msgNull)
{
  // Log still sends what it can (the number) if the text is NULL.
  textIn = NULL;
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  fmt_getMsg(&msgSent);
  LONGS_EQUAL(numIn, msgSent.sub.Log.value);
  ENUMS_EQUAL_INT(Top_Log_tag, msgSent.which_sub);
}

TEST(fmt_log, sendLog_msgEmptyString)
{
  textIn = "";
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  checkSentEqualsInput();
}

TEST(fmt_log, sendLog_levelBelowActive)
{
  fmt_setLogLevel(LOG_INFO);
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  checkLogNotSent();
}

TEST(fmt_log, sendLog_levelEqualActive)
{
  fmt_setLogLevel(LOG_INFO);
  fmt_sendLog(LOG_INFO, textIn, numIn);
  checkSentEqualsInput();
}
