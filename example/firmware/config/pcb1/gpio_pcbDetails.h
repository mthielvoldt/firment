/**
 * To prepare a new GPIO for use: 
 * 1. associate the new pin to its purpose with a descriptive name.
 * 2. add this new name to AVAILABLE_GPIOs
 * 3. #define the PIN_ID corresponding to the position of the new pin in 
 *    AVAILABLE_GPIOs.
 */

#ifndef gpio_pcbDetails_h
#define gpio_pcbDetails_h

#include <stm32l4xx_hal_gpio.h>

typedef struct portPin_s
{
  GPIO_TypeDef *const port;
  uint8_t pin;
} portPin_t;


#define LED_0_PIN GPIOD, 13
#define LED_1_PIN GPIOD, 12


#define AVAILABLE_GPIOs {{LED_0_PIN}, {LED_1_PIN}}
#define LED_0_PIN_ID 0
#define LED_1_PIN_ID 1

#endif // gpio_pcbDetails_h
