#include <flash_port.h>
#include <clock_port.h>
#include <fmt_flash.h>
#include <fmt_sysInit.h>

int flash_program(uint32_t address, const uint8_t *data, uint32_t len)
{
  return fmt_flash_write(address, data, len);
}

int initClock(void)
{
  fmt_initSys();
  return 0;
}