/** Gives Firment access to the pb message data types for this project.
 * Also defines which of those messages are in-use in this project.
 */
#pragma once
#include <messages.pb.h>

bool comm_init(void);

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

