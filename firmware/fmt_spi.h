#ifndef fmt_spi_H
#define fmt_spi_H

#include <Driver_SPI.h>
#include "fmt_transport.h"
#include "queue.h"

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
bool spi_linkTransport(queue_t *_sendQueue, rxCallback_t rxCallback);
void spi_startTxChain(void);

#endif // fmt_spi_H