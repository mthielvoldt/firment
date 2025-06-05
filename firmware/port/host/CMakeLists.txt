add_library(MCUPort
  fmt_flash_mock.c
  gpio_spy.c
  ioc_spy.c
  fmt_spi_host.c
  fmt_crc_host.c
  spi_test.c
)

target_include_directories(MCUPort
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ../..
    ../../lib/CMSIS-6.1.0/Driver/Include # Driver_Common.h
    ${APP_FW_CONFIG_DIR}
    ${APP_FW_CONFIG_DIR}/pcb${PCB}  # <feat>pcbDetails.h
)
