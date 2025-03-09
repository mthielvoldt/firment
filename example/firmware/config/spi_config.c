#include <fmt_spi.h>

extern ARM_DRIVER_SPI Driver_SPI4;

spiCfg_t spiConfig = {
  .spiModuleNo = 4,
  .spiModule = &Driver_SPI4,
  .msgWaitingIocId = 14, // P0_1, ERU0  see Device_IOC.h
  .msgWaitingIocOut = 3,
  .clearToSendIocId = 4, // P0_4, ERU0  see Device_IOC.h
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