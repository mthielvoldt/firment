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
extern FMT_DRIVER_CRC Driver_CRC0; // Need to worry about concurrent access?
TEST_GROUP(fmt_spi)
{
  bool initSuccess = false;
  uint8_t msgWaitingIocId = 2, clearToSendIocId = 0;
  Top emptyMsg, validMsg;
  uint8_t validPacket[MAX_PACKET_SIZE_BYTES];
  const spiCfg_t cfg = {
      .spiModuleId = 3,
      .spiModule = &Driver_SPI3,
      .msgWaitingIocId = msgWaitingIocId,
      .clearToSendIocId = clearToSendIocId,
      .baudHz = 1000000,
      .ssActiveLow = false,
      .irqPriority = 16};

  void setup()
  {
    emptyMsg = (Top){0};
    validMsg = (const Top){
        .which_sub = Top_Log_tag,
        .sub = {.Log = {.count = 1, .text = "Hey.", .value = 500}}};

    memset(validPacket, 0, sizeof(validPacket));
    messageToValidPacket(validMsg, validPacket);
    spiTest_reset();
    iocTest_setPinState(clearToSendIocId, true);
    initSuccess = fmt_initSpi(cfg);
  }
  void teardown()
  {
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
  CHECK_FALSE(fmt_getMsg(&emptyMsg));
}

TEST(fmt_spi, msgWaitingTriggersTransfer)
{
  // Trigger msg-waiting
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);
  CHECK_EQUAL(1, getCallCount(INITIALIZE));
  CHECK_EQUAL(1, getCallCount(TRANSFER));
}

TEST(fmt_spi, getMsgHappy)
{
  // Stage this packet as incoming data
  spiTest_queueIncoming(validPacket);

  // Trigger msg-waiting
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);

  // Exactly one message is pending
  CHECK_TRUE(fmt_getMsg(&emptyMsg));
  CHECK_FALSE(fmt_getMsg(&emptyMsg));
}

TEST(fmt_spi, initClearsPendingMessages)
{
  // Thest two actions put a message in the rxQueue (see getMsgHappy)
  spiTest_queueIncoming(validPacket);
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);

  // With a message in rxQueue, re-initialize spi.
  initSuccess = fmt_initSpi(cfg);

  CHECK_TRUE(initSuccess);
  // No message should be there.
  CHECK_FALSE(fmt_getMsg(&emptyMsg));
}

TEST(fmt_spi, sendMsgHappy)
{
  CHECK_TRUE(fmt_sendMsg(validMsg));
  const uint8_t *sentData = spiTest_getLastSent();
  MEMCMP_EQUAL(validPacket, sentData, sizeof(validPacket));
}

TEST(fmt_spi, clearToSendBlocksMsgWaiting)
{
  // If CTS is low, a rising edge on msg-waiting doesn't start a transfer.
  iocTest_setPinState(clearToSendIocId, false);
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);
  CHECK_EQUAL(1, getCallCount(INITIALIZE));
  CHECK_EQUAL(0, getCallCount(TRANSFER));
}

TEST(fmt_spi, ctsBlocksSendMsg)
{
  iocTest_setPinState(clearToSendIocId, false);
  CHECK_TRUE(fmt_sendMsg(validMsg));
  CHECK_EQUAL(0, getCallCount(TRANSFER));
}

/*
TEST(fmt_spi, maintainedMsgWaitingStacksTransfers)
{
// If CTS and msgWaiting remain high, transfers should cascade indefinitely.
// This will require new feature in test-ioc for counting pin-reads.
}

TEST(fmt_spi, notClearToSendBlocksTransfer)
{

}

TEST(fmt_spi, crcErrorsCounted)
{
// send a message with a bad CRC
// observe crc error count go up in status report.

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