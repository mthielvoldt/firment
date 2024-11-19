#include "ghostProbe.h"
#include "fmt_spi.h"
#include <cmsis_gcc.h> // __BKPT()

// RunScanCtl has 4 bytes for isContinuous and freq, leaving the rest for the
// padIds.  Each padId takes 2 bytes.  They come at the end.
#define NUM_PROBES ((RunScanCtl_size - 4) / 2)

static probePad_t pads[_PadId_ARRAYSIZE];

static PadId activePads[NUM_PROBES];
static bool running = false;
static uint32_t numActiveProbes = 0;
static SampleFreq selectedFreq = 0;
// static uint32_t freqDividers[_SampleFreq_ARRAYSIZE] = 0;

static ProbeSignal readPad(PadId padId);

void gp_init(void)
{
}

void gp_initProbe(PadId id, void *src, srcType_t type)
{
  if (id < _PadId_ARRAYSIZE)
  {
    pads[id] = (const probePad_t){.src = src, .type = type};
  }
  else
  {
    __BKPT(6);
  }
}

void handleRunScanCtl(RunScanCtl scanCtl)
{

  numActiveProbes = 0;
  PadId *padIds = &(scanCtl.padId0);

  for (unsigned i = 0; i < NUM_PROBES; i++)
  {
    PadId thisPadId = padIds[i];
    if (thisPadId != PadId_DISCONNECTED)
    {
      numActiveProbes++;
      activePads[i] = thisPadId;
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
      signals.probeSignals[i] = readPad(activePads[i]);
    }
    fmt_sendMsg((const Top){
        .which_sub = Top_ProbeSignals_tag,
        .sub = {
            .ProbeSignals = signals}});
  }
}

static ProbeSignal readPad(PadId padId)
{
  ProbeSignal signal;
  signal.id = padId;
  probePad_t pad = pads[padId];
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