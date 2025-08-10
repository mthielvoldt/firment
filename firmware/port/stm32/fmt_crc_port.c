#include "fmt_crc.h"
#define HAL_CRC_ENABLED
#define HAL_RCC_ENABLED
#include <stm32_hal_dispatch.h>

#define FMT_CRC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(0, 0)

static const ARM_DRIVER_VERSION_t DriverVersion = {
    .api = FMT_CRC_API_VERSION,
    .drv = FMT_CRC_DRV_VERSION,
};

static const FMT_CRC_CAPABILITIES_t DriverCapabilities = {
    .crc8_sae_j1850 = 1,
    .crc16_ccitt = 1,
    .crc32_ieee802p3 = 1,
    .reflect_input = 1,
    .reflect_result = 1,
    .invert_result = 1,
    .config_err = 1,
    .bus_err = 1,
};

static CRC_HandleTypeDef crcHandle[1] = {{
    .Init = {
        .CRCLength = CRC_POLYLENGTH_16B,
        .DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE, // default 0xFFFFFFFF
        .InitValue = 0xFFFFFFFF,
        .DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE,
        .GeneratingPolynomial = 0x1021, // crc16_ccitt
        .InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE,
        .OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE,
    },
    .InputDataFormat = CRC_INPUTDATA_FORMAT_HALFWORDS,
    .Instance = CRC,
    .Lock = HAL_UNLOCKED,
    .State = HAL_CRC_STATE_RESET,
}};

static int armCodeFromHalStatus[] = {
    [HAL_OK] = ARM_DRIVER_OK,
    [HAL_ERROR] = ARM_DRIVER_ERROR,
    [HAL_BUSY] = ARM_DRIVER_ERROR_BUSY,
    [HAL_TIMEOUT] = ARM_DRIVER_ERROR_TIMEOUT,
};

static bool isInitialized = false;

static ARM_DRIVER_VERSION_t GetVersion(void) { return DriverVersion; }

static FMT_CRC_CAPABILITIES_t GetCapabilities(void) { return DriverCapabilities; }

/**
 * This overrides a weak definition that does nothing.  The _MspInit functions
 * are the place STM enables the peripheral clocks.
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{
  __HAL_RCC_CRC_CLK_ENABLE();
}

static int32_t Initialize(void)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  if (!isInitialized)
  {
    status = HAL_CRC_Init(crcHandle);
    isInitialized = (status == HAL_OK);
  }
  return armCodeFromHalStatus[status];
}

static int32_t Uninitialize(void)
{
  isInitialized = false;
  HAL_StatusTypeDef status = HAL_CRC_DeInit(crcHandle);
  return armCodeFromHalStatus[status];
}

static int32_t PowerControl(ARM_POWER_STATE state)
{
  int32_t ret = ARM_DRIVER_ERROR;
  switch (state)
  {
  case ARM_POWER_FULL:
    ret = ARM_DRIVER_OK;
    break;
  default:
  {
    ret = ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  }
  return ret;
}

static int32_t ComputeCRC(const uint8_t *data, uint32_t length, uint16_t *result)
{
  // HAL_CRC_Calculate doesn't mutate data (type should be const).
  length /= sizeof(*result);
  *result = HAL_CRC_Calculate(crcHandle, (uint32_t *)data, length);
  return ARM_DRIVER_OK;
}

static int32_t Control(uint32_t control, uint32_t arg)
{
  int32_t ret = ARM_DRIVER_ERROR;

  switch (control)
  {
  case FMT_CRC_SET_RESULT_INVERSION:
  {
    break;
  }
  case FMT_CRC_SET_RESULT_REFLECTION:
  {
    break;
  }
  case FMT_CRC_SET_INPUT_REFLECTION:
  {
    break;
  }
  case FMT_CRC_SET_CRC_ORDER:
  {
    break;
  }
  case FMT_CRC_CLEAR_ERROR:
  {
    break;
  }
  default:
  {
    ret = ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  }
  return ret;
}

static FMT_CRC_STATUS_t GetStatus(void)
{
  FMT_CRC_STATUS_t res = {0};
  switch (HAL_CRC_GetState(crcHandle))
  {
  case HAL_CRC_STATE_RESET:
  case HAL_CRC_STATE_READY:
  case HAL_CRC_STATE_BUSY:
  {
    break;
  }
  case HAL_CRC_STATE_TIMEOUT:
  case HAL_CRC_STATE_ERROR:
  {
    res.bus_err = 1;
    break;
  }
  }
  return res;
}

FMT_DRIVER_CRC Driver_CRC0 = {
    .GetVersion = GetVersion,
    .GetCapabilities = GetCapabilities,
    .Initialize = Initialize,
    .Uninitialize = Uninitialize,
    .PowerControl = PowerControl,
    .ComputeCRC = ComputeCRC,
    .Control = Control,
    .GetStatus = GetStatus,
};