/**
 * fw_update.h
 * Provides address where new images should be written (so BL will see them).
 *   This comes from CMake, as it is also needed by bootloader.
 */
 
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
 