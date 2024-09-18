#include "fmt_spi.h"
#include <ctype.h>

#include "queue.h"
#include <pb_encode.h>
#include <pb_decode.h>

// Initialize with the start sequence in header.
static uint8_t sendQueueStore[MAX_PACKET_SIZE_BYTES * SEND_QUEUE_LENGTH];
static queue_t sendQueue;

// optimization: stop storing the config.
static portPin_t led = {};
static XMC_USIC_CH_t *channel = NULL;

// Private Declarations
static void initSpiGpio(spiCfg_t cfg);

bool initFirment_spi(spiCfg_t cfg)
{
  const bool initBrg = true; // Automatically configure the baudrate generator.
  led = cfg.led;
  channel = cfg.channel;

  // InitEx() must come first among XMC_SPI calls as subsequent "Set" functions
  // replace the defaults set by InitEx.
  XMC_SPI_CH_InitEx(cfg.channel, &cfg.channel_config, initBrg);
  XMC_SPI_CH_SetWordLength(cfg.channel, cfg.word_length);
  XMC_SPI_CH_SetFrameLength(cfg.channel, cfg.frame_length);
  XMC_SPI_CH_SetInputSource(
      cfg.channel, XMC_SPI_CH_INPUT_DIN0, (uint8_t)cfg.input_source);
  XMC_SPI_CH_SetBitOrderLsbFirst(cfg.channel);

  /** MSLS master slave select.
   * SELCFG block: supports SELOx output signals.
   * - Direct select mode = connect each SELOx line to sub device.
   *   - a SELOx output pin driven by MSLS signal if PCR.SELOx is 1.
   * PCR.SELCTR: choose direct or coded select mode
   */
  XMC_SPI_CH_EnableSlaveSelect(cfg.channel, cfg.sub);

  // Rig up interrupt
  XMC_SPI_CH_SelectInterruptNodePointer(
      cfg.channel,
      XMC_SPI_CH_INTERRUPT_NODE_POINTER_TRANSMIT_BUFFER,
      cfg.serviceRequest);
  XMC_SPI_CH_EnableEvent(cfg.channel, XMC_SPI_CH_EVENT_TRANSMIT_BUFFER);
  NVIC_SetPriority(cfg.IRQn, cfg.priority);
  NVIC_EnableIRQ(cfg.IRQn);

  XMC_SPI_CH_Start(cfg.channel);

  initSpiGpio(cfg);

  initQueue(
      MAX_PACKET_SIZE_BYTES,
      SEND_QUEUE_LENGTH,
      &sendQueue,
      sendQueueStore,
      MAX_SENDER_PRIORITY);
  return true;
}

static void initSpiGpio(spiCfg_t cfg)
{

  XMC_GPIO_Init(cfg.led.port, cfg.led.pin, &gpOutPinConfig);
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)cfg.sclk.port,
      (uint8_t)cfg.sclk.pin,
      &cfg.sclk.config);
  /* Configure the input pin properties */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)cfg.miso.port,
      (uint8_t)cfg.miso.pin,
      &(cfg.miso.config));
  /* Configure the output pin properties */
  XMC_GPIO_Init(
      (XMC_GPIO_PORT_t *)cfg.mosi.port,
      (uint8_t)cfg.mosi.pin,
      &cfg.mosi.config);

  /* Pin under USIC control as ALT1 output */
  XMC_GPIO_Init(cfg.ss0.port, cfg.ss0.pin, &cfg.ss0.config);
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

    // Kick off Tx in case it had paused.  Does nada if Spi HW busy.
    ISR_handleTx_spi();
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

  /*
  If we've finished transmitting the previous message, but there are still
  messages in the queue, pull a new packet out of the queue and keep Txing. */
  if (bytesSent >= currentPacketSize)
  {
    if (dequeueFront(&sendQueue, txPacket))
    {
      // dequeue successful.  Starting tx of new packet.
      currentPacketSize = txPacket[0] + HEADER_SIZE_BYTES;
      bytesSent = 0;
      XMC_GPIO_ToggleOutput(led.port, led.pin);
    }
  }

  /*
  We send 2 bytes at a time, so we Tx 1 extra byte for odd-size packets.
  MAX_PACKET_SIZE_BYTES is even, so we'll never read past the buffer.
  This Fn, being interrupt-driven, rapidly gets re-called until _Transmit() is
  skipped, which only happens when sendQueue is empty. */
  if (bytesSent < currentPacketSize)
  {
    /* The next line clears the Tx Buff Interrupt Flag (TBIF) so a new IRQ fires
    as soon as Tx Buff is ready again for a new word.
    It also blocks (spinlock) to wait for TBUF to become available in case it is
    called before the Tx Buff is ready.  Note: this assumes one-shot mode*/
    XMC_SPI_CH_Transmit(
        channel, *(int16_t *)(txPacket + bytesSent), XMC_SPI_CH_MODE_STANDARD);

    // Because the previous blocks until TBUF ready, we assume Tx happened.
    bytesSent += 2;
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