#include <fmt_flash.h>
#include <fmt_flash_port.h>
#include <stdbool.h>
#include <stdint.h>

bool flash_isErased(uint32_t address, uint32_t len)
{
  const uint32_t* endAddress = (uint32_t*)(address + len);
  for (uint32_t* ptr = (uint32_t*)address; ptr < endAddress; ptr++)
  {
    if (*ptr != ERASED_STATE)
      return false;
  }
  return true;
}

uint32_t getPreceedingWriteBoundary(uint32_t address)
{
  return (address / WRITE_BLOCK_SIZE) * WRITE_BLOCK_SIZE;
}