/**
 * To prepare a new GPIO for use:
 * 1. associate the new pin to its purpose with a descriptive name.
 * 2. add this new name to AVAILABLE_GPIOs
 * 3. #define the PIN_ID corresponding to the position of the new pin in
 *    AVAILABLE_GPIOs.
 */

#ifndef gpio_pcbDetails_h
#define gpio_pcbDetails_h

#include <gpio_mcuDetails.h>

#define LED_0_PIN_ID 0
#define PHASE_U_HI_ID 1
#define PHASE_U_LO_ID 2
#define PHASE_V_HI_ID 3
#define PHASE_V_LO_ID 4
#define PHASE_W_HI_ID 5
#define PHASE_W_LO_ID 6

#define AVAILABLE_GPIOs {          \
    [LED_0_PIN_ID] = {GPIOC, 6},   \
    [PHASE_U_HI_ID] = {GPIOA, 8},  \
    [PHASE_U_LO_ID] = {GPIOC, 13}, \
    [PHASE_V_HI_ID] = {GPIOA, 9},  \
    [PHASE_V_LO_ID] = {GPIOA, 12}, \
    [PHASE_W_HI_ID] = {GPIOA, 10}, \
    [PHASE_W_LO_ID] = {GPIOB, 15}, \
}

#endif // gpio_pcbDetails_h
