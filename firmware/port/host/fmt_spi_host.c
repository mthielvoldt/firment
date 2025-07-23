// This is a MFG-specific header file name. 
#include <fmt_spi.h>
#include <stdbool.h>
#include <stdint.h>

uint32_t port_getSpiEventIRQn(uint8_t spiDriverId)
{
  return spiDriverId + 1;
}

bool port_initSpiModule(spiCfg_t *cfg)
{
  return true;
}