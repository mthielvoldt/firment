add_library(mcu_port_lib
  firmware/mgtBoot_adapter.c
)

# This line arguably belongs in mgt-boot/CML
target_include_directories(mcu_port_lib PUBLIC 
  ${MGTBOOT_DIR}/inc  # flash_port.h, clock_port.h
  ${PROJECT_BINARY_DIR}/mgt-boot  # partitions.h (configured)
)

target_link_libraries(mcu_port_lib FirmentFW)