/** fmt_transport.h
 * Common interface for the firment transport layers (spi, uart, etc.)
 * Dependency graph: (interfaces)
 * 
 * (fmt_comms.h) -> fmt_comms.c -> (fmt_transport.h) -> [fmt_spi.c / fmt_uart.c]
 * 
 * These are function pointers for two reasons: 
 * 1. Several communication transports (spi, uart, can...) may implement this interface.
 * 2. Unit tests can cover fmt_comms.c using spys at the transport layer.
 * 
 */
#ifndef fmt_transport_H
#define fmt_transport_H

#include <stdint.h>
#include "queue.h"

typedef void(*rxCallback_t)(uint8_t *rxData);

void fmt_startTxChain(void);

bool fmt_linkTransport(queue_t *sendQueue, rxCallback_t rxCallback);

#endif  // fmt_transport_H