
/** Comms Periodic
 * To be called periodically either by making this a timer-driven ISR,
 * or called within a periodic RTOS task.
 * 
 * Generates many of the messages this ECU sends, including:
 *  - Telemetry advertising sensor readings
 *  - Alerts
 * 
 * Messages NOT handled by this function:
 *  - ACKs.  These are managed by handleTx().
 *  - Command responses.  These are driven by the handleRx().
 */

#include "gpio_common.h"

void comm_init(portPin_t ledPin);

void comm_handleTelemetry(void);

