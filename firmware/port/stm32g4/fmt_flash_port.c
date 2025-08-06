/** fmt_flash_port.c for STM32L4 family.
 *
 * STM32L4 uses two banks of flash.  CPU instruction read on one bank can happen
 * concurrently with program/erase operations on the *OTHER* bank, not the same.
 *
 * The banks are sized the same, each with 2kB pages (256 pages in each bank on 1MB devices).
 *
 * Erasing can be done by page or whole bank.
 *
 * Programming can be done one double-word (64-bits) at a time.
 *
 *
 * Special quirks:
 * - Programming can be accelerated if we know another woubld-word (64-bit) will
 *   will be programmed after this one.  See FLASH_TYPEPROGRAM_FAST in hal_flash
 */

#include "fmt_flash_port.h"
#include <fmt_flash.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define HAL_FLASH_ENABLED
#include <stm32_hal_dispatch.h>

/**
 * STM32L4 Flash is organized into 2 banks of 512kB each, (256X 2kB pages).
 * "Page" means a block that can be independently erased (not programmed).
 * CPU can continue fetching (reading) the bank that's not being written/erased.
 * Using fast-programming, 256 bytes (a row) must be programmed as a sequence.
 */
#define DOUBLEWORDS_PER_WRITE_BLOCK 32

static int getPageContainingAddress(uint32_t address);
static int flash_erase(uint32_t start_address, uint32_t len);

/**
 * @param address guaranteed to be aligned to the start of a writable block, but
 * not to the start of an erase-block. In STM32L4, writes are by 64-bit
 * double-word (8 bytes).
 */
int fmt_flash_write(uint32_t address, const uint8_t *data, uint32_t len)
{
  static uint64_t buffer[DOUBLEWORDS_PER_WRITE_BLOCK]; // 256B

  HAL_FLASH_Unlock();

  if (!flash_isErased(address, len))
    flash_erase(address, len);

  /* Find the closest page-aligned address preceeding first address to write*/
  uint32_t page_adr = getPreceedingWriteBoundary(address);
  uint32_t final_write_end_adr = address + len;
  uint32_t page_write_end_adr;   // One past the last address to be written.
  uint32_t page_write_start_adr; // The first address to be written.
  uint32_t bytes_written = 0;    // count of bytes written in previous pages.

  while (page_adr < final_write_end_adr)
  {
    page_write_start_adr = address + bytes_written;
    if (page_adr + WRITE_BLOCK_SIZE < final_write_end_adr)
    {
      page_write_end_adr = page_adr + WRITE_BLOCK_SIZE;
    }
    else
    {
      page_write_end_adr = final_write_end_adr;
    }

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
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer + (page_write_start_adr - page_adr),
           data + bytes_written,
           page_write_end_adr - page_write_start_adr);

    /* the Data parameter (last) must be a buffer address (32 dbl-words) when
    using row fast programming, despite the integer type.  This is documented
    in the comments preceeding this function's definition.*/
    // HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST_AND_LAST, page_adr, (uint32_t)buffer);

    // TODO: Get fast row programming working, and replace this loop. 
    for (int i = 0; i < DOUBLEWORDS_PER_WRITE_BLOCK; i++)
    {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, page_adr + (8*i), buffer[i]);
    }

    // Prepare for next page.
    bytes_written += page_write_end_adr - page_write_start_adr;
    page_adr += WRITE_BLOCK_SIZE;
  }
  HAL_FLASH_Lock();
  return 0;
}

static int flash_erase(uint32_t start_address, uint32_t len)
{
  uint32_t end_address = start_address + len - 1;

  int start_page = getPageContainingAddress(start_address);
  int end_page = getPageContainingAddress(end_address);
  if (start_page < 0 || end_page < 0 || len == 0)
  {
    return -1;
  }
  int page_count = end_page - start_page + 1;

  FLASH_EraseInitTypeDef eraseInit = {
      .TypeErase = FLASH_TYPEERASE_PAGES,
      .Banks = FLASH_BANK_1,
      .NbPages = page_count,
      .Page = start_page, // is page zero at bottom of this bank, or absolute number?
  };
  uint32_t pageError = FLASH_ERROR_NONE; // check this.
  HAL_FLASHEx_Erase(&eraseInit, &pageError);

  return 0;
}

static int getPageContainingAddress(uint32_t address)
{
  return (address - FLASH_BASE) / FLASH_PAGE_SIZE;
}