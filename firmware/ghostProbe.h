#pragma once

#include <gp_config.h>
// TODO: rename mcu_1 to something like projectMessages.proto
#include <generated/messages.pb.h>

#include <stdint.h>
#include <stddef.h>

/** A probe pad is the initialization that makes a given variable probe-able.
 * The analogy is to a PCB with physical probe pads.  There can be many pads,
 * that you switch your probes between at run-time.
 */
typedef enum _srcType {
  SRC_TYPE_FLOAT,
  SRC_TYPE_INT8,
  SRC_TYPE_INT16,
  SRC_TYPE_INT32,
} srcType_t;

typedef struct _testPoint {
  void *src;
  srcType_t type;
} testPoint_t;

/** Init GhostProbe
 * 
 */
void gp_init(uint32_t periodicCallFrequencyHz);

void gp_initTestPoint(TestPointId id, void *src, srcType_t type);

/** Called by generated fmt_rx.pb.c 
 * See 
*/
#define USE_RunScanCtl
void handleRunScanCtl(RunScanCtl scanCtl);

void gp_periodic(void);