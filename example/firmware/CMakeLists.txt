cmake_minimum_required(VERSION 3.22.1)

set(APP_NAME "spi_example")
project(${APP_NAME} LANGUAGES C )

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)



set(DEBUG_INFO_OPTIONS
  -g3
  -gdwarf-2
)
set(OPTIMIZATION_OPTIONS
  -o0
)
add_compile_options(
  ${DEBUG_INFO_OPTIONS}
  ${OPTIMIZATION_OPTIONS}
  -Wall
)


include_directories(
  "lib/XMC-for-Arduino/libraries/SPI/src"
  "lib/XMC-for-Arduino/cores/xmc_lib/XMCLib/inc"
  "lib/XMC-for-Arduino/cores/"
  "lib/XMC-for-Arduino/variants/XMC4700"
)

# target_sources()
set_property(SOURCE lib/XMC-for-Arduino/variants/XMC4700/startup_XMC4700.S PROPERTY LANGUAGE C)

add_link_options(
  ${MCU_OPTIONS}
  -Tlib/XMC-for-Arduino/variants/XMC4700/linker_script.ld
  -Wl,-Map=${PROJECT_NAME}.map,--cref
  -Wl,--gc-sections
  )
  
  link_libraries("-lc -lm -lnosys")
  
  add_executable(${APP_NAME}
    ${APP_NAME}.c
  )

