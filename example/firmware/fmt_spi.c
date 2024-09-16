#include "fmt_spi.h"
#include <ctype.h>

#include "queue.h"
#include <pb_encode.h>
#include <pb_decode.h>

// TODO: write this block in init.
static XMC_USIC_CH_t *const channel = XMC_USIC2_CH0;
static portPin_t ledPin = {.port = XMC_GPIO_PORT0, .pin = 13U};
static portPin_t espSelect = {.port = XMC_GPIO_PORT3, .pin = 10U};

// Initialize with the start sequence in header.

static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue;

// TODO: take in board parameters from caller.
bool initFirment_spi(void)
{
  // TODO: generalize this to any processor.
  /* Port 3, pins:
  7:  MISO
  8:  MOSI
  9:  SCK
  10: CS
  */
  const XMC_SPI_CH_CONFIG_t config = {
      .baudrate = 1000000,
      .normal_divider_mode = true,
      .bus_mode = XMC_SPI_CH_BUS_MODE_MASTER,
      .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
      .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE,
  };
  XMC_USIC_CH_t *const channel = XMC_USIC2_CH0;
  XMC_GPIO_CONFIG_t spiOutPinConfig = {
      .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1,
      .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
      .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM,
  };

  spiChannel_t spi20Pins = {
      .channel = channel,
      .channel_config = config,
      .mosi = {.port = XMC_GPIO_PORT3, .pin = 8U},
      .miso = {.port = XMC_GPIO_PORT3, .pin = 7U},
      .sclkout = {.port = XMC_GPIO_PORT3, .pin = 9U},
      .miso_config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE},
      .input_source = USIC_INPUT_C,
      .word_length = 16,
      .frame_length = 32,
  };

  const bool initBrg = true; // Automatically configure the baudrate generator.

  XMC_GPIO_Init(ledPin.port, ledPin.pin, &gpOutPinConfig);
  XMC_GPIO_Init(espSelect.port, espSelect.pin, &gpOutPinConfig);

  XMC_SPI_CH_InitEx(channel, &config, initBrg);
  XMC_SPI_CH_SetWordLength(channel, spi20Pins.word_length);
  XMC_SPI_CH_SetFrameLength(channel, spi20Pins.frame_length);
  XMC_SPI_CH_SetInputSource(
      spi20Pins.channel, XMC_SPI_CH_INPUT_DIN0, (uint8_t)spi20Pins.input_source);
  XMC_SPI_CH_SetBitOrderLsbFirst(channel);

  // Configure the FIFO

  XMC_SPI_CH_Start(channel);
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)spi20Pins.sclkout.port,
      (uint8_t)spi20Pins.sclkout.pin,
      &spiOutPinConfig);
  /* Configure the input pin properties */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)spi20Pins.miso.port,
      (uint8_t)spi20Pins.miso.pin,
      &(spi20Pins.miso_config));
  /* Configure the output pin properties */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)spi20Pins.mosi.port,
      (uint8_t)spi20Pins.mosi.pin,
      &spiOutPinConfig);

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
  uint8_t txPacket[MAX_PACKET_SIZE_BYTES] = {0x00, 0xEF, 0xCD, 0xAB};
  uint8_t *txMsg = txPacket + HEADER_SIZE_BYTES;
  pb_ostream_t ostream = pb_ostream_from_buffer(txMsg, MAX_MESSAGE_SIZE_BYTES);

  bool success = pb_encode(&ostream, Top_fields, &message);
  if (success)
  {
    txPacket[0] = ostream.bytes_written;
    enqueueBack(&sendQueue, txPacket);
  }
  else // message possibly bigger than PAYLOAD_SIZE_BYTES?
  {
    while (1)
      ;
    // transmit an error message instead?
  }

  return success;
}

void ISR_handleTx_spi(void)
{
  static uint8_t txPacket[MAX_PACKET_SIZE_BYTES];
  static uint_fast16_t currentPacketSize = 0;
  static uint_fast16_t bytesSent = 0;

  // look for HW-ready flag
  // check that there is stuff in the queue
  if (bytesSent >= currentPacketSize)
  {
    if (dequeueFront(&sendQueue, txPacket))
    {
      // dequeue successful.  Starting tx of new packet.
      currentPacketSize = txPacket[0] + HEADER_SIZE_BYTES;
      bytesSent = 0;
      XMC_GPIO_ToggleOutput(ledPin.port, ledPin.pin);
      XMC_GPIO_SetOutputLow(espSelect.port, espSelect.pin);
    }
  }

  // passes packet size by 1 for odd-size packets.  That's ok. 
  // MAX_PACKET_SIZE_BYTES is even, so we'll never reach past the buffer.

  // Auto-cleared when data transfers from TXBUF to TX shift reg (in SSC?)
  // auto-set when TXBUF is written to.  Making this the easiest!
  #define TX_DATA_VALID (channel->TCSR & USIC_CH_TCSR_TDV_Msk)
  #define TXBUF_INTERRUPT_FLAG (channel->PSR & USIC_CH_PSR_TBIF_Msk)

  while (!TX_DATA_VALID && (bytesSent < currentPacketSize))
  {
    // channel->PSCR |= USIC_CH_PSCR_CTBIF_Msk;  // clear the TBIF.
    XMC_SPI_CH_Transmit(
        channel, *(int16_t *)(txPacket + bytesSent), XMC_SPI_CH_MODE_STANDARD);

    bytesSent += 2;

    // XMC_GPIO_SetOutputHigh(espSelect.port, espSelect.pin);
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