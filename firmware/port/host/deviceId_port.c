#include <deviceId_port.h>

deviceId_t getDeviceId(void)
{
  return (deviceId_t){.dblWords = {1111, 2222}};
}