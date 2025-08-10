/** timer_mcuDetails.h
 * Provides mcu headers and a type that encompases the details needed for each
 * resource for use by timer_pcbDetails.h
 * 
 * Despite often being brief, _mcuDetails.h headers exist so unit tests 
 * (using "host" MCU) can cover production _pcbDetails.h files, and therefore, 
 * production module init functions, which represents a significant category of
 * bugs.
 * 
 * 
 */

#ifndef timer_mcuDetails_h
#define timer_mcuDetails_h

#define HAL_BASE_ENABLED
#define HAL_DMA_ENABLED
#define HAL_TIM_ENABLED
#include <stm32_hal_dispatch.h>


typedef struct timerResource_s {
  TIM_TypeDef *base;
  IRQn_Type irqNumber;
} timerResource_t;

#endif // timer_mcuDetails_h
