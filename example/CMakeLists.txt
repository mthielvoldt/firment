cmake_minimum_required(VERSION 3.22.1)

# Read the version from an external file
file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_CONTENT)
# Strip any trailing whitespace
string(STRIP "${VERSION_CONTENT}" PROJECT_VERSION)
message(STATUS "Project version: ${PROJECT_VERSION}")

# Disables CXX (C++)
project(spi_example VERSION ${PROJECT_VERSION} LANGUAGES C ASM)



set(DEBUG_INFO_OPTIONS -g)
  # -g3
  # -gdwarf-2

set(OPTIMIZATION_OPTIONS -O0)

add_compile_options(
  ${DEBUG_INFO_OPTIONS}
  ${OPTIMIZATION_OPTIONS}
  -Wall
)

set(EXECUTABLE ${PROJECT_NAME}.elf)


# Add firment, set where (in the binary dir) to place firment output files.
add_subdirectory(.. firment)
add_subdirectory(../firmware/port/XMC4 XMC4700_F144) # provides `MCUPort` lib.


add_executable(${EXECUTABLE}
  firmware/spi_example.c
  firmware/project_comms.c
  firmware/control.c
  firmware/gpio_common.c
)

target_include_directories(${EXECUTABLE} PRIVATE
  firmware
)


target_link_libraries(${EXECUTABLE} FirmentFW MCUPort -lm)

target_link_options(${EXECUTABLE} PRIVATE
    "-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections"
)
## Uncomment to see full linker command.
# target_link_options(${EXECUTABLE} PRIVATE "-v")


# PUBLIC separates this from above.
target_include_directories(${EXECUTABLE} PUBLIC
  ${PROJECT_BINARY_DIR}/firment
  ../../firmware
  ../../firmware/port/XMC4/4800_F144
)


add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${EXECUTABLE})
add_custom_command(TARGET ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)