## This Top-level CMakeLists.txt is accountable to: 
# TODO
# - Generate version.h with values from version.txt, and include it in build.
# - Generate firment.proto with values from firment.config. 
# - Link project to a local copy of the firment library.
# - Link project to the chosen firment MCU lib.
# - Link project to the needed standard libs with linker options
# - Provide project-wide compiler flags appropriate to project requirements.
# - Provide linker flags appropriate to both test and MCU architectures. 
# - Specify the project source files. 
# - Specify the project include search paths. 
# Note: The toolchain file (selected when cmake is invoked) specifies the MCU.

cmake_minimum_required(VERSION 3.22.1)

# Read the version from an external file
file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_CONTENT)
# Strip any trailing whitespace
string(STRIP "${VERSION_CONTENT}" PROJECT_VERSION)
message(STATUS "Project version: ${PROJECT_VERSION}")

# Disables CXX (C++)
project(spi_example VERSION ${PROJECT_VERSION} LANGUAGES C ASM)

## ****** This is the only line firment needs you to edit *****
set(FIRMENT_DIR ..)

# Project-wide compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")  # debug symbols
  # -g3
  # -gdwarf-2
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0") # Optimizations off
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") # Warnings: all
message(STATUS "C flags: ${CMAKE_C_FLAGS}")

set(EXECUTABLE ${PROJECT_NAME}.elf)


# Add firment, and set where (in the binary dir) to place firment output files.
add_subdirectory(${FIRMENT_DIR} firment)

# Select which MCU port library ('MCUPort' target) build project against.
# Note: MCU_FAMILY specified in the toolchain file. 
add_subdirectory(${FIRMENT_DIR}/firmware/port/${MCU_FAMILY} ${MCU_FAMILY})

# Specify the project's source files.
# TODO: make this so it re-builds when linker script changes. 
add_executable(${EXECUTABLE}
  firmware/spi_example.c
  firmware/project_comms.c
  firmware/control.c
  firmware/gpio_common.c
)

# Specify the project's include directories. Libraries add their own. 
target_include_directories(${EXECUTABLE} PRIVATE
  firmware
)

# Specify which libraries the top-level target depends on directly. 
# Note: top-level needs MCUPort to select the peripheral modules to use.
# Note: -lm links the standard math library (math.h).
target_link_libraries(${EXECUTABLE} FirmentFW MCUPort -lm)

target_link_options(${EXECUTABLE} PRIVATE
    "-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections"
)
## Uncomment to see full linker command.
# target_link_options(${EXECUTABLE} PRIVATE "-v")

# Build a hex file from the elf.
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex)

add_custom_target(web-ui ALL
  DEPENDS
    ${PROJECT_SOURCE_DIR}/web-ui/node_modules/firment-ui
    FirmentUI
)

# No DEPENDS - if firment-ui exists in web-ui/node_modules/, it's up-to-date. 
add_custom_command(
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/web-ui
  OUTPUT ${PROJECT_SOURCE_DIR}/web-ui/node_modules/firment-ui
  COMMAND npm install ${FIRMENT_DIR}/web-ui 
)

# # Uncomment if a binary file is also required.
# add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${EXECUTABLE})
# add_custom_command(TARGET ${PROJECT_NAME}.bin
#   COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)