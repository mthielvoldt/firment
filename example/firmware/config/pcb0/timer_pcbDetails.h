
#ifndef timer_pcbDetails_h
#define timer_pcbDetails_h

#include "XMC4700.h"
#include "xmc_ccu4.h" // timer


typedef struct timerResource_s {
  XMC_CCU4_MODULE_t *module;
  XMC_CCU4_SLICE_t *slice;
  IRQn_Type irqNumber;
} timerResource_t;

#define TIMER_0 {CCU40, CCU40_CC40, CCU40_0_IRQn}

#define AVAILABLE_TIMERS {TIMER_0,}

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