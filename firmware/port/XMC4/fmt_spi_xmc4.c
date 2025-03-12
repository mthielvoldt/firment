// This is a MFG-specific header file name. 
#include <spi_pcbDetails.h>
#include <SPI.h>

#if RTE_SPI0
extern SPI_RESOURCES SPI0_Resources;
#endif
#if RTE_SPI1
extern SPI_RESOURCES SPI1_Resources;
#endif
#if RTE_SPI2
extern SPI_RESOURCES SPI2_Resources;
#endif
#if RTE_SPI3
extern SPI_RESOURCES SPI3_Resources;
#endif
#if RTE_SPI4
extern SPI_RESOURCES SPI4_Resources;
#endif
#if RTE_SPI5
extern SPI_RESOURCES SPI5_Resources;
#endif

uint32_t port_getSpiEventIRQn(uint8_t spiModuleId)
{
  switch (spiModuleId) {
    case 0:
    #if RTE_SPI0
    return SPI0_Resources.irq_rx_num;
    #else
    return 0;
    #endif

    case 1:
    #if RTE_SPI1
    return SPI1_Resources.irq_rx_num;
    #else
    return 0;
    #endif

    case 2:
    #if RTE_SPI2
    return SPI2_Resources.irq_rx_num;
    #else
    return 0;
    #endif

    case 3:
    #if RTE_SPI3
    return SPI3_Resources.irq_rx_num;
    #else
    return 0;
    #endif

    case 4:
    #if RTE_SPI4
    return SPI4_Resources.irq_rx_num;
    #else
    return 0;
    #endif

    case 5:
    #if RTE_SPI5
    return SPI5_Resources.irq_rx_num;
    #else
    return 0;
    #endif
  }
  return 0;
}
