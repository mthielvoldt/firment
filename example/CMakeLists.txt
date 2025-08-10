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

set(PROJECT_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/config)
include(config/version.cmake) # -> VERSION_SEM

# Disables CXX (C++)
project(spi_example VERSION ${VERSION_SEM} LANGUAGES C ASM)

include(config/cryptOptions.cmake)      # no deps
include(config/firmentConfig.cmake)     # no deps
include(config/mcuCompileOptions.cmake) # after firmentConfig
include(config/partitions.cmake)        # after mcuCompileOptions

set(MGTBOOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mgt-boot)

include(${MGTBOOT_DIR}/tools/genLinker.cmake) # req: LINKER_SCRIPT_TEMPLATE


# Select which MCU port library ('MCUPort' target) build project against.

add_subdirectory(${PORT_DIR} ${MCU_FAMILY}) # provides MCUPort library.

# Add firment, and set where (in the binary dir) to place firment output files.
# Must come after add_subdirectory() for port or it won't find MCUPort target.
add_subdirectory(${FIRMENT_DIR} firment)

include(bootloaderMcuPort.cmake) # provides mcu_port_lib required by mgt_boot.

add_library(app_lib 
${STARTUP_FILE}
firmware/spi_example.c
firmware/project_comms.c
firmware/control.c
)
target_include_directories(app_lib 
  PRIVATE 
    firmware 
    ${APP_FW_CONFIG_DIR} 
    ${APP_FW_CONFIG_DIR}/pcb${PCB}  # <feat>pcbDetails.h
  PUBLIC 
    ${MGTBOOT_DIR}/inc # build_time.h
)
# Specify which libraries the top-level target depends on directly. 
# Note: -lm links the standard math library (math.h).
target_link_libraries(app_lib FirmentFW MCUPort -lm)

# Some startup files in assembly don't specify the architecture; this does.
target_compile_options(app_lib PRIVATE ${ASSEMBLER_FLAGS})


# Update the build_time, which gets used as the app version for signing.
set(APP_DEPENDS 
  $<TARGET_FILE:app_lib> 
  $<TARGET_FILE:FirmentFW> 
  $<TARGET_FILE:MCUPort>
)
include(${MGTBOOT_DIR}/tools/genBuildTime.cmake)

add_executable(app_root.elf ${BUILD_TIME_C})
target_link_libraries(app_root.elf app_lib)
target_link_options(app_root.elf PRIVATE "-T${ROOT_PARTITION_LINKER_FILE}")
## Uncomment to see full linker command.
# target_link_options(app_root.elf PRIVATE "-v")

# Invoke bootloader build
if (NOT DEFINED UPDATE_SUPPORTED OR UPDATE_SUPPORTED)
  add_executable(app_offset.elf ${BUILD_TIME_C})
  target_link_libraries(app_offset.elf app_lib)
  target_link_options(app_offset.elf PRIVATE "-T${APP_PARTITION_LINKER_FILE}")

  add_custom_target(app_elfs DEPENDS app_root.elf app_offset.elf)

  add_subdirectory(${MGTBOOT_DIR} mgt-boot)
endif()

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