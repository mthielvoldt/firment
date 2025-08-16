#include "control.h"
#include <fmt_gpio.h>
#include <gpio_pcbDetails.h>
#include <ghostProbe.h>

wave_t channelA = {
    .amplitude = 0.5f,
    .max = 1.0f,
    .min = 0.0f,
    .offset = 0.5f,
    .shape = WAVE_SHAPE_SINE};

wave_t channelB = {
    .amplitude = 0.5f,
    .max = 1.0f,
    .min = 0.0f,
    .offset = 0.5f,
    .shape = WAVE_SHAPE_SAWTOOTH};

/*
fmt_waveform only updates wave phases on wave_updateAll(), not the wave values.
Here, we calculate the values and mirror them in this static variable so we can
probe the values with ghostProbe. */
float chanAOut = 0.0F;
float chanBOut = 0.0F;
float chanAInv, chanAOffset, chanAOffsetInv;
int16_t chanB1000xInt;

static float floatTimes2(volatile void *rawValue);

void ctl_init(float waveformUpdateFreq)
{
  // calling this from periodicA at 1000Hz.
  wave_initAll(waveformUpdateFreq);

  wave_add(&channelA);
  wave_setFrequency(0, 0.1);

  wave_add(&channelB);
  wave_setFrequency(1, 0.15);

  gp_initTestPoint(TestPointId_CHAN_A, &chanAOut, SRC_TYPE_FLOAT, NULL);
  gp_initTestPoint(TestPointId_CHAN_A_INV, &chanAInv, SRC_TYPE_FLOAT, NULL);
  gp_initTestPoint(TestPointId_CHAN_A_PLUS, &chanAOffset, SRC_TYPE_FLOAT, NULL);
  gp_initTestPoint(TestPointId_CHAN_A_PLUS_INV, &chanAOffsetInv, SRC_TYPE_FLOAT, NULL);
  gp_initTestPoint(TestPointId_CHAN_B, &chanBOut, SRC_TYPE_FLOAT, NULL);
  gp_initTestPoint(TestPointId_CHAN_B_AS_INT, &chanB1000xInt, SRC_TYPE_INT16, NULL);
  gp_initTestPoint(TestPointId_CHAN_B_TIMES2, &chanBOut, SRC_TYPE_FLOAT, floatTimes2);

  // Init pins that drive phase gates in a way that they won't turn things on
  #ifdef PHASE_U_HI_ID
  fmt_initGpioInPin(PHASE_U_HI_ID, INPUT_MODE_PULL_DOWN);
  fmt_initGpioInPin(PHASE_U_LO_ID, INPUT_MODE_PULL_DOWN);
  fmt_initGpioInPin(PHASE_V_HI_ID, INPUT_MODE_PULL_DOWN);
  fmt_initGpioInPin(PHASE_V_LO_ID, INPUT_MODE_PULL_DOWN);
  fmt_initGpioInPin(PHASE_W_HI_ID, INPUT_MODE_PULL_DOWN);
  fmt_initGpioInPin(PHASE_W_LO_ID, INPUT_MODE_PULL_DOWN);
  #endif
}

void ctl_setWaveform(waveCfg_t cfg) {
  wave_setShape(cfg.id, cfg.shape);
  wave_setAmplitude(cfg.id, cfg.amplitudeV);
  wave_setFrequency(cfg.id, cfg.frequencyHz);
  wave_setOffset(cfg.id, cfg.offsetV);
}

void ctl_updateVoltageISR(void)
{
  wave_updateAll();
  // calculate this every update so we can plot it. 
  chanAOut = wave_getValue(&channelA);
  chanAInv = -chanAOut;
  chanAOffset = chanAOut + 0.1F;
  chanAOffsetInv = -chanAOffset;
  chanBOut = wave_getValue(&channelB);
  chanB1000xInt = chanBOut * 1000;
}

// This is just a prototype.  
telem_t ctl_getTelem(void)
{
  return (telem_t)
  {
    .voltage = chanAOut,
    .currentMa = 5
  };
}

float floatTimes2(volatile void *rawValue)
{
  return *(float*)rawValue * 2;
}
