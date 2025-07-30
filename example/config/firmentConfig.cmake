
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

# variable PCB set on cmake invocation. 
# Extract the FMT_TRANSPORT {SPI,UART} from relevant comm_pcbDetails.h 
set(PCB_DIR ${APP_FW_CONFIG_DIR}/pcb${PCB})
file(READ ${PCB_DIR}/comm_pcbDetails.h HEADER_CONTENTS)

string(REGEX MATCH "#define[ \t]+FMT_USES_([A-Z]+)[ \t]+1" FULL_MATCH "${HEADER_CONTENTS}" )
set(FMT_TRANSPORT ${CMAKE_MATCH_1})

if(FMT_TRANSPORT STREQUAL "SPI" OR FMT_TRANSPORT STREQUAL "UART")
  message(STATUS "Firment using ${FMT_TRANSPORT}")
else()
  message(SEND_ERROR "Unknown FMT_TRANSPORT: ${FIRMENT_TRANSPORT}")
endif()

message(STATUS "Update page size: ${UPDATE_PAGE_SIZE}")
message(STATUS "Message payload size max: ${DATA_MSG_PAYLOAD_SIZE_MAX}")
message(STATUS "Log text max size: ${LOG_TEXT_MAX_SIZE}")
