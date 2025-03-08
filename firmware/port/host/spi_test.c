#include <spi_test.h>
#include <stdint.h>
#include <string.h>

static int32_t callCounts[FUNCTION_TYPE_COUNT] = {0};

int32_t getCallCount(fnType_t fnType)
{
  return callCounts[fnType];
}

void resetCallCounts(void)
{
  memset(callCounts, 0, sizeof(callCounts));
}

/** PRIVATE FUNCTIONS  */

static int32_t Initialize(ARM_SPI_SignalEvent_t cb_event)
{
  callCounts[INITIALIZE]++;
  return ARM_DRIVER_OK;
}
static int32_t Uninitialize(void);
static int32_t PowerControl(ARM_POWER_STATE state)
{
  callCounts[POWER_CONTROL]++;
  return ARM_DRIVER_OK;
}
static int32_t Transfer(const void *data_out, void *data_in, uint32_t num)
{
  callCounts[TRANSFER]++;
  return ARM_DRIVER_OK;
}
static int32_t Control(uint32_t control, uint32_t arg)
{
  callCounts[CONTROL]++;
  return ARM_DRIVER_OK;
}
static ARM_SPI_STATUS GetStatus(void)
{
  callCounts[GET_STATUS]++;
  return (ARM_SPI_STATUS){.busy = 0, .data_lost = 0, .mode_fault = 0};
}

ARM_DRIVER_SPI Driver_SPI3 = {
  .Initialize = Initialize,
  .PowerControl = PowerControl,
  .Transfer = Transfer,
  .Control = Control,
  .GetStatus = GetStatus,
};