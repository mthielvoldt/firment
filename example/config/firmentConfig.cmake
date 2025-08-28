
# The location of the firment directory
set(FIRMENT_DIR ${PROJECT_SOURCE_DIR}/..)
set(APP_FW_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/../firmware/config)

# PCB should be set on cmake invocation via cmake ... -DPCB=<num>. 
set(PCB_DIR ${APP_FW_CONFIG_DIR}/pcb${PCB})

## Project handle (8 characters or fewer)
set(PROJECT_STR "fmt-ex")

## Optional modules
set(ENABLE_WAVEFORM 1)
set(ENABLE_GHOST_PROBE 1)
include(${FIRMENT_DIR}/cmake-tools/fmtTransport.cmake)

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
