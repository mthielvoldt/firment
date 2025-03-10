#include <fmt_spi.h>
#include "ISR_Config.h"

extern ARM_DRIVER_SPI Driver_SPI4;

spiCfg_t spiConfig = {
  .spiModuleId = 4,
  .spiModule = &Driver_SPI4,
  .msgWaitingIocId = 1,  // IOC_14: P1_0 see Device_IOC.h and RTE_DeviceConfig.h
  .msgWaitingIocOut = 3,
  .clearToSendIocId = 0, // IOC_4:  P0_4 see Device_IOC.h and RTE_DeviceConfig.h
  .clearToSendIocOut = 2,
  .baudHz = 1000000,
  .busMode = BUS_MODE_MAIN,
  .ssActiveLow = true,
  .irqPriority = spiTxBuf_priority, // TODO: check if this is used.
};

bool project_initSpi(void)
{
  return fmt_initSpi(spiConfig);
}