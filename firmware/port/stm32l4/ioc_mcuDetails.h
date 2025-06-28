#ifndef ioc_mcuDetails_H
#define ioc_mcuDetails_H

#include <stm32l4xx_hal_gpio.h>


typedef struct _RTE_IOC
{
  GPIO_TypeDef *port;
  uint8_t pin;
  IRQn_Type irqNum;
} RTE_IOC_t;

/**
 * From stm32l4xx_hal_exti.c: 
   Exti lines 0 to 15 are linked to gpio pin number 0 to 15. Gpio port can
        be selected through multiplexer.

  Some EXTI channels (specifically 5-9 and 10-15) share IRQn's
 */
#define IOC_B1 {GPIOB, 1, EXTI1_IRQn}
#define IOC_B2 {GPIOB, 2, EXTI2_IRQn}


#endif // ioc_mcuDetails_H
