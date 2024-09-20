#include "fmt_waveform.h"
#include <fmt_waveform_cfg.h>
#include <math.h>

#define TWO_PI 6.283185307F
#define PI 3.141592654F

static wave_t *waves[MAX_NUM_WAVES];
static uint32_t numWaves = 0;
static float updatePeriodSec = 0;

void wave_initAll(float updateFrequencyHz)
{
  updatePeriodSec = 1.0F / updateFrequencyHz;
}

bool wave_add(wave_t *wave)
{
  bool success = false;
  if (numWaves < MAX_NUM_WAVES)
  {
    waves[numWaves] = wave;
    numWaves++;
    success = true;
  }
  return success;
}

void wave_setFrequency(wave_t *wave, float frequencyHz)
{
  wave->deltaPhasePerUpdate = updatePeriodSec * TWO_PI * frequencyHz;
}

void wave_setPhase(wave_t *wave, float phaseRad)
{
  // limits the phase to [0-2*pi] so that phase resolution isn't lost and LUT
  // module doesn't burn CPU cycles enforcing in-bounds access.

  // First, let's try the CPU-inexpensive way that will work for most cases.
  if (phaseRad > TWO_PI)
  {
    phaseRad -= TWO_PI;
  }
  // Just subtracting 2Pi wasn't enough.  Do something more robust.
  if (phaseRad < 0.0F || phaseRad > TWO_PI)
  {
    // neg. example: floor(-7/(2pi)) = -2.  -7 - (-2 * 2pi) = 5.56. (in range)
    phaseRad -= floorf(phaseRad / TWO_PI) * TWO_PI;
  }
  wave->phase = phaseRad;
}

float wave_getValue(wave_t *wave)
{
  float acPart = 0.0F;
  switch (wave->shape)
  {
  case WAVE_SHAPE_SAWTOOTH:
    acPart = wave->amplitude * (wave->phase - PI) / PI;
    break;
  case WAVE_SHAPE_SQUARE:
    acPart = wave->amplitude * (wave->phase > PI ? 1.0F : -1.0F);
    break;
  case WAVE_SHAPE_SINE:
    acPart = wave->amplitude * sinf(wave->phase);
    break;
  case WAVE_SHAPE_DC:
  default:
    // Leave acPart == 0.
    break;
  }
  float value = wave->offset + acPart;

  // clamp value to range
  if (value > wave->max)
    value = wave->max;
  if (value < wave->min)
    value = wave->min;

  return value;
}

void wave_updateAll(void)
{
  // Iterate through intialized waves.
  // Just updates the phase.  The calculation of the value is left to get().
  // Calculate the phase change for this update period
  for (int waveId = 0; waveId < numWaves; waveId++)
  {
    wave_t *wave = waves[waveId];
    wave_setPhase(wave, wave->phase + wave->deltaPhasePerUpdate);
  }
}
