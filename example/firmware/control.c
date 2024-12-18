#include "control.h"
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
float chanAInv;
float chanAOffset;
float chanAOffsetInv;

void ctl_init(float waveformUpdateFreq)
{
  // calling this from periodicA at 1000Hz.
  wave_initAll(waveformUpdateFreq);

  wave_add(&channelA);
  wave_setFrequency(0, 0.1);

  wave_add(&channelB);
  wave_setFrequency(1, 0.15);

  gp_initTestPoint(TestPointId_CHAN_A, &chanAOut, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_CHAN_A_INV, &chanAInv, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_CHAN_A_PLUS, &chanAOffset, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_CHAN_A_PLUS_INV, &chanAOffsetInv, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_CHAN_B, &chanBOut, SRC_TYPE_FLOAT);

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