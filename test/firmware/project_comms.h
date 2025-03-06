/** Gives Firment access to the pb message data types for this project.
 * Also defines which of those messages are in-use in this project.
 * 
 * TODO: (12) Can I stop including this .h in fmt_rx and remove dup from test?
 * TODO: (13) Should USE_xxx be in firmentConfig - way to turn on/off modules?
 */
#pragma once
#include "gpio_common.h"
#include <fmt_comms.h>

// TODO: (14) feature envy: gpio really doesn't belong in this header.
void comm_init(portPin_t ledPin); // kinda hate xmc gpio being here - for main, not fmt.

#define USE_WaveformCtl
void handleWaveformCtl(WaveformCtl msg);

#define USE_Reset
void handleReset(Reset msg);

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

