#include <flash.h>
#include <fmt_flash.h>

int flash_program(uint32_t address, const uint8_t *data, uint32_t len)
{
  return fmt_flash_write(address, data, len);
}

int flash_erase(uint32_t address, uint32_t len)
{
  return fmt_flash_erase(address, len);
}