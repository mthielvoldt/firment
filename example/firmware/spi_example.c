
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

// #include "Arduino.h"
#include "xmc_gpio.h"
#include "xmc_spi.h"

typedef enum
{
  USIC_INPUT_A = 0U, /*< Input-A */
  USIC_INPUT_B,      /*< Input-B */
  USIC_INPUT_C,      /*< Input-C */
  USIC_INPUT_D,      /*< Input-D */
  USIC_INPUT_E,      /*< Input-E */
  USIC_INPUT_F,      /*< Input-F */
  USIC_INPUT_G,      /*< Input-G */
  USIC_INPUT_INVALID /*< This is to check during mode switch */
} usicInput_t;

typedef struct
{
  XMC_GPIO_PORT_t *port;
  uint8_t pin;
} portPin_t;

typedef struct
{
  XMC_USIC_CH_t *channel;
  XMC_SPI_CH_CONFIG_t channel_config;
  portPin_t mosi;
  portPin_t miso;
  portPin_t sclkout;
  portPin_t ss0;
  XMC_GPIO_CONFIG_t miso_config;
  usicInput_t input_source;
  uint8_t word_length;
  uint8_t frame_length;
} spiChannel_t;

int main(void)
{
  portPin_t ledPin = {.port = XMC_GPIO_PORT0, .pin = 13U};
  portPin_t espSelect = {.port = XMC_GPIO_PORT3, .pin = 10U};

  const XMC_GPIO_CONFIG_t gpOutPinConfig = {
      .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
      .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
      .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM,
  };

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
      .word_length = 8,
      .frame_length = 8,
  };

  const bool initBrg = true; // Automatically configure the baudrate generator.

  const uint16_t data = 0xA5u; //24u;

  XMC_GPIO_Init(ledPin.port, ledPin.pin, &gpOutPinConfig);
  XMC_GPIO_Init(espSelect.port, espSelect.pin, &gpOutPinConfig);

  XMC_SPI_CH_InitEx(channel, &config, initBrg);
  XMC_SPI_CH_SetWordLength(channel, spi20Pins.word_length);
  XMC_SPI_CH_SetFrameLength(channel, spi20Pins.frame_length);
  XMC_SPI_CH_SetInputSource(spi20Pins.channel, XMC_SPI_CH_INPUT_DIN0, (uint8_t)spi20Pins.input_source);
  XMC_SPI_CH_Start(channel);
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.sclkout.port, (uint8_t)spi20Pins.sclkout.pin, &spiOutPinConfig);
  /* Configure the input pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.miso.port, (uint8_t)spi20Pins.miso.pin, &(spi20Pins.miso_config));
  /* Configure the output pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)spi20Pins.mosi.port, (uint8_t)spi20Pins.mosi.pin, &spiOutPinConfig);

  for (;;)
  {

    XMC_GPIO_ToggleOutput(ledPin.port, ledPin.pin);
    
    XMC_GPIO_SetOutputLow(espSelect.port, espSelect.pin);
    // XMC_SPI_CH_EnableSlaveSelect(channel, XMC_SPI_CH_SLAVE_SELECT_0);

    XMC_SPI_CH_Transmit(channel, data, XMC_SPI_CH_MODE_STANDARD);
    // XMC_SPI_CH_Transmit(channel, data, XMC_SPI_CH_MODE_STANDARD);

    for (volatile int i = 0; i < 500; i++)
      ;
    XMC_GPIO_SetOutputHigh(espSelect.port, espSelect.pin);

    // XMC_SPI_CH_DisableSlaveSelect(channel);

    for (volatile int i = 0; i < 50000; i++)
      ;
  }
}