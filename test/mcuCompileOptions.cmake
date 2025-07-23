set(MCU_FAMILY "host")
set(PORT_DIR ${FIRMENT_DIR}/firmware/port/${MCU_FAMILY})

if(${PCB} STREQUAL "0")
  set(MCU_VARIANT "XMC4700")  # For partitions.cmake
  # set(STARTUP_FILE "${PORT_DIR}/mtb-xmclib-cat3/CMSIS/Infineon/COMPONENT_XMC4700/Source/TOOLCHAIN_GCC_ARM/startup_${MCU_VARIANT}.S")
  # set(LINKER_SCRIPT_TEMPLATE 
  #   ${CMAKE_CURRENT_SOURCE_DIR}/firmware/XMC4700x2048.ld.in)
  # add_compile_definitions("STM32L476xx")
endif()

message(STATUS "PCB: ${PCB}")
message(STATUS "MCU_FAMILY: ${MCU_FAMILY}")
message(STATUS "MCU: ${MCU_VARIANT}")