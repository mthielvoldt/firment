## This Top-level CMakeLists.txt is accountable to: 
# TODO
# - Generate fmt_version.h with values from version.txt, and include it in build.
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

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(config/version.cmake) # -> VERSION_SEM

# Disables CXX (C++)
project(spi_example VERSION ${VERSION_SEM} LANGUAGES C ASM)

include(config/mcuCompileOptions.cmake)
include(config/partitions.cmake)
include(config/cryptOptions.cmake)
include(config/firmentConfig.cmake)

set(MGTBOOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mgt-boot)
set(LINKER_SCRIPT_TEMPLATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/firmware/XMC4700x2048.ld.template)
include(${MGTBOOT_DIR}/tools/genLinker.cmake) # req: LINKER_SCRIPT_TEMPLATE


# Select which MCU port library ('MCUPort' target) build project against.
set(PORT_DIR ${FIRMENT_DIR}/firmware/port/${MCU_FAMILY})
add_subdirectory(${PORT_DIR} ${MCU_FAMILY}) # provides MCUPort library.

# Add firment, and set where (in the binary dir) to place firment output files.
# Must come after add_subdirectory() for port or it won't find MCUPort target.
add_subdirectory(${FIRMENT_DIR} firment)

include(cmFlashLib.cmake) # provides flash_lib library.  Needs FirmentFW lib

# Update the build_time, which gets used as the app version for signing.
set(APP_DEPENDS app_lib FirmentFW MCUPort)
include(${MGTBOOT_DIR}/tools/genBuildTime.cmake)

# Specify the project's source files.
# TODO: make this so it re-builds when linker script changes. 
add_library(app_lib OBJECT
  firmware/spi_example.c
  firmware/project_comms.c
  firmware/control.c
  firmware/init_spi.c
  ${BUILD_TIME_HEADER}
)
target_include_directories(app_lib PRIVATE
  firmware
  ${APP_FW_CONFIG_DIR}
  ${PROJECT_BINARY_DIR} # build_time.h
)
# Specify which libraries the top-level target depends on directly. 
# Note: top-level needs MCUPort to select the peripheral modules to use.
# Note: -lm links the standard math library (math.h).
target_link_libraries(app_lib FirmentFW MCUPort -lm)


add_executable(app_root.elf $<TARGET_OBJECTS:app_lib>)
target_link_libraries(app_root.elf app_lib)
target_link_options(app_root.elf PRIVATE "-T${ROOT_PARTITION_LINKER_FILE}")
## Uncomment to see full linker command.
# target_link_options(app_root.elf PRIVATE "-v")

add_executable(app_offset.elf $<TARGET_OBJECTS:app_lib>)
target_link_libraries(app_offset.elf app_lib)
target_link_options(app_offset.elf PRIVATE "-T${APP_PARTITION_LINKER_FILE}")


# Invoke bootloader build
add_subdirectory(${MGTBOOT_DIR} mgt-boot)

# Example UI
set(EXAMPLE_UI_DIR ${PROJECT_SOURCE_DIR}/web-ui)

add_custom_command(OUTPUT ${EXAMPLE_UI_DIR}/node_modules
  DEPENDS
    ${EXAMPLE_UI_DIR}/package.json
    FirmentUI
  WORKING_DIRECTORY ${EXAMPLE_UI_DIR}
  COMMAND npm install --no-audit
)

add_custom_command(OUTPUT ${EXAMPLE_UI_DIR}/dist/index.html
  DEPENDS
    ${EXAMPLE_UI_DIR}/node_modules
    ${EXAMPLE_UI_DIR}/src/App.tsx
    ${EXAMPLE_UI_DIR}/src/index.css
    ${EXAMPLE_UI_DIR}/src/main.tsx
    ${EXAMPLE_UI_DIR}/src/profiler.ts
    ${EXAMPLE_UI_DIR}/src/vite-env.d.ts
    ${EXAMPLE_UI_DIR}/tsconfig.app.json
    ${EXAMPLE_UI_DIR}/tsconfig.node.json
    ${EXAMPLE_UI_DIR}/vite.config.ts
  WORKING_DIRECTORY ${EXAMPLE_UI_DIR}
  COMMAND npm run build
)

add_custom_target(example_ui ALL DEPENDS ${EXAMPLE_UI_DIR}/dist/index.html)