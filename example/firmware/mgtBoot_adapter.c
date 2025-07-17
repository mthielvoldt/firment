#include <flash_port.h>
#include <clock_port.h>
#include <fmt_flash.h>
#include <fmt_sysInit.h>

int flash_program(uint32_t address, const uint8_t *data, uint32_t len)
{
  return fmt_flash_write(address, data, len);
}

// TODO: remove (flash_program handles this)
int flash_erase(uint32_t address, uint32_t len)
{
  return fmt_flash_erase(address, len);
}

int initClock(void)
{
  fmt_initSys();
  return 0;
}