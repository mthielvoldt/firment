#include <deviceId_port.h>
#include <xmc_device.h>

deviceId_t getDeviceId(void)
{
  return *(deviceId_t*)g_chipid;
}