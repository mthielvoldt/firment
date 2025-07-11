
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(APP_FW_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/../firmware/config)

# update_page_size is used in:
# - web-ui for <Image> upload widget (via /web-ui/src/generated/flashPage.ts)
# - fmt_update.c from configured fmt_update.h
set(UPDATE_PAGE_SIZE 256)
set(DATA_MSG_PAYLOAD_SIZE_MAX 32)
set(LOG_TEXT_MAX_SIZE      50)
set(PROBE_SIGNAL_MAX_COUNT 6) # Todo: remove

message(STATUS "Update page size: ${UPDATE_PAGE_SIZE}")
message(STATUS "Message payload size max: ${DATA_MSG_PAYLOAD_SIZE_MAX}")
message(STATUS "Log text max size: ${LOG_TEXT_MAX_SIZE}")
