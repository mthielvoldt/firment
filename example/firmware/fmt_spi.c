#include "fmt_spi.h"
#include <ctype.h>

#include <pb_encode.h>
#include <pb_decode.h>

// TODO: write this block in init.
static XMC_USIC_CH_t *const channel = XMC_USIC2_CH0;
static portPin_t ledPin = {.port = XMC_GPIO_PORT0, .pin = 13U};
static portPin_t espSelect = {.port = XMC_GPIO_PORT3, .pin = 10U};

static uint8_t buffer[BUFFER_SIZE_BYTES] = {0x00, 0xEF, 0xCD, 0xAB}; // Initialize with the start sequence in header.
static uint8_t *payload = buffer + HEADER_SIZE_BYTES;
static size_t msg_len;

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
  XMC_SPI_CH_SetInputSource(spi20Pins.channel, XMC_SPI_CH_INPUT_DIN0, (uint8_t)spi20Pins.input_source);
  XMC_SPI_CH_SetBitOrderLsbFirst(channel);

  // Configure the FIFO

  XMC_SPI_CH_Start(channel);
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.sclkout.port, (uint8_t)spi20Pins.sclkout.pin, &spiOutPinConfig);
  /* Configure the input pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.miso.port, (uint8_t)spi20Pins.miso.pin, &(spi20Pins.miso_config));
  /* Configure the output pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.mosi.port, (uint8_t)spi20Pins.mosi.pin, &spiOutPinConfig);
  return true;
}

bool sendMsg(Top message)
{
  // Optimization: move this to init time and re-use ostream.
  pb_ostream_t ostream = pb_ostream_from_buffer(payload, PAYLOAD_SIZE_BYTES);

  bool success = pb_encode(&ostream, Top_fields, &message);
  if (success)
  {
    buffer[0] = msg_len = ostream.bytes_written;
    // Queue the send.
    // xQueueSend(buffer)
  }
  else // message possibly bigger than PAYLOAD_SIZE_BYTES?
  {
    // transmit an error message instead?
  }

  return success;
}

void ISR_handleTx_spi(void)
{
  XMC_GPIO_ToggleOutput(ledPin.port, ledPin.pin);

  for (int i = 0; i < (msg_len + HEADER_SIZE_BYTES); i += 4)
  {
    XMC_GPIO_SetOutputLow(espSelect.port, espSelect.pin);

    XMC_SPI_CH_Transmit(channel, *(int16_t *)(buffer + i), XMC_SPI_CH_MODE_STANDARD);
    XMC_SPI_CH_Transmit(channel, *(int16_t *)(buffer + i + 2), XMC_SPI_CH_MODE_STANDARD);
    for (volatile int i = 0; i < 250; i++)
      ;
    XMC_GPIO_SetOutputHigh(espSelect.port, espSelect.pin);

    // XMC_SPI_CH_DisableSlaveSelect(channel);
  }
}

Top decode(uint8_t *buffer, size_t msg_len)
{
  Top msg = Top_init_zero;
  /* Create a stream that reads from the buffer. */
  pb_istream_t stream = pb_istream_from_buffer(buffer + HEADER_SIZE_BYTES, msg_len);

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
