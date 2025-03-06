
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)

# These needed if you're using Firmware Update fmt_update.c

set(FMT_IMAGE_DOWNLOAD_ADDRESS  0x0C080000U)
# TODO: (8) These need to be runtime-settable for test.
set(FMT_IMAGE_DOWNLOAD_SECTOR_SIZE 0x40000U)