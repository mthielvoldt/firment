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
  [PERIODIC_A_TIMER_ID] = {CCU40, CCU40_CC40, CCU40_0_IRQn}, \
}


#define PERIODIC_USES_CCU40_0 1
#define PERIODIC_USES_CCU40_1 0
#define PERIODIC_USES_CCU40_2 0
#define PERIODIC_USES_CCU40_3 0

#define PERIODIC_USES_CCU41_0 0
#define PERIODIC_USES_CCU41_1 0
#define PERIODIC_USES_CCU41_2 0
#define PERIODIC_USES_CCU41_3 0

#define PERIODIC_USES_CCU42_0 0
#define PERIODIC_USES_CCU42_1 0
#define PERIODIC_USES_CCU42_2 0
#define PERIODIC_USES_CCU42_3 0

#define PERIODIC_USES_CCU43_0 0
#define PERIODIC_USES_CCU43_1 0
#define PERIODIC_USES_CCU43_2 0
#define PERIODIC_USES_CCU43_3 0

#define PERIODIC_USES_CCU80_0 0
#define PERIODIC_USES_CCU80_1 0
#define PERIODIC_USES_CCU80_2 0
#define PERIODIC_USES_CCU80_3 0

#define PERIODIC_USES_CCU81_0 0
#define PERIODIC_USES_CCU81_1 0
#define PERIODIC_USES_CCU81_2 0
#define PERIODIC_USES_CCU81_3 0


#endif // timer_pcbDetails_h