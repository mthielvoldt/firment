#include "crc.h"
#include <xmc_fce.h>

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

// Default init config
static XMC_FCE_t engine = {
    .kernel_ptr = FCE_KE2, // Supports CRC16
    .fce_cfg_update = {
        .config_refin = 0,  // don't mirror (reflect) input words.
        .config_refout = 0, // don't mirror (reflect) CRC result.
        .config_xsel = 0,   // don't invert CRC result.
    },
    .seedvalue = 0,
};

static bool isInitialized = false;

ARM_DRIVER_VERSION_t GetVersion(void) { return DriverVersion; }

FMT_CRC_CAPABILITIES_t GetCapabilities(void) { return DriverCapabilities; }

static int32_t Initialize(void)
{
  if (!isInitialized)
  {
    XMC_FCE_Init(&engine); // always returns "OK".
    isInitialized = true;
  }
  return ARM_DRIVER_OK;
}

static int32_t Uninitialize(void)
{
  isInitialized = false;
  XMC_FCE_Disable();
  return ARM_DRIVER_OK;
}

static int32_t PowerControl(ARM_POWER_STATE state)
{
  int32_t ret = ARM_DRIVER_ERROR;
  switch (state) {
    case ARM_POWER_FULL:
    {
      XMC_FCE_Enable();
      ret = ARM_DRIVER_OK;
      break;
    }
    case ARM_POWER_OFF:
    {
      XMC_FCE_Disable();
      ret = ARM_DRIVER_OK;
      break;
    }
    default:
    {
      ret = ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  return ret;
}

static int32_t ComputeCRC(const void *data, uint32_t num, uint16_t *result)
{
  XMC_FCE_STATUS_t status;
  int32_t ret = ARM_DRIVER_ERROR; // Default to unspecified error.
  if (isInitialized)
  {
    status = XMC_FCE_CalculateCRC16(&engine, data, num, result);
    if (status == XMC_FCE_STATUS_OK)
    {
      ret = ARM_DRIVER_OK;
    }
    else if (status == XMC_FCE_STATUS_ERROR)
    {
      ret = ARM_DRIVER_ERROR_PARAMETER; // Is data size-aligned to CRC order?
    }
    else if (status == XMC_FCE_STATUS_BUSY)
    {
      ret = ARM_DRIVER_ERROR_BUSY;
    }
  }
  return ret;
}

static int32_t Control(uint32_t control, uint32_t arg)
{
  int32_t ret = ARM_DRIVER_ERROR;

  switch (control) {
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
      if (arg == FMT_CRC_BUS_ERR)
      {
        XMC_FCE_ClearEvent(&engine, XMC_FCE_STS_BUS_ERROR);
      }
      else if (arg == FMT_CRC_CONFIG_ERR)
      {
        XMC_FCE_ClearEvent(&engine, XMC_FCE_STS_CONFIG_ERROR);
      }
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
  res.config_err = XMC_FCE_GetEventStatus(&engine, XMC_FCE_STS_CONFIG_ERROR);
  res.bus_err = XMC_FCE_GetEventStatus(&engine, XMC_FCE_STS_BUS_ERROR);
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