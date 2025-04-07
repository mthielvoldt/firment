
#include <stdint.h>

/** fmt_flash_write
 * @param address Either an offset from the program memory base address, or an
 * absolute address.  Un-aligned addresses are permitted.
 * @param data the payload to be stored in the flash.
 * @param len the size of the payload. Valid range: [1, WOLFBOOT_SECTOR_SIZE]
 * @returns 0 on success, a negative value on failure.
 * The XMC flash memory model only supports writing aligned 256-byte pages at a
 * time.  So this function implements a read-modify-write process than handles
 * both un-aligned start and end addresses.
 */
int fmt_flash_write(uint32_t address, const uint8_t *data, int len);

/** fmt_flash_erase
 * Erase all the sectors within the specified range, accounting for flash sector
 * geometries.
 * @param start_address marks the start of the area to erase. start_address is
 * guaranteed to be aligned to WOLFBOOT_SECTOR_SIZE.
 * @param len specifies the size of the area to be erased. Len is guaranteed to 
 * be a multiple of WOLFBOOT_SECTOR_SIZE. 
 */
int fmt_flash_erase(uint32_t start_address, int len);