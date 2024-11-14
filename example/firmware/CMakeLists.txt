cmake_minimum_required(VERSION 3.22.1)

# Disables CXX (C++)
project(spi_example VERSION 0.1 LANGUAGES C ASM)

set(DEBUG_INFO_OPTIONS -g)
  # -g3
  # -gdwarf-2

set(OPTIMIZATION_OPTIONS -O0)

add_compile_options(
  ${DEBUG_INFO_OPTIONS}
  ${OPTIMIZATION_OPTIONS}
  -Wall
)

# target_sources()
# set_property(SOURCE ${XMC_STARTUP_FILE} PROPERTY LANGUAGE ASM)
# set_property(SOURCE
#   lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/src/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/xmc_common_gcc.S
#   PROPERTY LANGUAGE ASM
# )

add_link_options(-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections)
if(DEFINED XMC_LINKER_FILE)
  add_link_options(-T${XMC_LINKER_FILE})
endif()

set(EXECUTABLE ${PROJECT_NAME}.elf)
set(XMC_LIB "lib/XMC4000_DFP/Device/XMClib/src")
set(NANOPB "../../protocol/nanopb")

link_libraries("-lm")
# link_libraries("-lc -lm -lnosys")

add_executable(${EXECUTABLE}
  spi_example.c
  project_comms.c
  control.c
  generated/mcu_1.pb.c
  generated/fmt_rx.pb.c
  gpio_common.c
  ../../firmware/fmt_spi.c
  ../../firmware/queue.c
  ../../firmware/port/XMC4/fmt_periodic_xmc.c
  ../../firmware/port/XMC4/fmt_crc_xmc4.c
  ../../firmware/port/XMC4/fmt_ioc_xmc4.c
  ../../firmware/port/XMC4/fmt_gpio_xmc.c
  ../../firmware/fmt_hardfault.c
  ../../firmware/fmt_waveform.c
  ../../firmware/fmt_log.c
  ${XMC_SYSTEM_FILE}
  ${XMC_STARTUP_FILE}
  lib/XMC4000_DFP/RTE_Driver/SPI.c
  ${XMC_LIB}/xmc4_gpio.c
  ${XMC_LIB}/xmc_spi.c
  ${XMC_LIB}/xmc_usic.c
  ${XMC_LIB}/xmc_ccu4.c
  ${XMC_LIB}/xmc4_scu.c
  ${XMC_LIB}/xmc_fce.c
  ${XMC_LIB}/xmc_eru.c
  ${XMC_LIB}/xmc4_eru.c
  ${NANOPB}/pb_encode.c
  ${NANOPB}/pb_decode.c
  ${NANOPB}/pb_common.c
)

target_include_directories(${EXECUTABLE} PRIVATE
  lib/XMC4000_DFP/Device/XMClib/inc
  lib/XMC4000_DFP/Device/XMC4700_series/Include
  lib/XMC4000_DFP/RTE_Driver
  lib/XMC4000_DFP/CMSIS/RTOS/RTX/INC
  lib/CMSIS-6.1.0/Core/Include
  lib/CMSIS-6.1.0/Driver/Include
  ${NANOPB}
  ../../firmware
  ../../firmware/port/XMC4/4800_F144
  .
)


add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)