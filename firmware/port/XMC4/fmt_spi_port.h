/** fmt_spi_port.h
 * This file and accompanying hardware-specific implementation provide a 
 * mechanism for retrieving
 * the IRQ number for a SPI module that has been configured with
 * RTE_Device.h / RTE_DeviceConfig.h.  This is necessary because the CMSIS 
 * interface omits a way to set the prioiry of the SPI event callback ISR.
 * 
 * That control is essential to ensuring buffers don't overflow in a runtime
 * with long-running ISRs and high data Rx rates.  
 * 
 * To set the priority, one simply requires the IRQn on arm devices, so this 
 * is how we get it.
 */
#include <stdint.h>

/** getSpiEventIRQn
 * @param spiModuleNo should match the number of the Driver_SPIx that the IRQn
 * is being requested for. 
 * @returns the IRQn of the spi module indicated by spiModuleNo, provided that 
 * module is configured.  If that module is not configured, returns 0. 
 * 
 * Note: one of the following files must configure the 
 * SPI resource to be available.
 * RTE_DeviceConfig.h (firment way)
 * RTE_Device.h (other CMSIS BSPs)
 * 
 * For example with the following lines in RTE_Device.h:
 * #define RTE_SPI2 1 
 * #define RTE_SPI5 1
 * 
 * Data structures for SPI modules 2 and 5 will be available, a call to
 * getSpiEventIRQn(2) will return the IRQn for SPI2. 
 */
uint32_t getSpiEventIRQn(uint8_t spiModuleNo);
