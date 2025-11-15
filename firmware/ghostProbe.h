/** ghostProbe.h
 * Provides remote probing of variables in firmware over the fmt_comms link.
 * Each variable to be probed is registered as a "test point" with a unique
 * TestPointId, type, source address, and optional converter function.
 * 
 * A runScanCtl message configures which test points to sample at what frequency.
 * 
 * See docs/GhostProbe.md for more information.
 */
#ifndef ghostProbe_H
#define ghostProbe_H

#include <messages.pb.h>
#include <ghostProbeConfig.h>

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
  SRC_TYPE_UINT8,
  SRC_TYPE_UINT16,
  SRC_TYPE_UINT32,
} srcType_t;

typedef float (*converter_t)(volatile void *rawValue);

typedef struct _testPoint {
  volatile void *src;
  srcType_t type;
  converter_t converter;
  size_t size;  // redundant with type, included for access speed.
  uint32_t id;
} testPoint_t;

/** Init GhostProbe
 * 
 */
bool gp_init(uint32_t streamCallFrequencyHz, uint32_t snapCallFreqHz);

bool gp_initTestPoint(TestPointId id, volatile void *src, srcType_t type, converter_t converterFn);

/** Called by generated fmt_rx.pb.c 
 * See fmt_rx.in.c 
*/
#define USE_ScanCtl
void handleScanCtl(ScanCtl scanCtl);

#define USE_ManualSnapCtl
void handleManualSnapCtl(ManualSnapCtl _);

void gp_streamPeriodic(void);
void gp_snapPeriodic(void);
void gp_takeSnapshot(void);

#endif // ghostProbe_H