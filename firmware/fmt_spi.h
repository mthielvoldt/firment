#pragma once
#include <project_comms.h>
#include <Driver_SPI.h>

#define HEADER_SIZE_BYTES 1U
#define CRC_SIZE_BYTES 2U
#define MAX_PACKET_SIZE_BYTES 64U // Must be less than 259; len sent in 1 byte.
#define SEND_QUEUE_LENGTH 4U
#define MAX_SENDER_PRIORITY 16U

#define MAX_MESSAGE_SIZE_BYTES (MAX_PACKET_SIZE_BYTES - HEADER_SIZE_BYTES)

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
  uint32_t spiIrqPriority;
} spiCfg_t;

/** Initializes firment spi driver
 * passes project-specific pin and peripheral selections.
 */
bool fmt_initSpi(spiCfg_t config);

static inline spiCfg_t getDefaultSpiCfg(void)
{
  return (const spiCfg_t){
      .baudHz = 1000000,
      .busMode = BUS_MODE_MAIN,
      .ssActiveLow = true,
      .spiIrqPriority = 32,
  };
}

/** Send a SPI message
 * Queues a message for transmit.
 * Should only be called from a single context - not thread-safe.
 */
bool fmt_sendMsg(Top message);

bool fmt_getMsg(Top *message);
