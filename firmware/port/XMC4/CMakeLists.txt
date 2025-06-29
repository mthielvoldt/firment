
set(XMC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/XMCLib")
set(CMSIS_DEVICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/CMSIS/Infineon/COMPONENT_${MCU_VARIANT}")
set(CMSIS_DRIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/XMC4000_DFP/RTE_Driver")

set(SYSTEM_FILE  "${CMSIS_DEVICE_DIR}/Source/system_${MCU_VARIANT}.c")
if(MCU_VARIANT STREQUAL "XMC4700" OR MCU_VARIANT STREQUAL "XMC4800")
set(RTE_DEVICE_DIR "4800_F144")
endif()

add_library(MCUPort
  ${SYSTEM_FILE}
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
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../arm_generic # crit_section.h
    ${XMC_LIB}/inc  # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h
    ${CMSIS_DEVICE_DIR}/Include # xmc_device.h > XMC4700.h
    ../../lib/CMSIS-6.1.0/Core/Include # core_cm4.h
    ${RTE_DEVICE_DIR} # ioc_mcuDetails.h
  PRIVATE
    ../..   # fmt headers.
    ${CMSIS_DRIVER_DIR}
    ../../lib/CMSIS-6.1.0/Driver/Include
    ${APP_FW_CONFIG_DIR}
    ${APP_FW_CONFIG_DIR}/pcb${PCB}  # <feat>pcbDetails.h
)
