#include "ghostProbe.h"
#include "fmt_comms.h"

// NUM_PROBES must match number of probe_x fields in RunScanCtl.
// See firment_msg.in.proto.
#define NUM_PROBES 5

static testPoint_t testPoints[_TestPointId_ARRAYSIZE];

static TestPointId activeTestPoints[NUM_PROBES];
static volatile bool running = false;
static uint32_t numActiveProbes = 0;
static uint32_t scanFreqDivider = 0;
static uint32_t periodicFreqHz = 0;

static ProbeSignal readTestPoint(TestPointId testPoint);

void gp_init(uint32_t periodicCallFrequencyHz)
{
  periodicFreqHz = periodicCallFrequencyHz;
}

bool gp_initTestPoint(TestPointId id, volatile void *src, srcType_t type, converter_t converterFn)
{
  if (id < _TestPointId_ARRAYSIZE)
  {
    testPoints[id] = (const testPoint_t){.src = src, .type = type, .converter = converterFn};
    return true;
  }
  return false;
}

void handleRunScanCtl(RunScanCtl scanCtl)
{
  // Stop running first so we don't race gp_periodic().
  running = false;

  if (scanCtl.freq > SampleFreq_SCAN_DISABLED)
  {
    /* Integer division.  If scanFreqDivider set to 0, periodic will send signals
    on ever call, same as if scanFreqDivider == 1. */
    scanFreqDivider = periodicFreqHz / scanCtl.freq;

    numActiveProbes = 0;
    TestPointId *ids = &(scanCtl.probe_0);
    for (unsigned i = 0; i < NUM_PROBES; i++)
    {
      TestPointId thisId = ids[i];
      if (thisId != TestPointId_DISCONNECTED)
      {
        activeTestPoints[numActiveProbes] = thisId;
        numActiveProbes++;
      }
    }
    running = true;
  }
}

void gp_periodic(void)
{
  static uint_fast32_t callCount = 0;
  if (running && (++callCount >= scanFreqDivider))
  {
    callCount = 0;
    ProbeSignals signals = {.probeSignals_count = numActiveProbes};
    for (unsigned i = 0; i < numActiveProbes; i++)
    {
      signals.probeSignals[i] = readTestPoint(activeTestPoints[i]);
    }
    fmt_sendMsg((const Top){
        .which_sub = Top_ProbeSignals_tag,
        .sub = {
            .ProbeSignals = signals}});
  }
}

static ProbeSignal readTestPoint(TestPointId id)
{
  ProbeSignal signal;
  signal.id = id;
  testPoint_t pad = testPoints[id];

  if (pad.converter)
  {
    signal.value = pad.converter(pad.src);
  }
  else
  {
    switch (pad.type)
    {
    case SRC_TYPE_FLOAT:
      signal.value = *(float *)pad.src;
      break;
    case SRC_TYPE_INT8:
      signal.value = *(int8_t *)pad.src;
      break;
    case SRC_TYPE_INT16:
      signal.value = *(int16_t *)pad.src;
      break;
    case SRC_TYPE_INT32:
      signal.value = *(int32_t *)pad.src;
      break;
    case SRC_TYPE_UINT8:
      signal.value = *(uint8_t *)pad.src;
      break;
    case SRC_TYPE_UINT16:
      signal.value = *(uint16_t *)pad.src;
      break;
    case SRC_TYPE_UINT32:
      signal.value = *(uint32_t *)pad.src;
      break;
    }
  }
  return signal;
}