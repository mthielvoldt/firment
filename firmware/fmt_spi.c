#include "fmt_spi.h"
#include <ctype.h>

#include "queue.h"
#include <pb_encode.h>
#include <pb_decode.h>

#define BAUD_1MHZ 1000000u

// Initialize with the start sequence in header.
static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue;

// optimization: stop storing the config.
static portPin_t led = {};

extern ARM_DRIVER_SPI Driver_SPI4;
static ARM_DRIVER_SPI *spi1 = &Driver_SPI4;

ARM_SPI_SignalEvent_t callback;

void SPI1_callback(uint32_t event)
{
  switch (event)
  {
  case ARM_SPI_EVENT_TRANSFER_COMPLETE:
    SendNextPacket();
    break;
  case ARM_SPI_EVENT_DATA_LOST:
    /*  Occurs in slave mode when data is requested/sent by master
        but send/receive/transfer operation has not been started
        and indicates that data is lost. Occurs also in master mode
        when driver cannot transfer data fast enough. */
    __BKPT(0); /* Error: Call debugger or replace with custom error handling */
    break;
  case ARM_SPI_EVENT_MODE_FAULT:
    /*  Occurs in master mode when Slave Select is deactivated and
        indicates Master Mode Fault. */
    __BKPT(0); /* Error: Call debugger or replace with custom error handling */
    break;
  }
}

bool initFirment_spi(spiCfg_t cfg)
{
  led = cfg.led;
  XMC_GPIO_Init(cfg.led.port, cfg.led.pin, &gpOutPinConfig);

  spi1->Initialize(SPI1_callback);
  spi1->PowerControl(ARM_POWER_FULL);

  /** Warning:
   * CMSIS says we *may* OR (|) the mode parameters (excluding Miscellaneous
   * controls).  However the XMC4000 library *Requires* the mode parameters are
   * all OR'd.  It gates setting some parameters based on other parameters,
   * which only works if they're all OR'd together and sent in a single call. */
  uint32_t modeParams =
      ARM_SPI_MODE_MASTER |
      ARM_SPI_CPOL0_CPHA1 |
      ARM_SPI_LSB_MSB |
      ARM_SPI_SS_MASTER_HW_OUTPUT |
      ARM_SPI_DATA_BITS(8);
  spi1->Control(modeParams, BAUD_1MHZ); // second arg sets baud.

  spi1->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

  initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      &sendQueue,
      sendQueueStore,
      MAX_SENDER_PRIORITY);
  return true;
}

bool sendMsg(Top message)
{
  uint8_t txPacket[MAX_PACKET_SIZE_BYTES] = {0};
  uint8_t *txMsg = txPacket + HEADER_SIZE_BYTES;
  pb_ostream_t ostream = pb_ostream_from_buffer(txMsg, MAX_MESSAGE_SIZE_BYTES);

  bool success = pb_encode(&ostream, Top_fields, &message);
  if (success)
  {
    txPacket[0] = ostream.bytes_written;
    enqueueBack(&sendQueue, txPacket);

    // Kick off Tx in case it had paused.  Does nada if Spi HW busy.
    SendNextPacket();
  }
  else // message possibly bigger than PAYLOAD_SIZE_BYTES?
  {
    while (1)
      ;
    // transmit an error message instead?
  }

  return success;
}

void SendNextPacket(void)
{
  static uint8_t txPacket[MAX_PACKET_SIZE_BYTES];
  static uint_fast16_t currentPacketSize = 0;

  // sendMsg calls this fn, which is async with spi status, so check spi ready.
  bool spiReady = !spi1->GetStatus().busy;

  /*
  If we've finished transmitting the previous message, but there are still
  messages in the queue, pull a new packet out of the queue and keep Txing. */

  if (spiReady && dequeueFront(&sendQueue, txPacket))
  {
    currentPacketSize = txPacket[0] + HEADER_SIZE_BYTES;
    XMC_GPIO_ToggleOutput(led.port, led.pin);
    spi1->Send(txPacket, currentPacketSize);
  }
}

Top decode(uint8_t *buffer, size_t msg_len)
{
  Top msg = Top_init_zero;
  /* Create a stream that reads from the buffer. */
  pb_istream_t stream = pb_istream_from_buffer(
      buffer + HEADER_SIZE_BYTES, msg_len);

  /* Now we are ready to decode the message. */
  if (pb_decode(&stream, Top_fields, &msg))
  {
    // handle an error
  }
  return msg;
}

void ISR_handleRx_spi(void)
{
}

/** OPTIMIZATION POSSIBILITIES
 * - Just store the messages (payload) not the whole packet (with header) in Q.
 *
 */