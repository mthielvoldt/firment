// This is a MFG-specific header file name. 
#include <stdint.h>

uint32_t port_getSpiEventIRQn(uint8_t spiModuleId)
{
  return spiModuleId + 1;
}
