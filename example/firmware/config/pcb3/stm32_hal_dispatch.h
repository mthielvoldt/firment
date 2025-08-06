#ifndef stm32_hal_dispatch_H
#define stm32_hal_dispatch_H


#ifdef HAL_FAMILY_ENABLED
#include <stm32g4xx.h>
#endif

#ifdef HAL_BASE_ENABLED
#include <stm32g4xx_hal.h>
#endif

#ifdef HAL_RCC_ENABLED
#include "stm32g4xx_hal_rcc.h"
#endif /* HAL_RCC_ENABLED */

#ifdef HAL_GPIO_ENABLED
#include "stm32g4xx_hal_gpio.h"
#endif /* HAL_GPIO_ENABLED */

#ifdef HAL_DMA_ENABLED
#include "stm32g4xx_hal_dma.h"
#endif /* HAL_DMA_ENABLED */

#ifdef HAL_CORTEX_ENABLED
#include "stm32g4xx_hal_cortex.h"
#endif /* HAL_CORTEX_ENABLED */

#ifdef HAL_ADC_ENABLED
#include "stm32g4xx_hal_adc.h"
#endif /* HAL_ADC_ENABLED */

#ifdef HAL_COMP_ENABLED
#include "stm32g4xx_hal_comp.h"
#endif /* HAL_COMP_ENABLED */

#ifdef HAL_CORDIC_ENABLED
#include "stm32g4xx_hal_cordic.h"
#endif /* HAL_CORDIC_ENABLED */

#ifdef HAL_CRC_ENABLED
#include "stm32g4xx_hal_crc.h"
#endif /* HAL_CRC_ENABLED */

#ifdef HAL_CRYP_ENABLED
#include "stm32g4xx_hal_cryp.h"
#endif /* HAL_CRYP_ENABLED */

#ifdef HAL_DAC_ENABLED
#include "stm32g4xx_hal_dac.h"
#endif /* HAL_DAC_ENABLED */

#ifdef HAL_EXTI_ENABLED
#include "stm32g4xx_hal_exti.h"
#endif /* HAL_EXTI_ENABLED */

#ifdef HAL_FDCAN_ENABLED
#include "stm32g4xx_hal_fdcan.h"
#endif /* HAL_FDCAN_ENABLED */

#ifdef HAL_FLASH_ENABLED
#include "stm32g4xx_hal_flash.h"
#endif /* HAL_FLASH_ENABLED */

#ifdef HAL_FMAC_ENABLED
#include "stm32g4xx_hal_fmac.h"
#endif /* HAL_FMAC_ENABLED */

#ifdef HAL_HRTIM_ENABLED
#include "stm32g4xx_hal_hrtim.h"
#endif /* HAL_HRTIM_ENABLED */

#ifdef HAL_IRDA_ENABLED
#include "stm32g4xx_hal_irda.h"
#endif /* HAL_IRDA_ENABLED */

#ifdef HAL_IWDG_ENABLED
#include "stm32g4xx_hal_iwdg.h"
#endif /* HAL_IWDG_ENABLED */

#ifdef HAL_I2C_ENABLED
#include "stm32g4xx_hal_i2c.h"
#endif /* HAL_I2C_ENABLED */

#ifdef HAL_I2S_ENABLED
#include "stm32g4xx_hal_i2s.h"
#endif /* HAL_I2S_ENABLED */

#ifdef HAL_LPTIM_ENABLED
#include "stm32g4xx_hal_lptim.h"
#endif /* HAL_LPTIM_ENABLED */

#ifdef HAL_NAND_ENABLED
#include "stm32g4xx_hal_nand.h"
#endif /* HAL_NAND_ENABLED */

#ifdef HAL_NOR_ENABLED
#include "stm32g4xx_hal_nor.h"
#endif /* HAL_NOR_ENABLED */

#ifdef HAL_OPAMP_ENABLED
#include "stm32g4xx_hal_opamp.h"
#endif /* HAL_OPAMP_ENABLED */

#ifdef HAL_PCD_ENABLED
#include "stm32g4xx_hal_pcd.h"
#endif /* HAL_PCD_ENABLED */

#ifdef HAL_PWR_ENABLED
#include "stm32g4xx_hal_pwr.h"
#endif /* HAL_PWR_ENABLED */

#ifdef HAL_QSPI_ENABLED
#include "stm32g4xx_hal_qspi.h"
#endif /* HAL_QSPI_ENABLED */

#ifdef HAL_RNG_ENABLED
#include "stm32g4xx_hal_rng.h"
#endif /* HAL_RNG_ENABLED */

#ifdef HAL_RTC_ENABLED
#include "stm32g4xx_hal_rtc.h"
#endif /* HAL_RTC_ENABLED */

#ifdef HAL_SAI_ENABLED
#include "stm32g4xx_hal_sai.h"
#endif /* HAL_SAI_ENABLED */

#ifdef HAL_SMARTCARD_ENABLED
#include "stm32g4xx_hal_smartcard.h"
#endif /* HAL_SMARTCARD_ENABLED */

#ifdef HAL_SMBUS_ENABLED
#include "stm32g4xx_hal_smbus.h"
#endif /* HAL_SMBUS_ENABLED */

#ifdef HAL_SPI_ENABLED
#include "stm32g4xx_hal_spi.h"
#endif /* HAL_SPI_ENABLED */

#ifdef HAL_SRAM_ENABLED
#include "stm32g4xx_hal_sram.h"
#endif /* HAL_SRAM_ENABLED */

#ifdef HAL_TIM_ENABLED
#include "stm32g4xx_hal_tim.h"
#endif /* HAL_TIM_ENABLED */

#ifdef HAL_UART_ENABLED
#include "stm32g4xx_hal_uart.h"
#endif /* HAL_UART_ENABLED */

#ifdef HAL_USART_ENABLED
#include "stm32g4xx_hal_usart.h"
#endif /* HAL_USART_ENABLED */

#ifdef HAL_WWDG_ENABLED
#include "stm32g4xx_hal_wwdg.h"
#endif /* HAL_WWDG_ENABLED */

#endif