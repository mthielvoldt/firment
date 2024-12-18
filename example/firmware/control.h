/** Responsible for the physical (non-comms) function(s) of the device.
 * The "model" in MVC parlance - encapsulates the business logic.
 * 
 * CONCERNS:
 * - Combines services of lower-level libraries to achieve device capabilities.  
 * - Provides interfaces that gesture to those capabilities.
 * NOTCONCERNS: 
 * - convenience or safety of the entity sending messages to this device.
 * 
 * Example: keep-alive (turning off after some time without a message)
 *  does NOT belong here.
 * 
 * Analogy: a carburetor in a vehicle, which combines fuel system, air intake, 
 * and vacuum from engine to service the engine intake requirement.  Provides
 * an input shaft for power control.  Not opinionated about whether throttle is
 * able to be locked in-place or returns home if released. 
 * 
 * comms.c is a customer of this file (it depends on us, not vice versa).
 */
#include <stdint.h>
#include <fmt_waveform.h>

typedef struct {
  float voltage;
  uint32_t currentMa;
} telem_t;

typedef struct {
  uint8_t id;
  float amplitudeV;
  float frequencyHz;
  float offsetV;
  waveShape_t shape;
} waveCfg_t;

void ctl_init(float waveformUpdateFreq);

void setParam(uint32_t paramId, uint32_t value);

void ctl_setWaveform(waveCfg_t msg);

uint32_t getParam(uint32_t paramId);

telem_t ctl_getTelem(void);

void ctl_updateVoltageISR(void);