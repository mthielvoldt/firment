/**
 * Concerns: 
 * - Knows which pin is routed on PCB to which purpose (CTS, msgWait).
 * - Knows what role this chip plays on the spi bus (Main, sub).
 * - Communicates this information to the spi driver.
 */
#include <fmt_spi.h>
#include "spi_pcbDetails.h" // replaces RTEDevice.h for ARM SPI driver
#include "ioc_pcbDetails.h"
#include <ioc_mcuDetails.h> // IOC_x_to_ISR_y mcu-supported routing options
#include "ISR_Config.h"

extern ARM_DRIVER_SPI Driver_SPI4;

spiCfg_t spiConfig = {
  .spiModuleId = 4,
  .spiModule = &Driver_SPI4,
  .msgWaitingIocId = IOC_14_RTE_INDEX, // IOC_14: P1_0 on XMC4700
  .msgWaitingIocOut = IOC_14_to_ISR_3,
  .clearToSendIocId = IOC_4_RTE_INDEX, // IOC_4:  P0_4 on XMC4700
  .clearToSendIocOut = IOC_4_to_ISR_2,
  .baudHz = 1000000,
  .busMode = BUS_MODE_MAIN,
  .ssActiveLow = true,
  .irqPriority = spiTxBuf_priority, // TODO: check if this is used.
};

bool project_initSpi(void)
{
  return fmt_initSpi(spiConfig);
}