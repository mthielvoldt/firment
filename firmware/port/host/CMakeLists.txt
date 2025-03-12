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
    ${PROJECT_CONFIG_DIR}
)

# TODO: (1) make this the SOT for page size in FW, configuring fmt_flash_port.h

# flash_page_size is used in:
# - web-ui for <Image> upload widget (via /web-ui/src/generated/flashPage.ts)
# - fmt_flash_port.h > fmt_update.c  
# It's normally parsed from a target specific header.
set_target_properties(MCUPort PROPERTIES flash_page_size 256)