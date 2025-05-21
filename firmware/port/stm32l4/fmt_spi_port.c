// This is a MFG-specific header file name. 
#include <fmt_spi.h>        // port_getSpiEventIRQn declaration
#include <MX_Device.h>      // MX_SPIx
#include <stm32l476xx.h>


uint32_t port_getSpiEventIRQn(uint8_t spiModuleId)
{
  switch (spiModuleId) {
    case 0:
    #if MX_SPI0
    return SPI0_IRQn;
    #else
    return 0;
    #endif

    case 1:
    #if MX_SPI1
    return SPI1_IRQn;
    #else
    return 0;
    #endif

    case 2:
    #if MX_SPI2
    return SPI2_IRQn;
    #else
    return 0;
    #endif

    case 3:
    #if MX_SPI3
    return SPI3_IRQn;
    #else
    return 0;
    #endif

    case 4:
    #if MX_SPI4
    return SPI4_IRQn;
    #else
    return 0;
    #endif

    case 5:
    #if MX_SPI5
    return SPI5_IRQn;
    #else
    return 0;
    #endif
  }
  return 0;
}
