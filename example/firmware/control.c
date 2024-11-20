#include "control.h"
#include <fmt_waveform.h>
#include <ghostProbe.h>

wave_t wave0 = {
    .amplitude = 0.5f,
    .max = 1.0f,
    .min = 0.0f,
    .offset = 0.5f,
    .shape = WAVE_SHAPE_SINE};

wave_t wave1 = {
    .amplitude = 0.5f,
    .max = 1.0f,
    .min = 0.0f,
    .offset = 0.5f,
    .shape = WAVE_SHAPE_SAWTOOTH};

/*
fmt_waveform only updates wave phases on wave_updateAll(), not the wave values.
Here, we calculate the values and mirror them in this static variable so we can
probe the values with ghostProbe. */
float wave0Value = 0.0F;
float wave1Value = 0.0F;
float sineInv;
float sinePlus;

void ctl_init(float waveformUpdateFreq)
{
  // calling this from periodicA at 1000Hz.
  wave_initAll(waveformUpdateFreq);

  wave_add(&wave0);
  wave_setFrequency(&wave0, 0.1);

  wave_add(&wave1);
  wave_setFrequency(&wave1, 0.15);

  gp_initTestPoint(TestPointId_SINE, &wave0Value, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_SINE_INV, &sineInv, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_SINE_PLUS, &sinePlus, SRC_TYPE_FLOAT);
  gp_initTestPoint(TestPointId_SAWTOOTH, &wave1Value, SRC_TYPE_FLOAT);

}

void ctl_updateVoltageISR(void)
{
  wave_updateAll();
  // calculate this every update so we can plot it. 
  wave0Value = wave_getValue(&wave0);
  sineInv = -wave0Value;
  sinePlus = wave0Value + 0.1F;
  wave1Value = wave_getValue(&wave1);
}

// This is just a prototype.  
telem_t ctl_getTelem(void)
{
  return (telem_t)
  {
    .voltage = wave0Value,
    .currentMa = 5
  };
}