#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_comms.h>
#include <fmt_spi.h>
#include <fmt_crc.h>
#include <ioc_spy.h>
#include <comm_test.h>
#include <pb_encode.h>
}

#define COUNT_SOME 5
#define COUNT_MORE_THAN_SOME 15
#define COUNT_ALOT 500
#define COUNT_MORE_THAN_ALOT 1000

extern ARM_DRIVER_SPI Driver_SPI3;
extern FMT_DRIVER_CRC Driver_CRC0; // Need to worry about concurrent access?

TEST_GROUP(fmt_spi)
{
  bool initSuccess;
  uint8_t msgWaitingIocId = 0, clearToSendIocId = 1;
  Top emptyMsg, validMsg;
  uint8_t validPacket[MAX_PACKET_SIZE_BYTES];
  const spiCfg_t cfg = {
      .spiDriverId = 3,
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
    initSuccess = true;

    memset(validPacket, 0, sizeof(validPacket));
    messageToValidPacket(validMsg, validPacket);
    commTest_reset();
    iocTest_setPinState(clearToSendIocId, true);
    fmt_startTxChain = spi_startTxChain;
    fmt_linkTransport = spi_linkTransport;
    initSuccess = fmt_initSpi(cfg);
    initSuccess = initSuccess && fmt_initComms();
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
    uint32_t crcPosition = getCRCPosition(packet);
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
  commTest_queueIncoming(validPacket);

  // Trigger msg-waiting
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);

  // Exactly one message is pending
  CHECK_TRUE(fmt_getMsg(&emptyMsg));
  CHECK_FALSE(fmt_getMsg(&emptyMsg));
}

TEST(fmt_spi, initClearsPendingMessages)
{
  // Thest two actions put a message in the rxQueue (see getMsgHappy)
  commTest_queueIncoming(validPacket);
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);

  // With a message in rxQueue, re-initialize spi.
  initSuccess = fmt_initComms();

  CHECK_TRUE(initSuccess);
  // No message should be there.
  CHECK_FALSE(fmt_getMsg(&emptyMsg));
}

TEST(fmt_spi, sendMsgHappy)
{
  CHECK_TRUE(fmt_sendMsg(validMsg));
  const uint8_t *sentData = commTest_getLastSent();
  MEMCMP_EQUAL(validPacket, sentData, sizeof(validPacket));
}

TEST(fmt_spi, notClearToSendBlocksMsgWaiting)
{
  // If CTS is low, a rising edge on msg-waiting doesn't start a transfer.
  iocTest_setPinState(clearToSendIocId, false);
  iocTest_sendPinPulse(msgWaitingIocId, true, MAINTAIN_INDEFINITELY);
  CHECK_EQUAL(1, getCallCount(INITIALIZE));
  CHECK_EQUAL(0, getCallCount(TRANSFER));
}

TEST(fmt_spi, notCTSBlocksSendMsg)
{
  iocTest_setPinState(clearToSendIocId, false);
  CHECK_TRUE(fmt_sendMsg(validMsg));
  CHECK_EQUAL(0, getCallCount(TRANSFER));
}

TEST(fmt_spi, transfersContinueUntilMsgWaitingDrops)
{
  /** @note This is not ideal behavior.  Ideally, CTS pin would not need to
   * pulse, and transfers would cascade as fast as module allows.
   */
  // If msgWaiting remain high, transfers should follow CTS triggers.
  iocTest_sendPinPulse(msgWaitingIocId, true, COUNT_SOME);

  // Pulse CTS pin plenty of times
  for (int i = 0; i < COUNT_MORE_THAN_SOME; i++)
    iocTest_sendPinPulse(clearToSendIocId, true, MAINTAIN_INDEFINITELY);

  CHECK_EQUAL(COUNT_SOME, getCallCount(TRANSFER));
}
TEST(fmt_spi, transfersContinueUntilCTSStopsPulsing)
{
  // If CTS and msgWaiting remain high, transfers should cascade indefinitely.
  iocTest_sendPinPulse(msgWaitingIocId, true, COUNT_MORE_THAN_SOME);

  // Pulse CTS pin plenty of times
  for (int i = 0; i < COUNT_SOME; i++)
    iocTest_sendPinPulse(clearToSendIocId, true, MAINTAIN_INDEFINITELY);

  CHECK_EQUAL(COUNT_SOME + 1, getCallCount(TRANSFER));
}

TEST_GROUP(fmt_spi_no_setup) {

};
TEST(fmt_spi_no_setup, outOfOrderInit_NoCrash)
{
  spiCfg_t cfg = {
    .spiDriverId = 3,
    .spiModule = &Driver_SPI3,
    .baudHz = 1000000,
  };
  fmt_initComms();
  fmt_initSpi(cfg);
}
/*

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