
set(XMC_LIB "XMC4000_DFP/Device/XMClib")
set(XMC_DIR "XMC4000_DFP/Device/${MCU_VARIANT}_series")
set(XMC_STARTUP_FILE "${XMC_DIR}/Source/GCC/startup_${MCU_VARIANT}.S")
# set(XMC_LINKER_FILE  "${XMC_DIR}/Source/GCC/${MCU_VARIANT}x${XMC_MEM_VARIANT}.ld")
set(XMC_SYSTEM_FILE  "${XMC_DIR}/Source/system_${MCU_VARIANT}.c")
if(MCU_VARIANT STREQUAL "XMC4700")
  set(RTE_DEVICE_DIR 4800_F144)
  message("RTE_Device.h located at ${RTE_DEVICE_DIR}")
endif()

add_library(MCUPort
  ${XMC_SYSTEM_FILE}
  ${XMC_STARTUP_FILE}
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
    ${XMC_DIR}/Include # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h > XMC4700.h
)