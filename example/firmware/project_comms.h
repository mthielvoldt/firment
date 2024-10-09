/** Gives Firment access to the pb message data types for this projject.
 * Also defines which of those messages are in-use in this project.
 */
#pragma once
#include "gpio_common.h"
#include "generated/mcu_1.pb.h"
#include <fmt_spi.h>  // must be here to provide fmt_rx with correct driver.

void comm_init(portPin_t ledPin); // kinda hate xmc gpio being here - for main, not fmt.

#define USE_WaveformCtl
void handleWaveformCtl(WaveformCtl msg);

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
void comm_handleTelemetry(void);

