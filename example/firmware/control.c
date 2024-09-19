#include "control.h"
#include <fmt_waveform.h>

wave_t wave0 = {
    .amplitude = 0.5f,
    .max = 1.0f,
    .min = 0.0f,
    .offset = 0.5f,
    .shape = WAVE_SHAPE_SINE};

void ctl_init(float waveformUpdateFreq)
{
  // calling this from periodicA at 1000Hz.
  wave_initAll(waveformUpdateFreq);
  wave_add(&wave0);
  wave_setFrequency(&wave0, 0.1);
}

void ctl_updateVoltageISR(void)
{
  wave_updateAll();
}

// This is just a prototype.  
telem_t ctl_getTelem(void)
{
  return (telem_t)
  {
    .voltage = wave_getValue(&wave0),
    .currentMa = 5
  };
}