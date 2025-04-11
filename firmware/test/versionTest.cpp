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
#include <fmt_version.h>
#include "stub_comms.h"
}


TEST_GROUP(fmt_version)
{
  void setup()
  {
    UT_PTR_SET(fmt_sendMsg, fmt_sendMsg_test);
  }
  void teardown()
  {
  }
};

TEST(fmt_version, sendVersion_succeeds)
{
  Top msg;
  CHECK(fmt_sendVersion());
  CHECK(fmt_getMsg_test(&msg));
  CHECK_EQUAL(Top_Version_tag, msg.which_sub);
  CHECK_EQUAL(VERSION_MAJOR, msg.sub.Version.major);
  CHECK_EQUAL(VERSION_MINOR, msg.sub.Version.minor);
  CHECK_EQUAL(VERSION_PATCH, msg.sub.Version.patch);
}