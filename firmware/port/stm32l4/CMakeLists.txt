
set(HAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stm32l4xx-hal-driver")
set(CMSIS_FAMILY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmsis_device_l4")
set(CMSIS_DRIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-Driver_STM32/Drivers")

set(SYSTEM_FILE  "${CMSIS_FAMILY_DIR}/Source/Templates/system_${MCU_FAMILY}xx.c")

# Note: In the STM32 ecosystem, there is no equivalent of RTE_Device.h in XMC
# ecosystem, which combines MCU-wiring information for several peripherals. 
# The stm32<fam>xx_hal_<feat>_ex.h files provide MCU wiring constraints.  
# example: stm32l4xx_hal_gpio_ex.h provides wiring for pin alternate functions.

add_library(MCUPort
  ${SYSTEM_FILE}
  CMSIS-Driver_STM32/Drivers/SPI_STM32.c
  ${HAL_DIR}/Src/stm32l4xx_hal.c
  ${HAL_DIR}/Src/stm32l4xx_hal_cortex.c
  ${HAL_DIR}/Src/stm32l4xx_hal_dma.c
  ${HAL_DIR}/Src/stm32l4xx_hal_exti.c
  ${HAL_DIR}/Src/stm32l4xx_hal_flash.c
  ${HAL_DIR}/Src/stm32l4xx_hal_flash_ex.c
  ${HAL_DIR}/Src/stm32l4xx_hal_flash_ramfunc.c
  ${HAL_DIR}/Src/stm32l4xx_hal_gpio.c
  ${HAL_DIR}/Src/stm32l4xx_hal_pwr_ex.c
  ${HAL_DIR}/Src/stm32l4xx_hal_rcc.c
  ${HAL_DIR}/Src/stm32l4xx_hal_spi.c
  ${HAL_DIR}/Src/stm32l4xx_hal_spi_ex.c
  ${HAL_DIR}/Src/stm32l4xx_hal_tim.c
  ${HAL_DIR}/Src/stm32l4xx_hal_tim_ex.c
  ../common/flash_common.c
  fmt_crc_port.c
  fmt_flash_port.c
  fmt_periodic_port.c
  fmt_spi_port.c
  fmt_sysInit_port.c
  gpio_port.c
  ioc_port.c
)

target_include_directories(MCUPort
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../arm_generic
  PRIVATE
    ../..   # fmt headers.
    ${CMSIS_DRIVER_DIR}
    ../../lib/CMSIS-6.1.0/Driver/Include
    ${APP_FW_CONFIG_DIR}
    ${APP_FW_CONFIG_DIR}/pcb${PCB}  # <feat>pcbDetails.h
    PUBLIC
    ../../lib/CMSIS-6.1.0/Core/Include # core_cm4.h
    ${HAL_DIR}/Inc  # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h
    ${CMSIS_FAMILY_DIR}/Include # fmt_rx.pb.c > fmt_spi.h > RTE_Device.h > xmc_device.h > XMC4700.h
)
