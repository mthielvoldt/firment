## This file sets variables that are determined by MCU selection.

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -ffreestanding -fdata-sections -ffunction-sections")

# "-Wa," must precede each option passed to the assembler.
set(ASSEMBLER_FLAGS "-Wa,-mcpu=cortex-m4")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wmissing-declarations")
set(CMAKE_C_FLAGS_DEBUG "-Og -g")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(COMMON_LINK_OPTIONS "-Wl,-Map=${PROJECT_NAME}.map,--cref,--gc-sections")

if(${PCB} STREQUAL "0")
  set(MCU_FAMILY "XMC4")
  set(MCU_VARIANT "XMC4700")
  set(MCU_SPEED_VARIANT "F144")
  set(MCU_MEM_VARIANT "2048")
  set(LINKER_SCRIPT_TEMPLATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/XMC4700x2048.ld.template)
  add_compile_definitions(${MCU_VARIANT}_${MCU_SPEED_VARIANT}x${MCU_MEM_VARIANT})
elseif(${PCB} STREQUAL "1")
  set(MCU_FAMILY "stm32l4")
  set(MCU_VARIANT "stm32l476")
  set(LINKER_SCRIPT_TEMPLATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/STM32L476.ld.template)
  add_compile_definitions("STM32L476xx")
endif()

message(STATUS "PCB: ${PCB}")
message(STATUS "MCU: ${MCU_VARIANT}")
