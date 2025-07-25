
#include <stdint.h>
#include <stdbool.h>

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
int fmt_flash_write(uint32_t address, const uint8_t *data, uint32_t len);

/** flash_isErased 
 * @note This is intended for internal use because fmt_flash_write handles 
 * checking if an erase is needed before programming.  However, flash_isErased
 * may be useful for testing, and is a hardware-agnostic.  Implementation is in 
 * port/common/flash_common.c
 * 
 */
bool flash_isErased(uint32_t address, uint32_t len);

uint32_t getPreceedingWriteBoundary(uint32_t address);