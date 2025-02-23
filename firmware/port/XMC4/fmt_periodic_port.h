/** Firment utility: HW-timer driven periodic ISRs.
 * 
 * In Cortex-M processors, the NVIC provides preemption priority between IRQ's, 
 * so we can sometimes dispense with the RTOS, relying instead on a series of
 * ISR's executing at different periods and priorities (likely the faster-loops
 * running at a higher priorities  [lesser numeric values].)
 */

#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif
#include "xmc_ccu4.h" // timer

/** Configure a CCU4 Timer to drive an ISR at a fixed period.
 * Consumes a CCU4 timer slice.  (There are 4 slices per module).
 * The Timer's clock runs at the sys clock frequency. (144MHz for XMC4700).
 * To determine the ISR call frequency: f_sys / prescale / periodTick 
 * Example: 144MHz / 1024 prescale / 10000 period = 14.0625 Hz.
 */
void initPeriodicISR(
  XMC_CCU4_MODULE_t *module, 
  XMC_CCU4_SLICE_t *slice,
  XMC_CCU4_SLICE_PRESCALER_t prescaler,
  uint16_t periodTick,
  IRQn_Type IRQn,
  uint32_t priority);