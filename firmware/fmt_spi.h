#pragma once
#include "fmt_sizes.h"
#include <project_comms.h>
#include <Driver_SPI.h>
#include "fmt_ioc.h"

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
  RTE_IOC_t msgWaitingInput;  // HW signal: sub has a message for main.
  uint8_t msgWaitingOut;
  uint8_t msgWaitingIRQn;
  RTE_IOC_t clearToSendInput; // HW signal: sub ready for transaction.
  uint8_t clearToSendOut;
  uint8_t clearToSendIRQn;
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

/** Message Waiting ISR
 * This must be called externally by an interrupt handler linked to the IRQn 
 * set up to handle the Message Waiting signal. 
 */
void fmt_msgWaitingISR(void);

void fmt_clearToSendISR(void);
