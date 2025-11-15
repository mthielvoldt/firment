#include "ghostProbe.h"
#include "fmt_comms.h"

// GP_NUM_PROBES must match number of probe_x fields in ScanCtl.
// See firment_msg.in.proto.
typedef enum
{
  SCAN_DISABLED,
  SCAN_WAITING_ON_TX,
  SCAN_ONGOING, // all active scan states must appear after inactive ones.
  SCAN_FINISHING,
} scanState_t;

typedef struct _scanConfig
{
  testPoint_t *activeTestPoints[GP_NUM_PROBES];
  volatile scanState_t state;
  uint32_t numActiveProbes;
  uint32_t scanFreqDivider;
  uint16_t recordId;
} scanConfig_t;

static testPoint_t testPoints[_TestPointId_ARRAYSIZE];
static uint8_t snapBuffer[GP_BUFFER_SIZE];
static volatile uint8_t *samplePtr = snapBuffer;
static volatile uint8_t *bufferEnd = snapBuffer;
static uint32_t bufferedSampleCount = 0;
static volatile uint8_t *stopPtr = NULL;
static uint16_t nextRecordId = 0;

static uint32_t streamPeriodicFreq = 0;
static uint32_t snapPeriodicFreq = 0;
static scanConfig_t streamConfig = {};
static scanConfig_t snapConfig = {};

static bool sendSignals(scanConfig_t *config, bool fromBuffer);
static ProbeSignal readTestPoint(testPoint_t *testPoint, volatile void *altSrc);
static uint8_t getSrcSize(srcType_t srcType);
static bool setConfig(scanConfig_t *config, ScanCtl scanCtl);
static uint32_t getSampleSize(scanConfig_t *config);
static void resetBuffer(void);

bool gp_init(uint32_t streamCallFrequencyHz, uint32_t snapCallFreqHz)
{
  if (streamCallFrequencyHz < 100 || snapCallFreqHz < 1000)
  {
    return false;
  }
  streamPeriodicFreq = streamCallFrequencyHz;
  snapPeriodicFreq = snapCallFreqHz;
  memset(&streamConfig, 0, sizeof(scanConfig_t));
  memset(&snapConfig, 0, sizeof(scanConfig_t));
  samplePtr = snapBuffer;
  bufferEnd = snapBuffer;
  stopPtr = NULL;
  nextRecordId = 0;
  return true;
}

bool gp_initTestPoint(TestPointId id, volatile void *src, srcType_t type, converter_t converterFn)
{
  if (id < _TestPointId_ARRAYSIZE)
  {
    testPoints[id] = (testPoint_t){
        .src = src,
        .type = type,
        .converter = converterFn,
        .size = getSrcSize(type),
        .id = id};
    return true;
  }
  return false;
}

void handleScanCtl(ScanCtl scanCtl)
{
  if (scanCtl.streamFreq)
  {
    if (setConfig(&streamConfig, scanCtl))
      streamConfig.state = SCAN_ONGOING;
  }

  if (scanCtl.snapFreq)
  {
    if(setConfig(&snapConfig, scanCtl))
    {
      resetBuffer();
      snapConfig.state = SCAN_ONGOING;
    }
  }
}

void handleManualSnapCtl(ManualSnapCtl _)
{
  gp_takeSnapshot();
}

void gp_streamPeriodic(void)
{
  static uint_fast32_t callCount = 0;
  if (streamConfig.state && (++callCount >= streamConfig.scanFreqDivider))
  {
    callCount = 0;
    sendSignals(&streamConfig, false);
  }

  static uint32_t snapSendCount = 0;
  if (snapConfig.state == SCAN_WAITING_ON_TX)
  {
    if (sendSignals(&snapConfig, true))
    {
      if (++snapSendCount == bufferedSampleCount)
      {
        snapSendCount = 0;
        snapConfig.state = SCAN_DISABLED;
      }
    }
  }
}

void gp_snapPeriodic(void)
{
  static uint32_t callCount = 0;
  if (snapConfig.state >= SCAN_ONGOING && (++callCount >= snapConfig.scanFreqDivider))
  {
    callCount = 0;
    /* Here we store values in a buffer as raw values.  Send happens later.*/
    if (samplePtr == stopPtr)
    {
      snapConfig.state = SCAN_WAITING_ON_TX;
      stopPtr = NULL;
      return;
    }

    for (unsigned i = 0; i < snapConfig.numActiveProbes; i++)
    {
      uint32_t tpSize = snapConfig.activeTestPoints[i]->size;
      volatile void *tpSrc = snapConfig.activeTestPoints[i]->src;
      if (tpSize == 4)
      {
        *(uint32_t *)(samplePtr) = *(uint32_t *)tpSrc;
      }
      if (tpSize == 2)
      {
        *(uint16_t *)(samplePtr) = *(uint16_t *)tpSrc;
      }
      else
      {
        *(samplePtr) = *(uint8_t *)tpSrc;
      }
      samplePtr += tpSize;
    }

    /* Because bufferEnd is a multiple of sample size, this check can be after,
    (not inside) the intra-sample for loop above.*/
    if (samplePtr >= bufferEnd)
      samplePtr = snapBuffer;
  }
}

