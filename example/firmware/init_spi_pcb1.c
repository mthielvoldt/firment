/**
 * Concerns: 
 * - Knows which pin is routed on PCB to which purpose (CTS, msgWait).
 * - Knows what role this chip plays on the spi bus (Main, sub).
 * - Communicates this information to the spi driver.
 */
#include <fmt_spi.h>
#include <spi_pcbDetails.h> // replaces RTEDevice.h for ARM SPI driver
#include <ioc_pcbDetails.h>
#include <ioc_mcuDetails.h> // IOC_x_to_ISR_y mcu-supported routing options
#include <priority.h>

SPI_HandleTypeDef hspi3;
extern ARM_DRIVER_SPI Driver_SPI3;

spiCfg_t spiConfig = {
  .spiModuleId = 4,
  .spiModule = &Driver_SPI3,
  .msgWaitingIocId = IOC_14_RTE_INDEX, // IOC_14: P1_0 on XMC4700
  .msgWaitingIocOut = 0,  // not used in stm32; no output routing stage in IOC
  .clearToSendIocId = IOC_4_RTE_INDEX, // IOC_4:  P0_4 on XMC4700
  .clearToSendIocOut = 0, // not used in stm32; no output routing stage in IOC
  .baudHz = 1000000,
  .busMode = BUS_MODE_MAIN,
  .ssActiveLow = true,
  .irqPriority = spiTxBuf_priority,
};

bool project_initSpi(void)
{
  return fmt_initSpi(spiConfig);
}