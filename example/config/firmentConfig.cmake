
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(APP_FW_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/../firmware/config)

# update_page_size is used in:
# - web-ui for <Image> upload widget (via /web-ui/src/generated/flashPage.ts)
# - fmt_update.c from configured fmt_update.h
set(UPDATE_PAGE_SIZE 256)
message(STATUS "Update page size: ${UPDATE_PAGE_SIZE}")