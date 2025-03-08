#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_comms.h>
#include <fmt_spi.h>
#include <fmt_ioc_test.h>
#include <spi_test.h>
}

extern ARM_DRIVER_SPI Driver_SPI3;
TEST_GROUP(fmt_spi)
{
  bool initSuccess = false;
  uint8_t msgWaitingIocId = 2, clearToSendIocId = 0;
  Top msg = {0};
  spiCfg_t cfg = {
      .spiModuleId = 3,
      .spiModule = &Driver_SPI3,
      .msgWaitingIocId = msgWaitingIocId,
      .clearToSendIocId = clearToSendIocId,
      .baudHz = 1000000,
      .ssActiveLow = false,
      .irqPriority = 16};

  void setup()
  {
    resetCallCounts();
    test_iocSetPinState(clearToSendIocId, true);
    initSuccess = fmt_initSpi(cfg);
  }
  void teardown()
  {
    msg = (Top){0};
  }
};

TEST(fmt_spi, init)
{
  CHECK_TRUE(initSuccess);
  CHECK_FALSE(fmt_getMsg(&msg));
}

TEST(fmt_spi, msgWaitingTriggersTransfer)
{
  test_iocSetPinState(msgWaitingIocId, true);
  test_iocCallCallback(msgWaitingIocId);
  LONGS_EQUAL(1, getCallCount(TRANSFER));
}

// TEST(fmt_spi, notClearToSendBlocksTransfer)
// {

// }

/*
TEST(fmt_spi, getMsgHappy)
{
  // get a message into the rxQueue.
  // comes through the SPI driver we init.
  // 1. place a message into the spi driver.
  // 2. call the msgWaiting callback
  // 3. check that the spi->Transfer was called.
  // 4. call getMsg.

  // test_armSpi_queueIncoming(testMsg);
  test_iocSetPinState(msgWaitingIocId, true);
  test_iocCallCallback(msgWaitingIocId);
  CHECK_TRUE(fmt_getMsg(&msg));
}

TEST(fmt_spi, sendMsgHappy)
{

}

TEST(fmt_spi, getSeveral)
{

}

TEST(fmt_spi, sendSeveral)
{

}

TEST(fmt_spi, getTooMany)
{

}

TEST(fmt_spi, sendTooMany)
{

}

TEST(fmt_spi, iocOutOfSync)
{
// IOC CTS pin doesn't ever fall (or we miss the low-pulse).

}

TEST(fmt_spi, getUninitialized)
{

}

TEST(fmt_spi, sendUninitialized)
{
// Should not crash.
}
*/