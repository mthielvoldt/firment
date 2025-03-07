/** fmt_flash_port.c for Infineon XMC4000 series. 
 * 
 */

#include <stdint.h>
#include <xmc4_flash.h>
#include "fmt_flash_port.h"


#ifndef ARCH_FLASH_OFFSET
// Access address that bypasses the prefetch cache. Table 7-2
#define ARCH_FLASH_OFFSET XMC_FLASH_UNCACHED_BASE
#endif

/* Assembly helpers - Data Memory Barrier */
// #define DMB() asm volatile ("dmb")

/*** FLASH ***/
#define FLASH_TOP      (XMC_FLASH_UNCACHED_BASE + 0x0200000UL)

#define FLASH_SECTOR_COUNT 16
const uint32_t sector_base[FLASH_SECTOR_COUNT + 1] = {
    (uint32_t)XMC_FLASH_SECTOR_0,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_1,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_2,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_3,  /* 16 Kb   */

    (uint32_t)XMC_FLASH_SECTOR_4,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_5,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_6,  /* 16 Kb   */
    (uint32_t)XMC_FLASH_SECTOR_7,  /* 16 Kb   */

    (uint32_t)XMC_FLASH_SECTOR_8,  /* 128 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_9,  /* 256 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_10, /* 256 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_11, /* 256 Kb  */

    (uint32_t)XMC_FLASH_SECTOR_12, /* 256 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_13, /* 256 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_14, /* 256 Kb  */
    (uint32_t)XMC_FLASH_SECTOR_15, /* 256 Kb  */
    FLASH_TOP
};

static uint32_t getPreceedingPageBoundary(uint32_t address);
static int getSectorContainingAddress(uint32_t address);

void hal_init(void)
{
  // Max the system clock so crypto ops proceed quickly.
}

void hal_prepare_boot(void)
{
  // Put sysclock back to normal system startup defaults.
}

void hal_flash_unlock(void)
{
  // There is no global flash unlock on XMC.
  // Each page write and sector erase operation requires a command sequence.

  // If write protection is enabled, it is temporarily disabled sector-by-
  // sector with a pre-stored user password.
}

void hal_flash_lock(void) { }

int hal_flash_write(uint32_t address, const uint8_t *data, int len)
{
  uint8_t page_buffer[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
  
  /* adjust for flash base to allow for both offsets and absolute addresses. */
  if (address < ARCH_FLASH_OFFSET) {
    address += ARCH_FLASH_OFFSET;
  }
  
  /* Find the closest page-aligned address preceeding first address to write*/
  uint32_t page_adr = getPreceedingPageBoundary(address);
  uint32_t final_write_end_adr = address + len;
  uint32_t page_write_end_adr;   // One past the last address to be written.
  uint32_t page_write_start_adr; // The first address to be written. 
  uint32_t bytes_written = 0;    // count of bytes written in previous pages.

  while (page_adr < final_write_end_adr) {
    memset(page_buffer, 0, FLASH_PAGE_SIZE);
    page_write_start_adr = address + bytes_written;
    page_write_end_adr = (page_adr + FLASH_PAGE_SIZE < final_write_end_adr) ? 
      page_adr + FLASH_PAGE_SIZE : final_write_end_adr;
    
    //  WRITE FIRST PAGE
    //      _______buffer______ 
    //     |........===========|========............|
    //      ^       ^           ^
    //  page_adr  wr_start    wr_end
    //
    //  WRITE SECOND PAGE
    //                          _______buffer_______
    //     |........===========|========............|
    //                          ^       ^
    //                      page_adr  wr_end
    //                      wr_start
    memcpy(page_buffer + (page_write_start_adr - page_adr),
           data + bytes_written,
           page_write_end_adr - page_write_start_adr
    );
    XMC_FLASH_ProgramPage((uint32_t*)page_adr, (uint32_t*)page_buffer);

    // Prepare for next page.
    bytes_written += page_write_end_adr - page_write_start_adr;
    page_adr += FLASH_PAGE_SIZE;
  }
  return 0;
}

bool isSectorErased(int sector)
{
  uint32_t *address, *endAddress;
  address = (uint32_t*)sector_base[sector];
  endAddress = (uint32_t*)sector_base[sector + 1];

  for (; address < endAddress; address++ ) {
    if (*address != 0)
    {
      return false;
    }
  }
  return true;
}

int hal_flash_erase(uint32_t start_address, int len)
{
  // Work with absolute addresses. 
  if (start_address < ARCH_FLASH_OFFSET) {
    start_address += ARCH_FLASH_OFFSET;
  }
  if (len == 0) {
    return -1;
  }
  
  uint32_t end_address = start_address + len - 1; 

  int start_sector = getSectorContainingAddress(start_address);
  int end_sector = getSectorContainingAddress(end_address);
  if (start_sector < 0 || end_sector < 0){
    return -1;
  }

  for (int sector = start_sector; sector <= end_sector; sector++)
  {
    if (!isSectorErased(sector))
    {
      XMC_FLASH_EraseSector((uint32_t*)sector_base[sector]);
    }
  }
  return 0;
}

/**
 * @param address must be an absolute address, not an offset.
 */
static uint32_t getPreceedingPageBoundary(uint32_t address)
{
  return (address / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
}

/**
 * @param address must be an absolute address (not an offset).
 */
static int getSectorContainingAddress(uint32_t address) {
  int ret = -1;
  for (int sectorIdx = 0; sectorIdx < FLASH_SECTOR_COUNT; sectorIdx++) {
    if ((address >= sector_base[sectorIdx]) && \
        (address <  sector_base[sectorIdx + 1])) {
          ret = sectorIdx;
    }
  }
  return ret;
}
