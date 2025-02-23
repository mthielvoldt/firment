
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)

# These needed if you're using Firmware Update fmt_update.c

# XMC_FLASH_SECTOR_(0-8): Bootloader, 256kB total.
# XMC_FLASH_SECTOR_9:     Active App, 256kB
# XMC_FLASH_SECTOR_10:    App download, 256kB  <-- This file only uses this.
# XMC_FLASH_SECTOR_11:    Swap space, 256kB
set(FMT_IMAGE_DOWNLOAD_ADDRESS  0x0C080000U)
set(FMT_IMAGE_DOWNLOAD_SECTOR_SIZE 0x40000U)