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

void wave_setFrequency(wave_t *wave, float frequencyHz);

void wave_setPhase(wave_t *wave, float phaseRad);

float wave_getValue(wave_t *wave);

/** A periodic that should be called at updateFrequencyHz */
void wave_updateAll(void);

/*
typedef struct {
} waveCfg_t;
void wave_setShape(wave_t *wave, waveShape_t shape);
void wave_setOffset(wave_t *wave, float dcOffset);
void wave_setAmplitude(wave_t *wave, float amplitude);
void wave_setLimits(wave_t *wave, float min, float max);
*/