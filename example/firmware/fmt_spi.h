#include "generated/mcu_1.pb.h"
#include "xmc_spi.h"
#include "xmc_gpio.h"
#include "fmt_types.h"

#define HEADER_SIZE_BYTES 4U
#define BUFFER_SIZE_BYTES 64U
#define PAYLOAD_SIZE_BYTES (BUFFER_SIZE_BYTES - HEADER_SIZE_BYTES)

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

/** Initializes firment spi driver
 * passes project-specific pin and peripheral selections.
 */
bool initFirment_spi(void);

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