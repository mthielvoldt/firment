#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_ioc_test.h>
}

void callback0(void)
{

}

TEST_GROUP(fmt_ioc_test)
{
  const uint8_t pin0 = 0;
  const uint8_t out0 = 2;
  const uint32_t prio0 = 33;
  bool initStatus;
  void setup()
  {
    initStatus = fmt_initIoc(pin0, out0, EDGE_TYPE_RISING, prio0, callback0);
  }
  void teardown()
  {
  }
  void checkPinNTimes(unsigned n, bool expected)
  {
    for (;n > 0; n--)
    {
      CHECK_EQUAL(expected, fmt_getIocPinState(pin0));
    }
  }
};

TEST(fmt_ioc_test, initWorks)
{
  CHECK_TRUE(initStatus);
}
TEST(fmt_ioc_test, sendPulseIndefinite)
{
  iocTest_sendPinPulse(pin0, true, MAINTAIN_INDEFINITELY);
  checkPinNTimes(50, true);
}
TEST(fmt_ioc_test, sendPulseSeveral)
{
  iocTest_sendPinPulse(pin0, true, 30);
  checkPinNTimes(30, true);
  CHECK_FALSE(fmt_getIocPinState(pin0));
}
TEST(fmt_ioc_test, sendPulseZero)
{
  iocTest_sendPinPulse(pin0, true, 0);
  CHECK_FALSE(fmt_getIocPinState(pin0));
}
/*
TEST(fmt_ioc_test, )
{
  
}
TEST(fmt_ioc_test, )
{
  
}
*/