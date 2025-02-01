
set(XMC_LIB "XMC4000_DFP/Device/XMClib/src")

add_library(MCU_LIB
  XMC4000_DFP/RTE_Driver/SPI.c
  ${XMC_LIB}/xmc4_gpio.c
  ${XMC_LIB}/xmc_spi.c
  ${XMC_LIB}/xmc_usic.c
  ${XMC_LIB}/xmc_ccu4.c
  ${XMC_LIB}/xmc4_scu.c
  ${XMC_LIB}/xmc_fce.c
  ${XMC_LIB}/xmc_eru.c
  ${XMC_LIB}/xmc4_eru.c
  fmt_crc_xmc4.c
  fmt_gpio_xmc.c
  fmt_ioc_xmc4.c
  fmt_periodic_xmc.c
)