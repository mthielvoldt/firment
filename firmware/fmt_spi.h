#pragma once
#include "fmt_sizes.h"
#include <project_comms.h>
#include <Driver_SPI.h>

typedef enum {
  BUS_MODE_MAIN,
  BUS_MODE_SUB,
} busMode_t;

/** SPI configuration struct
 *
 */
typedef struct
{
  ARM_DRIVER_SPI *spiModule;
  uint32_t baudHz;
  busMode_t busMode;
  bool ssActiveLow;
  uint32_t spiIrqPriority;
} spiCfg_t;

/** Initializes firment spi driver
 * passes project-specific pin and peripheral selections.
 */
bool fmt_initSpi(spiCfg_t config);

/** Send a SPI message
 * Queues a message for transmit.
 * Should only be called from a single context - not thread-safe.
 */
bool fmt_sendMsg(Top message);

bool fmt_getMsg(Top *message);
