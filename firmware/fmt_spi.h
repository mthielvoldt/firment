#include "generated/mcu_1.pb.h"
#include <Driver_SPI.h>
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

typedef enum {
  BUS_MODE_MAIN,
  BUS_MODE_SUB,
} busMode_t;

/** SPI configuration struct
 *
 */
typedef struct
{
  uint32_t baudHz;
  busMode_t busMode;
  bool ssActiveLow;
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
  return (const spiCfg_t){
      .IRQn = USIC2_0_IRQn,
      .serviceRequest = 0,
      .baudHz = 1000000,
      .busMode = BUS_MODE_MAIN,
      .ssActiveLow = true,
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

/** Pulls the next packed from the sendQueue and sends it.
 * Triggered on SPI_EVENT_TRANSFER_COMPLETE, which can be in ISR context, or as 
 * a result of a sendMsg() call.
 */
void SendNextPacket(void);

/** Receive handler
 * An ISR that runs when a new word has been received and is ready to process.
 */
void ISR_handleRx_spi(void);