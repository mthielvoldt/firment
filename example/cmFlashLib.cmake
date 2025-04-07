add_library(flash_lib 
  firmware/flash.c
)

# This line arguably belongs in mgt-boot/CML
target_include_directories(flash_lib PUBLIC 
  ${MGTBOOT_DIR}/inc  # flash.h
  ${PROJECT_BINARY_DIR}/mgt-boot  # partitions.h (configured)
)

target_link_libraries(flash_lib FirmentFW)