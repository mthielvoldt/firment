#include <CppUTest/TestHarness.h>

extern "C"
{
#include <ghostProbe.h>
#include "stub_comms.h"
#include <signal.h>
}

static float srcs[_TestPointId_ARRAYSIZE];
static float initials[_TestPointId_ARRAYSIZE];
static int32_t i32Src = 5;
static int16_t i16Src = 5;
static int8_t i8Src = 5;
static uint32_t u32Src = 5U;
static uint16_t u16Src = 5U;
static uint8_t u8Src = 5U;

TEST_GROUP(ghostProbe)
{
  void setup()
  {
    // Sets up the fmt_comms spy to intercept when ghostProbe sends a message.
    UT_PTR_SET(fmt_sendMsg, fmt_sendMsg_test);
    test_resetComms();
    gp_init(1000, 1000);
    gp_initTestPoint(TestPointId_CHAN_A, &srcs[TestPointId_CHAN_A], SRC_TYPE_FLOAT, NULL);
    gp_initTestPoint(TestPointId_CHAN_A_INV, &srcs[TestPointId_CHAN_A_INV], SRC_TYPE_FLOAT, NULL);
    gp_initTestPoint(TestPointId_CHAN_A_PLUS, &srcs[TestPointId_CHAN_A_PLUS], SRC_TYPE_FLOAT, NULL);
    memset(srcs, 0, sizeof(srcs));
    memset(initials, 0, sizeof(initials));
  };
  void teardown() {
  };
};

unsigned getActiveProbeCount(ScanCtl scanCtl)
{
  TestPointId *probe = &scanCtl.probe_0;
  unsigned activeProbeCount = 0;
  for (unsigned i = 0; i < GP_NUM_PROBES; i++)
  {
    if (probe[i] > TestPointId_DISCONNECTED)
      activeProbeCount++;
  }
  return activeProbeCount;
}

void setExpectedInitialSnapValues(unsigned samplesInBuffer)
{
  for (unsigned i = 0; i < _TestPointId_ARRAYSIZE; i++)
  {
    initials[i] = srcs[i] + 1 - (samplesInBuffer / 2);
  }
}

void checkSignals(ScanCtl scanCtl, Top *msg, unsigned iter)
{
  char errMsg[80];
  sprintf(errMsg, "Iteration: %d, recordId: %d", iter, msg->sub.ProbeSignals.recordId);
  LONGS_EQUAL(Top_ProbeSignals_tag, msg->which_sub);

  ProbeSignals *signals = &msg->sub.ProbeSignals;
  TestPointId *probe = &scanCtl.probe_0;
  size_t expectedSignalCount = getActiveProbeCount(scanCtl);
  LONGS_EQUAL(expectedSignalCount, signals->probeSignals_count);

  for (unsigned i = 0; i < expectedSignalCount; i++)
  {
    LONGS_EQUAL(probe[i], signals->probeSignals[i].id);
    if (scanCtl.snapFreq != SnapFreq_SNAP_DISABLED)
      DOUBLES_EQUAL_TEXT(initials[probe[i]] + iter, signals->probeSignals[i].value, 0.1, errMsg);
    else
      DOUBLES_EQUAL_TEXT(srcs[probe[i]], signals->probeSignals[i].value, 0.1, errMsg);
  }
}

// Each source will have values that never overlap with other sources.
void setNewValues(void)
{
  for (unsigned i = 0; i < _TestPointId_ARRAYSIZE; i++)
  {
    if (srcs[i] == 0.0F)
    {
      srcs[i] = 1000 * i;
    }
    srcs[i]++;
  }
}

TEST(ghostProbe, init)
{
  CHECK_FALSE(gp_init(0, 0));
}

TEST(ghostProbe, initTP_success)
{
  CHECK_TRUE(gp_initTestPoint(TestPointId_CHAN_A, &srcs[TestPointId_CHAN_A], SRC_TYPE_FLOAT, NULL));
}

TEST(ghostProbe, streamOneFloat)
{
  ScanCtl scanCtl = {
      .streamFreq = StreamFreq_FREQ_100_HZ,
      .probe_0 = TestPointId_CHAN_A,
  };
  handleScanCtl(scanCtl);

  // simulate one second passing
  Top msg = {};
  for (unsigned i = 0; i < 1000; i++)
  {
    setNewValues();
    gp_streamPeriodic();
    if (fmt_getMsg_test(&msg))
    {
      checkSignals(scanCtl, &msg, i);
    }
  }
  // Expect to see 100 samples sent via fmt_sendMsg()
  CHECK_EQUAL(100, test_getSendCount());
}

