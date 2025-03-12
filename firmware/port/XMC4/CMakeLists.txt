
set(XMC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/XMCLib")
set(XMC_SERIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/CMSIS/Infineon/COMPONENT_XMC4700")
set(STARTUP_FILE "${XMC_SERIES_DIR}/Source/TOOLCHAIN_GCC_ARM/startup_${MCU_VARIANT}.S")
set(SYSTEM_FILE  "${XMC_SERIES_DIR}/Source/system_${MCU_VARIANT}.c")
if(MCU_VARIANT STREQUAL "XMC4700" OR MCU_VARIANT STREQUAL "XMC4800")
set(RTE_DEVICE_DIR "4800_F144")
endif()

add_library(MCUPort
  ${SYSTEM_FILE}
  ${STARTUP_FILE}
  XMC4000_DFP/RTE_Driver/SPI.c
  ${XMC_LIB}/src/xmc_ccu4.c
  ${XMC_LIB}/src/xmc_eru.c
  ${XMC_LIB}/src/xmc_fce.c
  ${XMC_LIB}/src/xmc_spi.c
  ${XMC_LIB}/src/xmc_usic.c
  ${XMC_LIB}/src/xmc4_eru.c
  ${XMC_LIB}/src/xmc4_flash.c
  ${XMC_LIB}/src/xmc4_gpio.c
  ${XMC_LIB}/src/xmc4_scu.c
  fmt_crc_xmc4.c
  fmt_flash_xmc4.c
  gpio_xmc4.c
  ioc_xmc4.c
  fmt_periodic_xmc4.c
  fmt_spi_xmc4.c
)

target_include_directories(MCUPort
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ../..   # fmt headers.  TODO: this seems wrong.
    XMC4000_DFP/RTE_Driver
    XMC4000_DFP/CMSIS/RTOS/RTX/INC
    ../../lib/CMSIS-6.1.0/Driver/Include
    ${PROJECT_CONFIG_DIR}
    PUBLIC
    ../../lib/CMSIS-6.1.0/Core/Include # core_cm4.h
    ${RTE_DEVICE_DIR}
    ${XMC_LIB}/inc  # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h
    ${XMC_SERIES_DIR}/Include # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h > XMC4700.h
)

# Tells the assembler what architecture the toolchain file specifies. 
target_compile_options(MCUPort PRIVATE ${ASSEMBLER_FLAGS})

# Contains the page size for the flash write operations. This propagates up so 
# web-ui can break FW-image sends into page-size chunks, waiting in between.
file(READ ${XMC_LIB}/inc/xmc4_flash.h FLASH_HEADER_CONTENTS)
string(REGEX MATCH "#define XMC_FLASH_BYTES_PER_PAGE[ \t]+.?[0-9]+"
  MATCHED_DEFINE_LINE ${FLASH_HEADER_CONTENTS}
)
string(REGEX MATCH "[0-9]+" FLASH_PAGE_SIZE ${MATCHED_DEFINE_LINE})

set_target_properties(MCUPort PROPERTIES flash_page_size ${FLASH_PAGE_SIZE})