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
  Top msgSent;
  logLevel_t level;
  void setup()
  {
    // set some defaults.
    numIn = 5000;
    textIn = "Hello, Firment.";
  }
  void teardown()
  {
  }
  void checkSentEqualsInput()
  {
    fmt_getMsg(&msgSent);
    ENUMS_EQUAL_INT(Top_Log_tag, msgSent.which_sub);
    STRCMP_EQUAL(textIn, msgSent.sub.Log.text);
    LONGS_EQUAL(msgSent.sub.Log.value, numIn);
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
  textIn = NULL;
  fmt_sendLog(LOG_VERBOSE, textIn, numIn);
  checkSentEqualsInput();
}

/*
TEST(fmt_log, sendLog_msgEmptyString)
{
  //
}

TEST(fmt_log, sendLog_levelBelowActive)
{
  // should do nothing.
}

TEST(fmt_log, sendLog_levelEqualActive)
{
  // should send
}
*/