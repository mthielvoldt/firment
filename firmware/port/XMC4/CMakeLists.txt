
set(XMC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/XMCLib")
set(XMC_SERIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mtb-xmclib-cat3/CMSIS/Infineon/COMPONENT_XMC4700")
set(STARTUP_FILE "${XMC_SERIES_DIR}/Source/TOOLCHAIN_GCC_ARM/startup_${MCU_VARIANT}.S")
set(LINKER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/${MCU_VARIANT}x${MCU_MEM_VARIANT}.ld")
set(SYSTEM_FILE  "${XMC_SERIES_DIR}/Source/system_${MCU_VARIANT}.c")
if(MCU_VARIANT STREQUAL "XMC4700" OR MCU_VARIANT STREQUAL "XMC4800")
  set(RTE_DEVICE_DIR "4800_F144")
endif()

add_library(MCUPort
  ${SYSTEM_FILE}
  ${STARTUP_FILE}
  XMC4000_DFP/RTE_Driver/SPI.c
  ${XMC_LIB}/src/xmc4_gpio.c
  ${XMC_LIB}/src/xmc_spi.c
  ${XMC_LIB}/src/xmc_usic.c
  ${XMC_LIB}/src/xmc_ccu4.c
  ${XMC_LIB}/src/xmc4_scu.c
  ${XMC_LIB}/src/xmc_fce.c
  ${XMC_LIB}/src/xmc_eru.c
  ${XMC_LIB}/src/xmc4_eru.c
  fmt_crc_xmc4.c
  fmt_gpio_xmc.c
  fmt_ioc_xmc4.c
  fmt_periodic_xmc.c
)

target_include_directories(MCUPort
  PRIVATE
    .       # fmt_periodic_xmc.h
    ../..   # fmt headers.
    XMC4000_DFP/RTE_Driver
    XMC4000_DFP/CMSIS/RTOS/RTX/INC
    ../../lib/CMSIS-6.1.0/Driver/Include
    ${PROJECT_SOURCE_DIR}/firmware  # TODO: make this not assume project dir structure.
    PUBLIC
    ../../lib/CMSIS-6.1.0/Core/Include # core_cm4.h
    ${RTE_DEVICE_DIR}
    ${XMC_LIB}/inc  # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h
    ${XMC_SERIES_DIR}/Include # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h > XMC4700.h
)

# Tells the assembler what architecture the toolchain file specifies. 
target_compile_options(MCUPort PRIVATE ${ASSEMBLER_FLAGS})

# This gets propagated up to the project (consuming targets).
target_link_options(MCUPort INTERFACE "-T${LINKER_FILE}")