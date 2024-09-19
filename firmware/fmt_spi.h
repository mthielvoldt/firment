#include "generated/mcu_1.pb.h"
#include "xmc_spi.h"
#include "xmc_gpio.h"
#include "fmt_types.h"

#define HEADER_SIZE_BYTES 4U
#define MAX_PACKET_SIZE_BYTES 64U // Must be less than 259; len sent in 1 byte.
#define SEND_QUEUE_LENGTH 4U
#define MAX_SENDER_PRIORITY 16U

#define MAX_MESSAGE_SIZE_BYTES (MAX_PACKET_SIZE_BYTES - HEADER_SIZE_BYTES)

// TODO: Abstract this away from XMC.
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

/** SPI configuration struct
 *
 */
typedef struct
{
  XMC_USIC_CH_t *channel; // Module # (first digit) must match IRQn first digit.
  XMC_SPI_CH_CONFIG_t channel_config;
  portPin_t mosi;
  portPin_t miso;
  portPin_t sclk;
  portPin_t ss0;                 // Must match sub
  XMC_SPI_CH_SLAVE_SELECT_t sub; // Must match ss0 port/pin per datasheet.
  portPin_t led;
  usicInput_t input_source;
  uint8_t word_length;
  uint8_t frame_length;   // Must be a multiple of .word_length.
  uint8_t serviceRequest; // Must match the second digit in IRQn.
  // First digit must match channel and second digit must match serviceReq.
  // example: .channel = XMC_USIC2_CH0, .serviceReq = 3, .IRQn = USIC2_3_IRQn
  IRQn_Type IRQn;
  uint32_t priority;
} spiCfg_t;

/** Initializes firment spi driver
 * passes project-specific pin and peripheral selections.
 */
bool initFirment_spi(spiCfg_t config);

static inline spiCfg_t getDefaultSpiCfg(void)
{
  const XMC_GPIO_CONFIG_t outPinConfig = {
      .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1,
      .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
      .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM,
  };
  return (const spiCfg_t){
      .channel = XMC_USIC2_CH0,
      .IRQn = USIC2_0_IRQn,
      .serviceRequest = 0,
      .channel_config = {
          .baudrate = 1000000,
          .normal_divider_mode = true,
          .bus_mode = XMC_SPI_CH_BUS_MODE_MASTER,
          .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
          .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE,
      },
      .sclk = {.port = XMC_GPIO_PORT3, .pin = 9U, .config = outPinConfig},
      .mosi = {.port = XMC_GPIO_PORT3, .pin = 8U, .config = outPinConfig},
      .miso = {
          .port = XMC_GPIO_PORT3,
          .pin = 7U,
          .config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE},
      },
      .ss0 = {.port = XMC_GPIO_PORT3, .pin = 10U, .config = outPinConfig},
      .sub = XMC_SPI_CH_SLAVE_SELECT_0,
      .led = {.port = XMC_GPIO_PORT0, .pin = 13U},
      .input_source = USIC_INPUT_C,
      .word_length = 16,
      .frame_length = 32,
      .priority = 32,
  };
}

/** Send a SPI message
 * Queues a message for transmit.
 * Should only be called from a single context - not thread-safe.
 */
bool sendMsg(Top message);

/** Transmit handler
 * An ISR that runs when the Tx buffer is done transferring out the last word
 * and is ready for a new word to be written to it.
 *
 *
 */
void ISR_handleTx_spi(void);

/** Receive handler
 * An ISR that runs when a new word has been received and is ready to process.
 */
void ISR_handleRx_spi(void);