TEST(ghostProbe, snapOneFloat)
{
  const ScanCtl snapCtl = {
      .snapFreq = SnapFreq_FREQ_1KHZ,
      .probe_0 = TestPointId_CHAN_A_INV,
  };

  handleScanCtl(snapCtl);

  const unsigned samplesInBuffer = GP_BUFFER_SIZE / sizeof(float);
  for (unsigned i = 0; i < samplesInBuffer; i++)
  {
    setNewValues();
    gp_snapPeriodic();
  }

  // not Triggered yet, should not see any samples.
  gp_streamPeriodic();
  CHECK_EQUAL(0, test_getSendCount());

  // Trigger manually.
  handleManualSnapCtl((const ManualSnapCtl){});
  setExpectedInitialSnapValues(samplesInBuffer);

  // Take another half-buffer of samples
  for (unsigned i = 0; i < (samplesInBuffer / 2); i++)
  {
    setNewValues();
    gp_snapPeriodic();
  }
  // The next sample attempt will transition to transmitting.
  gp_snapPeriodic();

  Top msg;
  float results[samplesInBuffer * 2] = {};
  for (unsigned i = 0; i < (samplesInBuffer * 2); i++)
  {
    gp_streamPeriodic();
    if (fmt_getMsg_test(&msg))
    {
      checkSignals(snapCtl, &msg, i);
    }
  }
  CHECK_EQUAL(samplesInBuffer, test_getSendCount());
}

// Test Simultaneous plotting
TEST(ghostProbe, snapAndStream)
{
  const ScanCtl streamCtl = {
      .streamFreq = StreamFreq_FREQ_100_HZ,
      .probe_0 = TestPointId_CHAN_A};
  const ScanCtl snapCtl = {
      .snapFreq = SnapFreq_FREQ_1KHZ,
      .probe_0 = TestPointId_CHAN_A_INV,
      .probe_1 = TestPointId_CHAN_A_PLUS};

  handleScanCtl(streamCtl);
  handleScanCtl(snapCtl);

  const unsigned samplesInBuffer = GP_BUFFER_SIZE / sizeof(float);
  for (unsigned i = 0; i < (samplesInBuffer / 4); i++)
  {
    setNewValues();
    gp_snapPeriodic();
    gp_streamPeriodic();
  }

  // Trigger manually.
  handleManualSnapCtl((const ManualSnapCtl){});
  setExpectedInitialSnapValues(samplesInBuffer);


  // Drop any previously sent messages (we expect stream has been sending)
  test_resetComms();

  // Take another buffer worth of samples, checking each message for correctness.
  Top msg;
  for (unsigned i = 0; i < samplesInBuffer; i++)
  {
    // Vary values
    setNewValues();
    gp_snapPeriodic();
    gp_streamPeriodic();
    while (fmt_getMsg_test(&msg))
    {
      if (msg.sub.ProbeSignals.recordId == 0)
        checkSignals(streamCtl, &msg, i);
      else if (msg.sub.ProbeSignals.recordId == 1)
        checkSignals(snapCtl, &msg, i);
      else
        FAIL("recordId not either 0 or 1.");
    }
  }
}

TEST(ghostProbe, snapWithPartialBuffer)
{
  const unsigned samplesInBuffer = GP_BUFFER_SIZE / sizeof(float);

  handleScanCtl((ScanCtl){
      .snapFreq = SnapFreq_FREQ_1KHZ,
      .probe_0 = TestPointId_CHAN_A});
  for (unsigned i = 0; i < 10; i++)
  {
    gp_snapPeriodic();
  }

  handleScanCtl((ScanCtl){
      .snapFreq = SnapFreq_FREQ_1KHZ,
      .probe_0 = TestPointId_CHAN_A,
      .probe_1 = TestPointId_CHAN_A_INV});
  for (unsigned i = 0; i < 10; i++)
  {
    gp_snapPeriodic();
    gp_streamPeriodic();
  }

  // Trigger manually.
  handleManualSnapCtl((const ManualSnapCtl){});
  setExpectedInitialSnapValues(samplesInBuffer);

  // Take another buffer worth of samples, checking each message for correctness.
  Top msg;
  char errMsg[256];
  for (unsigned i = 0; i < samplesInBuffer; i++)
  {
    gp_snapPeriodic();
    gp_streamPeriodic();
    while (fmt_getMsg_test(&msg))
    {
      if (msg.sub.ProbeSignals.probeSignals_count != 2)
      {
        snprintf(errMsg, sizeof(msg),
                 "recordId=0, probeSignals_count: %d Expected: 1.  Iteration: %d",
                 msg.sub.ProbeSignals.probeSignals_count,
                 i);
        FAIL(errMsg);
      }
    }
  }
}
