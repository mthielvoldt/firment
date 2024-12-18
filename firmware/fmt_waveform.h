/** Firment Waveform Generator
 * Provides waveform signals for several popular shapes for general consumption.
 * Requires a periodic calling function. 
 */

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  WAVE_SHAPE_DC,
  WAVE_SHAPE_SQUARE,
  WAVE_SHAPE_SINE,
  WAVE_SHAPE_SAWTOOTH,
} waveShape_t;

typedef struct {
  float phase;
  float offset;
  float amplitude;
  float max, min;
  float deltaPhasePerUpdate;
  waveShape_t shape;
} wave_t;



void wave_initAll(float updateFrequencyHz);

bool wave_add(wave_t *wave);

void wave_setPhase(wave_t *wave, float phaseRad);

float wave_getValue(wave_t *wave);

/** A periodic that should be called at updateFrequencyHz */
void wave_updateAll(void);

void wave_setFrequency(uint32_t waveId, float frequencyHz);
void wave_setShape(uint32_t waveId, waveShape_t shape);
void wave_setOffset(uint32_t waveId, float dcOffset);
void wave_setAmplitude(uint32_t waveId, float amplitude);
void wave_setLimits(uint32_t waveId, float min, float max);
