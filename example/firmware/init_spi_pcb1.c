/**
 * Concerns: 
 * - Knows which pin is routed on PCB to which purpose (CTS, msgWait).
 * - Knows what role this chip plays on the spi bus (Main, sub).
 * - Communicates this information to the spi driver.
 */
#include "init_spi.h"
#include <fmt_spi.h>
#include <spi_pcbDetails.h> // replaces RTEDevice.h for ARM SPI driver
#include <ioc_pcbDetails.h>
#include <ioc_mcuDetails.h> // IOC_x_to_ISR_y mcu-supported routing options
#include <priority.h>

extern ARM_DRIVER_SPI Driver_SPI2;

spiCfg_t spiConfig = {
  .spiDriverId = 2,
  .spiModule = &Driver_SPI2,
  .msgWaitingIocId = MSG_WAIT_IOC_ID, // 
  .msgWaitingIocOut = 0,  // not used in stm32; no output routing stage in IOC
  .clearToSendIocId = CTS_IOC_ID, // 
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