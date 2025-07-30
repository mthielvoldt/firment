#ifndef fmt_spi_H
#define fmt_spi_H

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
  uint8_t spiDriverId;
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

#endif // fmt_spi_H