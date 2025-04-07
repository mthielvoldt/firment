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
string(STRIP "${VERSION_CONTENT}" APP_VERSION)
message(STATUS "App version: ${APP_VERSION}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Disables CXX (C++)
project(spi_example VERSION ${APP_VERSION} LANGUAGES C ASM)

include(config/mcuCompileOptions.cmake)
include(config/partitions.cmake)
include(config/cryptOptions.cmake)
include(config/firmentConfig.cmake)

set(MGTBOOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mgt-boot)
set(LINKER_SCRIPT_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/firmware/XMC4700x2048.ld.template)
include(${MGTBOOT_DIR}/tools/genLinker.cmake) # req: LINKER_SCRIPT_TEMPLATE


set(PORT_DIR ${FIRMENT_DIR}/firmware/port/${MCU_FAMILY})

# Project-wide compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


# Select which MCU port library ('MCUPort' target) build project against.
# Note: MCU_FAMILY specified in the toolchain file. TODO: move into fmt. CML.
add_subdirectory(${PORT_DIR} ${MCU_FAMILY})

# Add firment, and set where (in the binary dir) to place firment output files.
# Must come after add_subdirectory() for port or it won't find MCUPort target.
add_subdirectory(${FIRMENT_DIR} firment)

# Specify the project's source files.
# TODO: make this so it re-builds when linker script changes. 
add_executable(app_root.elf
  firmware/spi_example.c
  firmware/project_comms.c
  firmware/control.c
  firmware/init_spi.c
)

# Specify the project's include directories. Libraries add their own. 
target_include_directories(app_root.elf PRIVATE
  firmware
  ${PROJECT_CONFIG_DIR}
)

# Specify which libraries the top-level target depends on directly. 
# Note: top-level needs MCUPort to select the peripheral modules to use.
# Note: -lm links the standard math library (math.h).
target_link_libraries(app_root.elf FirmentFW MCUPort -lm)


set(LINKER_FILE ${PORT_DIR}/${MCU_VARIANT}x${MCU_MEM_VARIANT}.ld)
target_link_options(app_root.elf PRIVATE
    "-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections,-T${LINKER_FILE}"
)
## Uncomment to see full linker command.
# target_link_options(app_root.elf PRIVATE "-v")

# Build a hex file from the elf.
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS app_root.elf)
add_custom_command(TARGET ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O ihex app_root.elf ${PROJECT_NAME}.hex)

# Build a binary.
add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS app_root.elf)
add_custom_command(TARGET ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary app_root.elf ${PROJECT_NAME}.bin)

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
