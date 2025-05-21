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
 */
#define IOC_4  {GPIOC, 13, EXTI15_10_IRQn}
#define IOC_14 {GPIOD,  2, EXTI2_IRQn}


#endif // ioc_mcuDetails_H
