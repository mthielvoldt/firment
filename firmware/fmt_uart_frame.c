#include "fmt_uart_frame.h"
#include <string.h>

static rxCallback_t packetReady_cb = NULL;
static enum {
  AWAITING_START_CODE,
  AWAITING_LENGTH_BYTE,
  AWAITING_PAYLOAD,
} rxState = AWAITING_START_CODE;

static inline bool lengthValid(const uint8_t *packet);
static rxParams_t getLengthPrefix(void);
static rxParams_t getPayload(const uint8_t *rxPacket);

void setPacketReadyCallback(rxCallback_t rxCallback)
{
  packetReady_cb = rxCallback;
}

rxParams_t handleRxSegment(const uint8_t *packet)
{
  switch (rxState)
  {
  case AWAITING_START_CODE:
  {
    if (memcmp(packet, (const uint8_t[]){START_CODE}, START_CODE_SIZE) == 0)
      return getLengthPrefix();
    break;
  }
  case AWAITING_LENGTH_BYTE:
  {
    if (lengthValid(packet))
      return getPayload(packet);
    break;
  }
  case AWAITING_PAYLOAD:
  {
    // Message fully present now, hand off.
    if (packetReady_cb)
      packetReady_cb(&packet[LENGTH_POSITION]);
    break;
  }
  }
  return getStartCode();
}

/* private functions */

rxParams_t getStartCode(void)
{
  rxState = AWAITING_START_CODE;
  return (rxParams_t){START_CODE_POSITION, START_CODE_SIZE};
}
static rxParams_t getLengthPrefix(void)
{
  rxState = AWAITING_LENGTH_BYTE;
  return (rxParams_t){LENGTH_POSITION, LENGTH_SIZE_BYTES};
}
static rxParams_t getPayload(const uint8_t *rxPacket)
{
  uint32_t payloadLen = getPacketLength(rxPacket) - PAYLOAD_POSITION;
  rxState = AWAITING_PAYLOAD;
  return (rxParams_t){PAYLOAD_POSITION, payloadLen};
}

static inline bool lengthValid(const uint8_t *packet)
{
  uint8_t length = packet[LENGTH_POSITION];
  return (length > 0) && (length <= MAX_MESSAGE_SIZE_BYTES);
}
