
### This File requires:
# - CMSIS_FAMILY_DIR to be set

set(FAMILY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../${MCU_FAMILY}")

# Allow project to define HAL_DIR if it already has this.
if(NOT DEFINED HAL_DIR)
  set(HAL_DIR "${FAMILY_DIR}/${MCU_FAMILY}xx-hal-driver")
endif()

set(HAL_SRC_PREFIX "${HAL_DIR}/Src/${MCU_FAMILY}xx_hal")
set(CMSIS_DRIVER_DIR "${FAMILY_DIR}/../stm32/CMSIS-Driver_STM32/Drivers")
set(STARTUP_FILE "${CMSIS_FAMILY_DIR}/Source/Templates/gcc/startup_${MCU_VARIANT}xx.s" PARENT_SCOPE)
set(SYSTEM_FILE  "${CMSIS_FAMILY_DIR}/Source/Templates/system_${MCU_FAMILY}xx.c")

# Note: In the STM32 ecosystem, there is no equivalent of RTE_Device.h in XMC
# ecosystem, which combines MCU-wiring information for several peripherals. 
# The stm32<fam>xx_hal_<feat>_ex.h files provide MCU wiring constraints.  
# example: _gpio_ex.h provides wiring for pin alternate functions.

add_library(MCUPort
  ${SYSTEM_FILE}
  ${HAL_SRC_PREFIX}.c
  ${HAL_SRC_PREFIX}_cortex.c
  ${HAL_SRC_PREFIX}_crc.c
  ${HAL_SRC_PREFIX}_crc_ex.c
  ${HAL_SRC_PREFIX}_dma.c
  ${HAL_SRC_PREFIX}_exti.c
  ${HAL_SRC_PREFIX}_flash.c
  ${HAL_SRC_PREFIX}_flash_ex.c
  ${HAL_SRC_PREFIX}_flash_ramfunc.c
  ${HAL_SRC_PREFIX}_gpio.c
  ${HAL_SRC_PREFIX}_pwr_ex.c
  ${HAL_SRC_PREFIX}_rcc.c
  ${HAL_SRC_PREFIX}_spi.c
  ${HAL_SRC_PREFIX}_spi_ex.c
  ${HAL_SRC_PREFIX}_tim.c
  ${HAL_SRC_PREFIX}_tim_ex.c
  ${HAL_SRC_PREFIX}_uart.c
  ${HAL_SRC_PREFIX}_uart_ex.c
  ../common/flash_common.c
  ${FAMILY_DIR}/fmt_crc_port.c
  ${FAMILY_DIR}/fmt_flash_port.c
  ${FAMILY_DIR}/fmt_periodic_port.c
  ${FAMILY_DIR}/fmt_sysInit_port.c
  ${FAMILY_DIR}/gpio_port.c
)

if(FMT_TRANSPORT STREQUAL "SPI")
  target_sources(MCUPort PRIVATE
    ${CMSIS_DRIVER_DIR}/SPI_STM32.c
    ${FAMILY_DIR}/fmt_spi_port.c
    ${FAMILY_DIR}/ioc_port.c
  )
elseif(FMT_TRANSPORT STREQUAL "UART")
target_sources(MCUPort PRIVATE
    ${CMSIS_DRIVER_DIR}/USART_STM32.c
    ${FAMILY_DIR}/fmt_uart_port.c
  )
endif()

target_include_directories(MCUPort
  PUBLIC
    ${FAMILY_DIR}
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
