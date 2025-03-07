// TODO: there is an issue where the ESP resets and the XMC gets stuck not
// sending anything.
// Oberved 4f56582 (webgl-plot-explore) webgl Plot component showing sin with noise

#include "fmt_spi.h"
#include "fmt_comms.h" // fmt_sendMsg fmt_getMsg
#include <core_port.h> // NVIC_...()  __BKPT()
#include <cmsis_gcc.h>
#include "fmt_crc.h"
#include "fmt_ioc.h" // fmt_initIoc
#include "queue.h"
#include <pb_encode.h>
#include <pb_decode.h>

// globals for debug
uint32_t spiTxDropCount = 0;
uint32_t spiRxDropCount = 0;

// Initialize with the start sequence in header.
static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue;

static uint8_t rxQueueStore[MAX_PACKET_SIZE_BYTES * RX_QUEUE_LENGTH];
static queue_t rxQueue;
static uint8_t rxPacket[MAX_PACKET_SIZE_BYTES] = {0};

static ARM_DRIVER_SPI *spi;
static uint8_t clearToSendIocId; // An Interrupt-on-Change config.
static uint8_t msgWaitingIocId;

extern FMT_DRIVER_CRC Driver_CRC0;
static FMT_DRIVER_CRC *crc = &Driver_CRC0;

ARM_SPI_SignalEvent_t callback;

/* Declarations of private functions */
static void SendNextPacket(void);
static void spiEventHandlerISR(uint32_t event);
static uint32_t getCRCPosition(uint8_t *lengthPrefixedBuffer);
static void addCRC(uint8_t packet[MAX_PACKET_SIZE_BYTES]);

/* Public function definitions */
bool fmt_initSpi(spiCfg_t cfg)
{
  spi = cfg.spiModule;
  clearToSendIocId = cfg.clearToSendIocId;
  msgWaitingIocId = cfg.msgWaitingIocId;
  uint32_t spiEventIRQn = port_getSpiEventIRQn(cfg.spiModuleId);
  if (spiEventIRQn == 0)
    return false;

  spi->Initialize(spiEventHandlerISR);
  spi->PowerControl(ARM_POWER_FULL);

  /* The CMSIS SPI driver interface omits any control over the priority of the
  data-ready ISR.  This should be project-specified. The above calls to
  Initialize and PowerControl sets a default priority, we'll re-set it now.*/

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), cfg.irqPriority, 0U);
  NVIC_SetPriority(spiEventIRQn, encodedPrio);

  crc->Initialize();
  crc->PowerControl(ARM_POWER_FULL);

  fmt_initIoc(cfg.clearToSendIocId, cfg.clearToSendIocOut, EDGE_TYPE_RISING, 
    cfg.irqPriority, subClearToSendISR
  );

  fmt_initIoc(cfg.msgWaitingIocId, cfg.msgWaitingIocOut, EDGE_TYPE_RISING,
    cfg.irqPriority, subMsgWaitingISR
  );

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
  spi->Control(modeParams, cfg.baudHz);

  spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

  initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      &sendQueue,
      sendQueueStore,
      MAX_SENDER_PRIORITY);
  initQueue(
      MAX_PACKET_SIZE_BYTES,
      RX_QUEUE_LENGTH,
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
    if (!enqueueSuccess)
    {
      spiTxDropCount++;
      // __BKPT(4);
    }

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
    if (!success)
    {
      __BKPT(5);
    }
  }
  return success;
}

static void SendNextPacket(void)
{
  static uint8_t txPacket[MAX_PACKET_SIZE_BYTES];

  /*
  If we've finished transmitting the previous message, but there are still
  messages in the queue, pull a new packet out of the queue and keep Txing. */

  // fmt_sendMsg calls this fn at any time, so check spi ready.
  bool spiReady = !spi->GetStatus().busy;
  if (spiReady)
  {
    bool clearToSend = fmt_getIocPinState(clearToSendIocId);

    if (clearToSend)
    {
      bool txWaiting = numItemsInQueue(&sendQueue);
      bool rxWaiting = fmt_getIocPinState(msgWaitingIocId);

      if (txWaiting || rxWaiting)
      {
        if (txWaiting)
        {
          dequeueFront(&sendQueue, txPacket);
        }
        else // Implies rxWaiting.
        {
          // Send empty (zero-length) message.
          memset(txPacket, 0, MAX_PACKET_SIZE_BYTES);
        }

        /** Note: If application has multiple subs, this driver will need the
         * "MultiSlave wrapper" <SPI_MultiSlave.h> added underneath it.
         * see https://arm-software.github.io/CMSIS-Driver/latest/driver_SPI.html */
        spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);

        /** Note: in SPI we're using fixed-width data frames to simplify staying
         * synchronized in the presence of data corruption.
         * Note: this call only starts the transfer, it doesn't block.*/
        spi->Transfer(txPacket, rxPacket, MAX_PACKET_SIZE_BYTES);

        /* Some test code for counting bytes that get through.*/
        // static uint8_t substitutePacket[] = {
        //     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        //     10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        //     20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        //     30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        //     40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
        //     50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        //     60, 61, 62, 63, 64, 65, 66, 67, 68, 69};
        // spi->Send(substitutePacket, MAX_PACKET_SIZE_BYTES);
      }
    }
    else // not clear to send.  Enable cts ISR to restart when cts re-asserted.
    {
      fmt_enableIoc(clearToSendIocId);
    }
  }
}

void subMsgWaitingISR(void)
{
  SendNextPacket();
}

void subClearToSendISR(void)
{
  // disable self (one-shot behavior)
  fmt_disableIoc(clearToSendIocId);

  // Re-start the transaction chain.
  SendNextPacket();
}

/* PRIVATE (static) functions */

/** Get the position of the CRC in bytes from the first element of the packet.
 * CRC position must be 16-bit aligned (even number) for hardware CRC engines.
 * so if the length of the buffer (including length prefix,) is odd, there
 * will be a byte of padding, between the buffer and the CRC.
 * The value of the padding byte (if present) will be checked by the CRC, but it
 * has no effect on the decoded message.
 */
static uint32_t getCRCPosition(uint8_t *lengthPrefixedBuffer)
{
  return ((lengthPrefixedBuffer[0] + PREFIX_SIZE_BYTES + 1) >> 1) << 1;
}

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

/** Event Handler ISR
 * Runs when the SPI module detects one of the following events:
 * - Transaction completes
 * - Incoming Data is Lost (Should not fire, applies in sub mode only)
 * - Mode Fault (sub-select line deactivated at invalid time)
 */
static void spiEventHandlerISR(uint32_t event)
{
  switch (event)
  {
  case ARM_SPI_EVENT_TRANSFER_COMPLETE:
    spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
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
        bool success = enqueueBack(&rxQueue, rxPacket);
        if (!success)
        {
          spiRxDropCount++;
          __BKPT(6);
        }
      }
    }
    /* This will trigger a Send as soon as CTS pin has a rising edge.
    We do this instead of calling SendNextPacket() because the ESP doesn't lower
    CTS until a few us AFTER a transaction completes, so this event handler gets
    there too early, while CTS is still high, but the ESP isn't actually ready.
    One consequence is that we now depend on the CTS signal pulsing low between
    each transaction.*/
    fmt_enableIoc(clearToSendIocId);
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