
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(APP_FW_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/../firmware/config)

# These needed if you're using Firmware Update fmt_update.c
set(FMT_IMAGE_DOWNLOAD_ADDRESS  ${PARTITION_UPDATE_ADDRESS})
set(FMT_IMAGE_DOWNLOAD_SECTOR_SIZE ${SECTOR_SIZE})