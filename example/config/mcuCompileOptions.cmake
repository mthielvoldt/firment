## This file sets variables that are determined by MCU selection.

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -ffreestanding -fdata-sections -ffunction-sections")

# "-Wa," must precede each option passed to the assembler.
set(ASSEMBLER_FLAGS "-Wa,-mcpu=cortex-m4")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wmissing-declarations")

set(MCU_FAMILY "XMC4")
set(MCU_VARIANT "XMC4700")
set(MCU_SPEED_VARIANT "F144")
set(MCU_MEM_VARIANT "2048")

# add_compile_definitions(XMC4400_F100x512)
add_compile_definitions(${MCU_VARIANT}_${MCU_SPEED_VARIANT}x${MCU_MEM_VARIANT})