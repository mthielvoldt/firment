 /** 
 * To prepare a new timer for use: 
 * 1. add a _TIMER_ID With a descriptive name indicating the timer's purpose.
 * 2. add a row to AVAILABLE_TIMERS using this TIMER_ID, assigning available 
 *    hardware resources.
 * 3. Change the appropriate value of PERIODIC_USES_... from 0 to 1.
 */

#ifndef timer_pcbDetails_h
#define timer_pcbDetails_h

#include <timer_mcuDetails.h>

#define PERIODIC_A_TIMER_ID 0

#define AVAILABLE_TIMERS {\
  [PERIODIC_A_TIMER_ID] = {TIM3, TIM3_IRQn}, \
}


#define PERIODIC_USES_TIM1 0  // advanced
#define PERIODIC_USES_TIM2 0  // 32-bit
#define PERIODIC_USES_TIM3 1
#define PERIODIC_USES_TIM4 0
#define PERIODIC_USES_TIM5 0  // 32-bit
#define PERIODIC_USES_TIM6 0
#define PERIODIC_USES_TIM7 0
#define PERIODIC_USES_TIM8 0  // advanced
#define PERIODIC_USES_TIM15 0
#define PERIODIC_USES_TIM16 0
#define PERIODIC_USES_TIM17 0



#endif // timer_pcbDetails_h