void gp_takeSnapshot(void)
{
  uint32_t sampleSize = getSampleSize(&snapConfig);
  uint32_t remainingSampleCount = (GP_BUFFER_SIZE / sampleSize) / 2;
  stopPtr = samplePtr + remainingSampleCount * sampleSize;
  if (stopPtr >= bufferEnd)
    stopPtr -= GP_BUFFER_SIZE;
}

static bool setConfig(scanConfig_t *config, ScanCtl scanCtl)
{
  // Stop running first so we don't race gp_streamPeriodic().
  config->state = SCAN_DISABLED;
  config->recordId = nextRecordId++;

  /* Integer division.  If scanFreqDivider set to 0, periodic will send signals
    on ever call, same as if scanFreqDivider == 1. */
  if (config == &streamConfig)
  {
    config->scanFreqDivider = streamPeriodicFreq / scanCtl.streamFreq;
  }
  else
  {
    config->scanFreqDivider = snapPeriodicFreq / scanCtl.snapFreq;
  }

  uint32_t nextActiveTp = 0;
  TestPointId *ids = &(scanCtl.probe_0); // array of ids in scanCtl message
  for (unsigned probe = 0; probe < GP_NUM_PROBES; probe++)
  {
    TestPointId testPointId = ids[probe];
    // Add testPointId to config only if it's valid. (Disabled is never valid)
    if (testPoints[testPointId].src != NULL)
    {
      config->activeTestPoints[nextActiveTp] = &testPoints[testPointId];
      nextActiveTp++;
    }
  }
  if (nextActiveTp)
  {
    config->numActiveProbes = nextActiveTp;
    return true;
  }
  return false;
}

static uint32_t getSampleSize(scanConfig_t *config)
{
  uint32_t sampleSize = 0;
  for (int i = 0; i < config->numActiveProbes; i++)
  {
    sampleSize += config->activeTestPoints[i]->size;
  }
  return sampleSize;
}

static bool sendSignals(scanConfig_t *config, bool fromBuffer)
{
  static ProbeSignals signals;
  volatile uint8_t *readPtr = samplePtr;

  signals.probeSignals_count = config->numActiveProbes;
  signals.recordId = config->recordId;
  for (unsigned i = 0; i < config->numActiveProbes; i++)
  {
    testPoint_t *testPoint = config->activeTestPoints[i];
    if (fromBuffer)
    {
      signals.probeSignals[i] = readTestPoint(testPoint, readPtr);
      readPtr += testPoint->size;
    }
    else
    {
      signals.probeSignals[i] = readTestPoint(testPoint, NULL);
    }
  }

  bool sendSuccess = fmt_sendMsg((const Top){
      .which_sub = Top_ProbeSignals_tag,
      .sub = {
          .ProbeSignals = signals}});

  // If queue was full, don't update samplePtr so we read this same point again.
  if (sendSuccess && fromBuffer)
  {
    if (readPtr >= bufferEnd)
      readPtr = snapBuffer;
    samplePtr = readPtr;
  }
  return sendSuccess;
}

static ProbeSignal readTestPoint(testPoint_t *testPoint, volatile void *altSrc)
{
  ProbeSignal signal;
  signal.id = testPoint->id;
  volatile void *src = (altSrc) ? altSrc : testPoint->src;

  if (testPoint->converter)
  {
    signal.value = testPoint->converter(src);
  }
  else
  {
    switch (testPoint->type)
    {
    case SRC_TYPE_FLOAT:
      signal.value = *(float *)src;
      break;
    case SRC_TYPE_INT8:
      signal.value = *(int8_t *)src;
      break;
    case SRC_TYPE_INT16:
      signal.value = *(int16_t *)src;
      break;
    case SRC_TYPE_INT32:
      signal.value = *(int32_t *)src;
      break;
    case SRC_TYPE_UINT8:
      signal.value = *(uint8_t *)src;
      break;
    case SRC_TYPE_UINT16:
      signal.value = *(uint16_t *)src;
      break;
    case SRC_TYPE_UINT32:
      signal.value = *(uint32_t *)src;
      break;
    }
  }
  return signal;
}

static uint8_t getSrcSize(srcType_t srcType)
{
  switch (srcType)
  {
  case SRC_TYPE_FLOAT:
  case SRC_TYPE_INT32:
  case SRC_TYPE_UINT32:
    return 4;
  case SRC_TYPE_INT16:
  case SRC_TYPE_UINT16:
    return 2;
  default:
    return 1;
  }
}

static void resetBuffer(void)
{
  uint32_t sampleSize = getSampleSize(&snapConfig);
    bufferedSampleCount = GP_BUFFER_SIZE / sampleSize;
    samplePtr = snapBuffer;
    bufferEnd = snapBuffer + (bufferedSampleCount * sampleSize);
}