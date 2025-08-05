#include "fmt_comms.h"
#include "fmt_sizes.h"
#include "fmt_transport.h" // transport_sendNext(), FMT_BUILTIN_CRC
#include "fmt_assert.h"
#include "queue.h"

#include <crc_mcuDetails.h> // FMT_BUILTIN_CRC

#if !FMT_BUILTIN_CRC
#include "fmt_crc.h"
#endif

#include <pb_encode.h>
#include <pb_decode.h>
#include <cmsis_gcc.h> // __BKPT()
#include <stdint.h>
#include <stdbool.h>

static uint32_t txDropCount = 0, rxDropCount = 0, badCrcCount = 0;

// Initialize with the start sequence in header.
static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue[1];
static uint8_t rxQueueStore[MAX_PACKET_SIZE_BYTES * RX_QUEUE_LENGTH];
static queue_t rxQueue[1];

#if !FMT_BUILTIN_CRC
extern FMT_DRIVER_CRC Driver_CRC0;
static FMT_DRIVER_CRC *crc = &Driver_CRC0;
#endif

static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES]);
static bool checkCRCMatch(const uint8_t packet[]);

/**
 * Passes queues to underlying transport (spi, uart)
 *
 *
 */
static void acceptMsgIfValid(const uint8_t rxPacket[])
{
  // check CRC here so we don't consume Rx queue with errors.
  bool crcMatch = checkCRCMatch(rxPacket);

  if (crcMatch)
  {
    bool success = enqueueBack(rxQueue, rxPacket);
    if (!success)
    {
      rxDropCount++;
    }
  }
  else
  {
    badCrcCount++;
  }
}

static bool fmt_sendMsg_prod(Top message)
{
  uint8_t txPacket[MAX_PACKET_SIZE_BYTES] = {0};
  uint8_t *txMsg = txPacket + LENGTH_SIZE_BYTES;
  pb_ostream_t ostream = pb_ostream_from_buffer(txMsg, MAX_MESSAGE_SIZE_BYTES);

  bool success = pb_encode(&ostream, Top_fields, &message);
  if (success)
  {
    txPacket[LENGTH_POSITION] = ostream.bytes_written;
    addCRC(txPacket);
    bool enqueueSuccess = enqueueBack(sendQueue, txPacket);
    if (!enqueueSuccess)
    {
      txDropCount++;
      // __BKPT(4);
    }

    // Kick off Tx in case it had paused.  Does nada if Spi HW busy.
    fmt_startTxChain();
  }
  else // message possibly bigger than PAYLOAD_SIZE_BYTES?
  {
    __BKPT(2); // TODO: Increment err counter, Send error message.
  }

  return success;
}
bool (*fmt_sendMsg)(Top message) = fmt_sendMsg_prod;

static bool fmt_getMsg_prod(Top *message)
{
  bool success = false;
  if (numItemsInQueue(rxQueue) > 0)
  {
    uint8_t packet[MAX_PACKET_SIZE_BYTES];
    dequeueFront(rxQueue, packet);
    uint8_t messageLen = packet[LENGTH_POSITION];

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(&packet[1], messageLen);
    /* Now we are ready to decode the message. */
    success = pb_decode(&stream, Top_fields, message);
    if (!success)
    {
      __BKPT(5);
    }
  }
  return success;
}
bool (*fmt_getMsg)(Top *message) = fmt_getMsg_prod;

bool fmt_initComms(void)
{
  ASSERT_SUCCESS(initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      sendQueue,
      sendQueueStore,
      MAX_SENDER_PRIORITY));
  ASSERT_SUCCESS(initQueue(
      MAX_PACKET_SIZE_BYTES,
      RX_QUEUE_LENGTH,
      rxQueue,
      rxQueueStore,
      MAX_SENDER_PRIORITY));

#if !FMT_BUILTIN_CRC
  ASSERT_ARM_OK(crc->Initialize());
  ASSERT_ARM_OK(crc->PowerControl(ARM_POWER_FULL));
#endif
  ASSERT_SUCCESS(fmt_linkTransport(sendQueue, acceptMsgIfValid));
  return true;
}

#if FMT_BUILTIN_CRC
static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES]) {}
static bool checkCRCMatch(const uint8_t packet[]) { return true; }

#else
static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES])
{
  uint32_t crcPosition = getCRCPosition(packet);
  uint16_t computedCRC;
  int32_t result = crc->ComputeCRC(packet, crcPosition, &computedCRC);
  *(uint16_t *)(&packet[crcPosition]) = computedCRC;
  if (result != ARM_DRIVER_OK)
  {
    __BKPT(0);
  }
}

static bool checkCRCMatch(const uint8_t packet[])
{
  uint32_t crcPosition = getCRCPosition(packet);
  uint16_t result;
  int32_t status = crc->ComputeCRC(packet, crcPosition, &result);
  bool crcMatch = (result == *(uint16_t *)(&packet[crcPosition]));
  return status == ARM_DRIVER_OK; //  && crcMatch;
}
#endif