#pragma once

#include <ISR_Config.h>
#include <messages.pb.h>
#include <Driver_SPI.h>
#include "fmt_sizes.h"
#include "RTE_Device.h"  // Interrupt-on-change pins.  includes RTE_Device.

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
  uint8_t msgWaitingOut;      // TODO: Out and IRQn might be HW-specific. Abstract.
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

/** Sub Message Waiting ISR
 * @attention Requires #defining to the ISR that corresponds to msgWaitingInput
 * and msgWaitingOut in ISR_Config.h.  
 * 
 * Example from an XMC4 project:
 * #define subMsgWaitingISR ERU0_3_IRQHandler  // in ISR_Config.h
 * In this line: 
 * - ERU0 is the Event Request Unit provided in msgWaitingInput.
 * - 3 matches msgWaitingSendOut (the selected ERU output channel). 
 * 
 * Runs when the ESP drives a rising edge on the Message Waiting GPIO.
 *
 * Immediately starts a chain of transactions to flush the ESP's buffer until
 * the queued message is transferred.
 */
void subMsgWaitingISR(void);

/** Sub Clear To Send ISR
 * @attention Requires #defining to the ISR that corresponds to clearToSendInput
 * and clearToSendOut in ISR_Config.h.  
 * 
 * Example from an XMC4 project:
 * #define subClearToSendISR ERU0_2_IRQHandler  // in ISR_Config.h
 * In this line: 
 * - ERU0 is the Event Request Unit provided in clearToSendInput.
 * - 2 matches clearToSendOut (the selected ERU output channel). 
 * 
 *
 * Runs when the ESP (sub) drives a rising edge on the Clear-to-Send GPIO.
 *
 * This ISR re-starts SPI transactions then disables itself (one-shot).
 * This ISR is re-enabled when the clear-to-send signal being read low 
 * (inactive) pauses SPI transactions to wait for the sub to be ready.
 */
void subClearToSendISR(void);