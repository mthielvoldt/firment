#include <stdint-gcc.h>

/**
 * Stolen from vPortRaiseBASEPRI implementation found in:
 * amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4_MPU/portmacro.h
 * 
 * Nomenclature note: "High priority" ISRs preempt "Low priority" ISRs.
 * When discussig numeric values of NVIC_prio_num, we use "lesser/greater" and
 * "least/greatest".  So the "Highest priority" ISR will have the "least"
 * NVIC_prio.
 */
inline static void disableLowPriorityInterrupts( uint32_t leastDisabledNVIC_prio )
{
  // TODO: find a way to test use of this.
  (void)leastDisabledNVIC_prio; // suppress warning.
}

#define enableAllInterrupts() disableLowPriorityInterrupts(0)
