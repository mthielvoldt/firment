// This is a MFG-specific header file name. 
#include <fmt_spi.h>
#include <fmt_uart.h>
#include <stdbool.h>
#include <stdint.h>

uint32_t port_getSpiEventIRQn(uint8_t driverId)
{
  return driverId + 1;
}

bool port_initSpiModule(spiCfg_t *cfg)
{
  return true;
}

uint32_t port_getUartEventIRQn(uint8_t driverId)
{
  return driverId + 1;
}

bool port_initUartModule(spiCfg_t *cfg)
{
  return true;
}
