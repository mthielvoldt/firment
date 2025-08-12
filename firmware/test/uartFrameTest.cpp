#include <CppUTest/TestHarness.h>
#include <cstring>
#include <cstdlib>

#undef malloc

extern "C"
{
#include <fmt_uart_frame.h>
}

#define PAYLOAD0 100
#define PAYLOAD1 101
#define PAD 10
#define CRC0 200
#define CRC1 201

static volatile int readyCount;

const uint8_t len1Payload[] = {START_CODE, 1, PAYLOAD0, CRC0, CRC1};
const uint8_t maxPayload[UART_PACKET_SIZE] = {START_CODE, MAX_MESSAGE_SIZE_BYTES};

void packetReadyCallback(const uint8_t *data)
{
  readyCount++;
}

TEST_GROUP(fmt_uart_frame_no_setup){};
TEST(fmt_uart_frame_no_setup, noCallback_noCrash)
{
  handleRxSegment(len1Payload);
  handleRxSegment(len1Payload);
  handleRxSegment(len1Payload);
}

TEST_GROUP(fmt_uart_frame)
{
  uint8_t out[UART_PACKET_SIZE];
  void setup(void)
  {
    readyCount = 0;
    memset(out, 0, sizeof(out));
    setPacketReadyCallback(packetReadyCallback);
  };
};

rxParams_t processInput(const uint8_t *input, uint8_t *out, int inputSize)
{
  rxParams_t nextRx = getStartCode(); // resets the state machine.

  while (nextRx.length <= inputSize)
  {
    // simulates call to uart_receive(out, nextRx.length);
    std::memcpy(out + nextRx.position, input, nextRx.length);
    input += nextRx.length;
    inputSize -= nextRx.length;
    nextRx = handleRxSegment(out);
  }
  return nextRx;
}
void checkInputAccepted(const uint8_t input[], uint8_t *out, size_t inputSize)
{
  rxParams_t nextRx = processInput(input, out, inputSize);
  CHECK_EQUAL(0, nextRx.position);
  CHECK_EQUAL(1, readyCount);
  MEMCMP_EQUAL(input, out, inputSize);
}

TEST(fmt_uart_frame, emptyPayload_skipped)
{
  const uint8_t input[] = {START_CODE, 0, CRC0, CRC1};
  rxParams_t nextRx = processInput(input, out, sizeof(input));
  CHECK_EQUAL(0, nextRx.position); // Ready for start of next message.
  CHECK_EQUAL(0, readyCount);      // Invalid msg, so never called callback.
}

TEST(fmt_uart_frame, len1Payload_ok)
{
  const uint8_t input[] = {START_CODE, 1, PAYLOAD0, CRC0, CRC1};
  checkInputAccepted(input, out, sizeof(input));
}
TEST(fmt_uart_frame, len2Payload_ok)
{
  const uint8_t input[] = {START_CODE, 2, PAYLOAD0, PAYLOAD1, PAD, CRC0, CRC1};
  checkInputAccepted(input, out, sizeof(input));
}

TEST(fmt_uart_frame, maxPayload_ok)
{
  const uint8_t input[UART_PACKET_SIZE] = {START_CODE, MAX_MESSAGE_SIZE_BYTES};
  checkInputAccepted(input, out, sizeof(input));
}

TEST(fmt_uart_frame, badStart_dropped)
{
  const uint8_t input[] = {0, 0, 1, PAYLOAD0, CRC0, CRC1};

  rxParams_t finalParams = processInput(input, out, sizeof(input));
  CHECK_EQUAL(0, readyCount);
  CHECK_EQUAL(0, finalParams.position);
}

TEST(fmt_uart_frame, payloadShort1_dropsAtMost1ValidPacket)
{
  const uint8_t input[] =
      {START_CODE, 1, CRC0, CRC1,             // missing a payload byte.
       START_CODE, 1, PAYLOAD0, CRC0, CRC1,  // valid packet
       START_CODE, 1, PAYLOAD0, CRC0, CRC1}; // valid packet
  const uint8_t validPacket[] =
      {START_CODE, 1, PAYLOAD0, CRC0, CRC1}; // valid packet

  rxParams_t rxParams = processInput(input, out, sizeof(input));
  CHECK_COMPARE(readyCount, >=, 1);
  CHECK_EQUAL(0, rxParams.position);
  MEMCMP_EQUAL(validPacket, out, sizeof(validPacket));
}

TEST(fmt_uart_frame, payloadLong1_dropsAtMost1ValidPacket)
{
  const uint8_t input[] =
      {START_CODE, 1, PAYLOAD0, PAYLOAD1, CRC0, CRC1, // extra payload byte.
       START_CODE, 1, PAYLOAD0, CRC0, CRC1,  // valid packet
       START_CODE, 1, PAYLOAD0, CRC0, CRC1}; // valid packet
  const uint8_t validPacket[] =
      {START_CODE, 1, PAYLOAD0, CRC0, CRC1}; // valid packet

  rxParams_t rxParams = processInput(input, out, sizeof(input));
  CHECK_COMPARE(readyCount, >=, 1);
  CHECK_EQUAL(0, rxParams.position);
  MEMCMP_EQUAL(validPacket, out, sizeof(validPacket));
}
/*
TEST(fmt_uart_frame, badStartEmpty_nextOk)
{
}
TEST(fmt_uart_frame, badStartEven_nextOk)
{
}

TEST(fmt_uart_frame, badStartOdd_nextOk)
{
}
TEST(fmt_uart_frame, badStart)
{
}
TEST(fmt_uart_frame, )
{
}
*/
// TEST(fmt_uart, )
// {

// }