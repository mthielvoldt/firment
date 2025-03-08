#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_comms.h>
#include <fmt_spi.h>
#include <fmt_crc.h>
#include <fmt_ioc_test.h>
#include <spi_test.h>
#include <pb_encode.h>
}

extern ARM_DRIVER_SPI Driver_SPI3;
extern FMT_DRIVER_CRC Driver_CRC0;  // Need to worry about concurrent access?
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
    spiTest_reset();
    test_iocSetPinState(clearToSendIocId, true);
    initSuccess = fmt_initSpi(cfg);
  }
  void teardown()
  {
    msg = (Top){0};
  }
  
  size_t messageToValidPacket(Top msg, uint8_t packet[])
  {
    // Pack buffer
  pb_ostream_t ostream = pb_ostream_from_buffer(packet, MAX_MESSAGE_SIZE_BYTES);
  bool success = pb_encode_delimited(&ostream, Top_fields, &msg);

  // append CRC
  uint32_t crcPosition = ((packet[0] + PREFIX_SIZE_BYTES + 1) >> 1) << 1;
  uint16_t computedCRC;
  int32_t result = Driver_CRC0.ComputeCRC(packet, crcPosition, &computedCRC);
  *(uint16_t *)(&packet[crcPosition]) = computedCRC;
  return crcPosition + CRC_SIZE_BYTES;
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
  LONGS_EQUAL(1, getCallCount(INITIALIZE));
  LONGS_EQUAL(1, getCallCount(TRANSFER));
  
}

// TEST(fmt_spi, notClearToSendBlocksTransfer)
// {

// }

TEST(fmt_spi, getMsgHappy)
{
  // get a message into the rxQueue.
  // comes through the SPI driver we init.
  // 1. place a message into the spi driver.
  // 2. call the msgWaiting callback
  // 3. check that the spi->Transfer was called.
  // 4. call getMsg.

  // Construct a message
  msg = (Top){
    .which_sub = Top_Log_tag, 
    .sub = {.Log = {.count = 1, .text = "Hey."}}
  };
  uint8_t packet[MAX_PACKET_SIZE_BYTES] = {0};
  size_t packetLength = messageToValidPacket(msg, packet);
  
  // Stage this packet as incoming data
  spiTest_queueIncoming(packet, packetLength);

  // Trigger msg-waiting
  test_iocSetPinState(msgWaitingIocId, true);
  test_iocCallCallback(msgWaitingIocId);

  // Exactly one message is pending
  CHECK_TRUE(fmt_getMsg(&msg));
  CHECK_FALSE(fmt_getMsg(&msg));
}

TEST(fmt_spi, initClearsPendingMessages)
{
  // 
}

TEST(fmt_spi, clearToSendBlocksMsgWaiting)
{
  // If CTS is low, a rising edge on msg-waiting doesn't start a transfer.
}

TEST(fmt_spi, maintainedMsgWaitingStacksTransfers)
{
  // If CTS and msgWaiting remain high, transfers should cascade indefinitely.
}
/*
TEST(fmt_spi, crcErrorsCounted)
{
  // send a message with a bad CRC
  // observe crc error count go up in status report.

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