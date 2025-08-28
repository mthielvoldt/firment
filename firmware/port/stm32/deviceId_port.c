#include <deviceId_port.h>

#define HAL_BASE_ENABLED
#include <stm32_hal_dispatch.h>
#include <string.h>

static deviceId_t id = {};
static uint8_t isSet = 0;

deviceId_t getDeviceId(void)
{
  if (isSet == 0)
  {
    memcpy(&id, (uint32_t*)UID_BASE, 12);
    isSet = 1;
  }
  return id;
}