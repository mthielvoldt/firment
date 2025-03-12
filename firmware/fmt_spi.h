#pragma once

#include <Driver_SPI.h>
#include "fmt_sizes.h"

typedef enum {
  BUS_MODE_MAIN,
  BUS_MODE_SUB,
} busMode_t;

/** SPI configuration struct
 *
 */
typedef struct
{
  uint8_t spiModuleId;
  ARM_DRIVER_SPI *spiModule;
  uint8_t msgWaitingIocId;  // HW signal: sub has a message for main.
  uint8_t msgWaitingIocOut;
  uint8_t clearToSendIocId; // HW signal: sub ready for transaction.
  uint8_t clearToSendIocOut;
  uint32_t baudHz;
  busMode_t busMode;
  bool ssActiveLow;
  uint32_t irqPriority;
} spiCfg_t;

/** Initializes firment spi driver
 * passes project-specific pin and peripheral selections.
 */
bool fmt_initSpi(spiCfg_t config);

/** port_getSpiEventIRQn
 * @param spiModuleNo should match the number of the Driver_SPIx that the IRQn
 * is being requested for. 
 * @returns the IRQn of the spi module indicated by spiModuleNo, provided that 
 * module is configured.  If that module is not configured, returns 0. 
 * 
 * This file should be implemented in a platform-specific _port.c file.
 * 
 * Note: one of the following files must configure the 
 * SPI resource to be available.
 * spi_pcbDetails.h (this is where firment configures the options)
 * RTE_Device.h (other CMSIS BSP desktop apps configure them here.)
 * 
 * For example with the following lines in RTE_Device.h:
 * #define RTE_SPI2 1 
 * #define RTE_SPI5 1
 * 
 * Data structures for SPI modules 2 and 5 will be available, a call to
 * port_getSpiEventIRQn(2) will return the IRQn for SPI2. 
 */
uint32_t port_getSpiEventIRQn(uint8_t spiModuleId);

/** Sub Message Waiting ISR
 * @attention Requires #defining to the ISR that corresponds to msgWaitingIocId
 * and msgWaitingOut in ISR_Config.h.  
 * 
 * Example from an XMC4 project:
 * #define subMsgWaitingISR ERU0_3_IRQHandler  // in ISR_Config.h
 * In this line: 
 * - ERU0 is the Event Request Unit provided in msgWaitingIocId.
 * - 3 matches msgWaitingSendOut (the selected ERU output channel). 
 * 
 * Runs when the ESP drives a rising edge on the Message Waiting GPIO.
 *
 * Immediately starts a chain of transactions to flush the ESP's buffer until
 * the queued message is transferred.
 */
void subMsgWaitingISR(void);

/** Sub Clear To Send ISR
 * @attention Requires #defining to the ISR that corresponds to clearToSendIocId
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