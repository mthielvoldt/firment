#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_spi.h>
}

static int32_t Initialize(ARM_SPI_SignalEvent_t cb_event)
{
  return 0;
}
static int32_t Uninitialize(void);
static int32_t PowerControl(ARM_POWER_STATE state)
{
  return 0;
}
static int32_t Transfer(const void *data_out, void *data_in, uint32_t num)
{
  return 0;
}
static int32_t Control(uint32_t control, uint32_t arg)
{
  return 0;
}
static ARM_SPI_STATUS GetStatus(void)
{
  return (ARM_SPI_STATUS){.busy = 0, .data_lost = 0, .mode_fault = 0};
}

ARM_DRIVER_SPI Driver_SPI3 = {
    .Initialize = Initialize,
    .PowerControl = PowerControl,
    .Transfer = Transfer,
    .Control = Control,
    .GetStatus = GetStatus,
};

TEST_GROUP(fmt_spi){
    void setup(){

    } void teardown(){

    }};

TEST(fmt_spi, init)
{
  spiCfg_t cfg = {
      .spiModuleId = 3,
      .spiModule = &Driver_SPI3,
      .msgWaitingIocId = 2,
      .clearToSendIocId = 0,
      .baudHz = 1000000,
      .ssActiveLow = false,
      .irqPriority = 16};
  bool success = fmt_initSpi(cfg);
  CHECK_TRUE(success);
}