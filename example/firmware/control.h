/** Responsible for the physical (non-comms) function(s) of the device.
 * The "model" in MVC parlance - encapsulates the business logic.
 * 
 * comms.c is a customer of this file (it depends on us, not vice versa).
 */
#include <stdint.h>

typedef struct {
  float voltage;
  uint32_t currentMa;
} telem_t;


void ctl_init(float waveformUpdateFreq);

void setParam(uint32_t paramId, uint32_t value);

uint32_t getParam(uint32_t paramId);

telem_t ctl_getTelem(void);

void ctl_updateVoltageISR(void);