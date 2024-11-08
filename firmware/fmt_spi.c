#include "fmt_spi.h"
#include <cmsis_gcc.h> // __BKPT()
#include "fmt_crc.h"

#include "queue.h"
#include <pb_encode.h>
#include <pb_decode.h>

#define BAUD_1MHZ 1000000u

// Initialize with the start sequence in header.
static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue;

static uint8_t rxQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t rxQueue;
static uint8_t rxPacket[MAX_PACKET_SIZE_BYTES] = {0};

extern ARM_DRIVER_SPI Driver_SPI4; // Need to not hard-code SPI4.
static ARM_DRIVER_SPI *spi1 = &Driver_SPI4;

extern FMT_DRIVER_CRC Driver_CRC0;
static FMT_DRIVER_CRC *crc = &Driver_CRC0;

ARM_SPI_SignalEvent_t callback;

/* Declarations of private functions */
static void SendNextPacket(void);
static void SPI1_callback(uint32_t event);
static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES]);

/* Public function definitions */
bool fmt_initSpi(spiCfg_t cfg)
{
  spi1->Initialize(SPI1_callback);
  spi1->PowerControl(ARM_POWER_FULL);

  crc->Initialize();
  crc->PowerControl(ARM_POWER_FULL);

  /** Warning:
   * CMSIS says we *may* OR (|) the mode parameters (excluding Miscellaneous
   * controls).  However the XMC4000 library *Requires* the mode parameters are
   * all OR'd.  It gates setting some parameters based on other parameters,
   * which only works if they're all OR'd together and sent in a single call. */
  uint32_t modeParams =
      ARM_SPI_MODE_MASTER |
      ARM_SPI_CPOL0_CPHA1 |
      ARM_SPI_LSB_MSB |
      ARM_SPI_SS_MASTER_SW |
      ARM_SPI_DATA_BITS(8);
  spi1->Control(modeParams, BAUD_1MHZ); // second arg sets baud.

  spi1->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

  initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      &sendQueue,
      sendQueueStore,
      MAX_SENDER_PRIORITY);
  initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      &rxQueue,
      rxQueueStore,
      MAX_SENDER_PRIORITY);
  return true;
}

bool fmt_sendMsg(Top message)
{
  uint8_t txPacket[MAX_PACKET_SIZE_BYTES] = {0};
  uint8_t *txMsg = txPacket + PREFIX_SIZE_BYTES;
  pb_ostream_t ostream = pb_ostream_from_buffer(txMsg, MAX_MESSAGE_SIZE_BYTES);

  bool success = pb_encode(&ostream, Top_fields, &message);
  if (success)
  {
    txPacket[0] = ostream.bytes_written;
    addCRC(txPacket);
    bool enqueueSuccess = enqueueBack(&sendQueue, txPacket);

    // Kick off Tx in case it had paused.  Does nada if Spi HW busy.
    SendNextPacket();
  }
  else // message possibly bigger than PAYLOAD_SIZE_BYTES?
  {
    __BKPT(2); // TODO: Increment err counter, Send error message.
  }

  return success;
}

bool fmt_getMsg(Top *message)
{
  bool success = false;
  if (numItemsInQueue(&rxQueue) > 0)
  {
    uint8_t packet[MAX_PACKET_SIZE_BYTES];
    dequeueFront(&rxQueue, packet);
    uint8_t messageLen = packet[0];

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(&packet[1], messageLen);
    /* Now we are ready to decode the message. */
    success = pb_decode(&stream, Top_fields, message);
  }
  return success;
}

static void SendNextPacket(void)
{
  static uint8_t txPacket[MAX_PACKET_SIZE_BYTES];

  // fmt_sendMsg calls this fn, which is async with spi status, so check spi ready.
  bool spiReady = !spi1->GetStatus().busy;

  /*
  If we've finished transmitting the previous message, but there are still
  messages in the queue, pull a new packet out of the queue and keep Txing. */

  if (spiReady && dequeueFront(&sendQueue, txPacket))
  {
    /** Note: If application has multiple subs, this driver will need the
     * "MultiSlave wrapper" <SPI_MultiSlave.h> added underneath it.
     * see https://arm-software.github.io/CMSIS-Driver/latest/driver_SPI.html */
    spi1->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);

    /** Note: in SPI we're using fixed-width data frames to simplify staying
     * synchronized in the presence of data corruption.
     * Note: this call only starts the transfer, it doesn't block.*/
    spi1->Transfer(txPacket, rxPacket, MAX_PACKET_SIZE_BYTES);

    /* Some test code for counting bytes that get through.*/
    // static uint8_t substitutePacket[] = {
    //     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    //     10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    //     20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    //     30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    //     40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    //     50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    //     60, 61, 62, 63, 64, 65, 66, 67, 68, 69};
    // spi1->Send(substitutePacket, MAX_PACKET_SIZE_BYTES);
  }
}

/* PRIVATE (static) functions */
static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES])
{
  uint32_t crcPosition = getCRCPosition(packet);
  uint16_t computedCRC;
  int32_t result =
      crc->ComputeCRC(packet, crcPosition, &computedCRC);
  *(uint16_t *)(&packet[crcPosition]) = computedCRC;
  if (result != ARM_DRIVER_OK)
  {
    __BKPT(0);
  }
}

static void SPI1_callback(uint32_t event)
{
  switch (event)
  {
  case ARM_SPI_EVENT_TRANSFER_COMPLETE:
    spi1->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    if (rxPacket[0])
    {
      // check CRC here so we don't consume Rx queue with errors.
      // Todo: pad messages so all lengths are even.
      uint16_t result;

      uint32_t crcPosition = getCRCPosition(rxPacket);
      int32_t status = crc->ComputeCRC(rxPacket, crcPosition, &result);
      if (status == ARM_DRIVER_OK &&
          result == *(uint16_t *)(&rxPacket[crcPosition]))
      {
        enqueueBack(&rxQueue, rxPacket);
      }
    }
    SendNextPacket();
    break;
  case ARM_SPI_EVENT_DATA_LOST:
    /*  Occurs in slave mode when data is requested/sent by master
        but send/receive/transfer operation has not been started
        and indicates that data is lost. Occurs also in master mode
        when driver cannot transfer data fast enough. */
    __BKPT(0); /* TODO: Handle this error */
    break;
  case ARM_SPI_EVENT_MODE_FAULT:
    /*  Occurs in master mode when Slave Select is deactivated and
        indicates Master Mode Fault. */
    __BKPT(1); /* TODO: Handle this error */
    break;
  }
}

/** OPTIMIZATION POSSIBILITIES
 * - Just store the messages (payload) not the whole packet (with header) in Q.
 *
 */