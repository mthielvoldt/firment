cmake_minimum_required(VERSION 3.22.1)

# Disables CXX (C++)
project(spi_example VERSION 0.1 LANGUAGES C ASM)

# set(DEBUG_INFO_OPTIONS
#   -g3
#   -gdwarf-2
# )
# set(OPTIMIZATION_OPTIONS
#   -o0
# )
# add_compile_options(
#   ${DEBUG_INFO_OPTIONS}
#   ${OPTIMIZATION_OPTIONS}
#   -Wall
# )

# target_sources()
set_property(SOURCE ${XMC_STARTUP_FILE} PROPERTY LANGUAGE ASM)
# set_property(SOURCE
#   lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/src/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/xmc_common_gcc.S
#   PROPERTY LANGUAGE ASM
# )

add_link_options(-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections)
if(DEFINED XMC_LINKER_FILE)
  add_link_options(-T${XMC_LINKER_FILE})
endif()

set(EXECUTABLE ${PROJECT_NAME}.elf)
set(XMC_LIB "lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/src")
  
# link_libraries("-lc -lm -lnosys")

add_executable(${EXECUTABLE}
  spi_example.c
  ${XMC_SYSTEM_FILE}
  ${XMC_STARTUP_FILE}
  ${XMC_LIB}/xmc4_gpio.c
  ${XMC_LIB}/xmc_spi.c
  ${XMC_LIB}/xmc_usic.c
  ${XMC_LIB}/xmc4_scu.c
)

target_include_directories(${EXECUTABLE} PRIVATE
  lib/XMC-for-Arduino/libraries/SPI/src
  lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/inc
  lib/XMC-for-Arduino/cores/xmc_lib/CMSIS/Include
  lib/XMC-for-Arduino/cores
  lib/XMC-for-Arduino/variants/${XMC_VARIANT}
  lib/XMC-for-Arduino/variants/${XMC_VARIANT}/config/XMC4700_Relax_Kit
)


add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)