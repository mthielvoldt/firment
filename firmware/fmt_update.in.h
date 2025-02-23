/**
 * fw_update.h
 * Provides address where new images should be written (so BL will see them).
 *   This comes from CMake, as it is also needed by bootloader.
 */

#include "messages.pb.h"
#include <stdint.h>
 
// This needs to be gotten from project-level cmake (shared with bootloader). 
// XMC_FLASH_SECTOR_(0-8): Bootloader, 256kB total. 
// XMC_FLASH_SECTOR_9:     Active App, 256kB
// XMC_FLASH_SECTOR_10:    App download, 256kB  <-- This file only uses this.
// XMC_FLASH_SECTOR_11:    Swap space, 256kB
#define IMAGE_RX_ADDRESS     (0x0C000000U + 0x80000U)
#define IMAGE_RX_SECTOR_SIZE (0x40000UL)

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 256
#endif

// Must be a factor of FLASH_PAGE_SIZE.
#define IMAGE_CHUNK_MAX_SIZE 32

#define CHUNKS_PER_PAGE_MAX  (FLASH_PAGE_SIZE / IMAGE_CHUNK_MAX_SIZE)
#define PAGES_PER_SECTOR_MAX (IMAGE_RX_SECTOR_SIZE / FLASH_PAGE_SIZE)

#define USE_ImageData
void handleImageData(ImageData msg);
