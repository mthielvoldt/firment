# Extract the FMT_TRANSPORT {SPI,UART} from relevant comm_pcbDetails.h 

file(READ ${PCB_DIR}/comm_pcbDetails.h HEADER_CONTENTS)
string(REGEX MATCH "#define[ \t]+FMT_USES_([A-Z]+)[ \t]+1" FULL_MATCH "${HEADER_CONTENTS}" )
set(FMT_TRANSPORT ${CMAKE_MATCH_1})

if(FMT_TRANSPORT STREQUAL "SPI" OR FMT_TRANSPORT STREQUAL "UART")
  message(STATUS "Firment using ${FMT_TRANSPORT}")
else()
  message(SEND_ERROR "Unknown FMT_TRANSPORT: ${FIRMENT_TRANSPORT}")
endif()