#include "ghostProbe.h"
#include "fmt_spi.h"
#include <cmsis_gcc.h> // __BKPT()

// RunScanCtl has 4 bytes for isContinuous and freq, leaving the rest for the
// testPointIds.  Each testPointId takes 2 bytes.  They come at the end.
#define NUM_PROBES ((RunScanCtl_size - 4) / 2)

static testPoint_t testPoints[_TestPointId_ARRAYSIZE];

static TestPointId activeTestPoints[NUM_PROBES];
static bool running = false;
static uint32_t numActiveProbes = 0;
static SampleFreq selectedFreq = 0;
// static uint32_t freqDividers[_SampleFreq_ARRAYSIZE] = 0;

static ProbeSignal readTestPoint(TestPointId testPoint);

void gp_init(void)
{
}

void gp_initProbe(TestPointId id, void *src, srcType_t type)
{
  if (id < _TestPointId_ARRAYSIZE)
  {
    testPoints[id] = (const testPoint_t){.src = src, .type = type};
  }
  else
  {
    __BKPT(6);
  }
}

void handleRunScanCtl(RunScanCtl scanCtl)
{

  numActiveProbes = 0;
  TestPointId *ids = &(scanCtl.probe_0);

  for (unsigned i = 0; i < NUM_PROBES; i++)
  {
    TestPointId thisId = ids[i];
    if (thisId != TestPointId_DISCONNECTED)
    {
      numActiveProbes++;
      activeTestPoints[i] = thisId;
    }
  }
  selectedFreq = scanCtl.freq;
  running = true;
}

void gp_stopScan()
{
  running = false;
}

void gp_periodic(SampleFreq freqId)
{
  if (running && (freqId == selectedFreq))
  {
    ProbeSignals signals = {0};
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
  }
  return signal;
}