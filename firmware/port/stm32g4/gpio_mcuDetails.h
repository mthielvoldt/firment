/** gpio_mcuDetails.h
 * Provides mcu headers for use by gpio_pcbDetails.h
 * Despite its brevity, this is separate from gpio_pcbDetails.h so unit tests 
 * (using "host" MCU) can cover files that #include pcbDetails files - e.g.
 * project_comms.c which uses pcbDetails to access pin-mapped LEDs. 
 * 
 * Note that "host" MCU can currently only meet the requirements of pcb0.
 * Adding host support for pcb1 is an active TODO. 
 * 
 * To prepare a new GPIO for use: 
 * 1. associate the new pin to its purpose with a descriptive name.
 * 2. add this new name to AVAILABLE_GPIOs
 * 3. #define the PIN_ID corresponding to the position of the new pin in 
 *    AVAILABLE_GPIOs.
 */

#ifndef gpio_mcuDetails_h
#define gpio_mcuDetails_h

#define HAL_GPIO_ENABLED
#include <stm32_hal_dispatch.h>

typedef struct portPin_s
{
  GPIO_TypeDef *const port;
  uint8_t pin;
} portPin_t;

#endif // gpio_mcuDetails_h
