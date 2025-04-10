#include <stdint.h>
#include <stddef.h>

/**
 * Nomenclature note: "High priority" ISRs preempt "Low priority" ISRs.
 * When discussig numeric values of NVIC_prio_num, we use "lesser/greater" and
 * "least/greatest".  So the "Highest priority" ISR will have the "least"
 * NVIC_prio.
 */
void (*disableLowPriorityInterruptsCallback)(void) = NULL;
void (*enableAllInterruptsCallback)(void) = NULL;

inline static void disableLowPriorityInterrupts(uint32_t leastDisabledNVIC_prio)
{
  if (leastDisabledNVIC_prio > 0) // disableLow... called.
  {
    if (disableLowPriorityInterruptsCallback)
    { 
      void (*tempCb)(void) = disableLowPriorityInterruptsCallback;
      disableLowPriorityInterruptsCallback = NULL; // break infinite recursion
      tempCb();
    }
  }
  else // enableAllInterrupts called.
  {
    if (enableAllInterruptsCallback)
    {
      void (*tempCb)(void) = enableAllInterruptsCallback;
      enableAllInterruptsCallback = NULL; // break infinite recursion
      tempCb();
    }
  }
}

#define enableAllInterrupts() disableLowPriorityInterrupts(0)
