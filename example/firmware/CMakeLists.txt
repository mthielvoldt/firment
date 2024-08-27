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

add_link_options(-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections,-T${XMC_LINKER_FILE})

set(EXECUTABLE ${PROJECT_NAME}.elf)
  
# link_libraries("-lc -lm -lnosys")

add_executable(${EXECUTABLE}
  spi_example.c
  ${XMC_SYSTEM_FILES}.c
  ${XMC_STARTUP_FILE}
)

target_include_directories(${EXECUTABLE} PRIVATE
  lib/XMC-for-Arduino/libraries/SPI/src
  lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/inc
  lib/XMC-for-Arduino/cores/xmc_lib/CMSIS/Include
  lib/XMC-for-Arduino/cores
  lib/XMC-for-Arduino/variants/XMC4700
)

