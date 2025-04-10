
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(PROJECT_CONFIG_DIR ${PROJECT_SOURCE_DIR}/firmware/config)

# These needed if you're using Firmware Update fmt_update.c
set(FMT_IMAGE_DOWNLOAD_ADDRESS  ${PARTITION_UPDATE_ADDRESS})
set(FMT_IMAGE_DOWNLOAD_SECTOR_SIZE ${SECTOR_SIZE})