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

add_link_options(-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections)
if(DEFINED XMC_LINKER_FILE)
  add_link_options(-T${XMC_LINKER_FILE})
endif()

set(EXECUTABLE ${PROJECT_NAME}.elf)


# Add firment, set where (in the binary dir) to place firment output files.
add_subdirectory(.. firment)
add_subdirectory(../firmware/port/XMC4 XMC4)
set(CPU_VARIANT "4800_F144")

link_libraries("-lm")
# link_libraries("-lc -lm -lnosys")

add_executable(${EXECUTABLE}
  firmware/spi_example.c
  firmware/project_comms.c
  firmware/control.c
  firmware/gpio_common.c
  firmware/${XMC_SYSTEM_FILE}
  firmware/${XMC_STARTUP_FILE}

)

target_link_libraries(${EXECUTABLE} FirmentFW)

target_include_directories(${EXECUTABLE} PRIVATE
  lib/XMC4000_DFP/Device/XMClib/inc
  lib/XMC4000_DFP/Device/XMC4700_series/Include
  lib/XMC4000_DFP/RTE_Driver
  lib/XMC4000_DFP/CMSIS/RTOS/RTX/INC
  lib/CMSIS-6.1.0/Core/Include  # TODO: Remove.  Relocated CMSIS to FMT. 
  .
  PUBLIC
    
)

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