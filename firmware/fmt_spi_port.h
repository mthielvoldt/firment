#include <fmt_spi.h>
#include <stdint.h>

bool port_initSpiModule(spiCfg_t *cfg);

/** port_getSpiEventIRQn
 * @param spiModuleNo should match the number of the Driver_SPIx that the IRQn
 * is being requested for. 
 * @returns the IRQn of the spi module indicated by spiModuleNo, provided that 
 * module is configured.  If that module is not configured, returns 0. 
 * 
 * This file should be implemented in a platform-specific _port.c file.
 * 
 * Note: one of the following files must configure the 
 * SPI resource to be available.
 * spi_pcbDetails.h (this is where firment configures the options)
 * RTE_Device.h (other CMSIS BSP desktop apps configure them here.)
 * 
 * For example with the following lines in RTE_Device.h:
 * #define RTE_SPI2 1 
 * #define RTE_SPI5 1
 * 
 * Data structures for SPI modules 2 and 5 will be available, a call to
 * port_getSpiEventIRQn(2) will return the IRQn for SPI2. 
 */
uint32_t port_getSpiEventIRQn(uint8_t spiDriverId);