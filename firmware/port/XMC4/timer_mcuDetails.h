/** timer_mcuDetails.h
 * Provides mcu headers and a type that encompases the details needed for each
 * resource for use by timer_pcbDetails.h
 * 
 * Despite often being brief, _mcuDetails.h headers exist so unit tests 
 * (using "host" MCU) can cover production _pcbDetails.h files, and therefore, 
 * production module init functions, which represents a significant category of
 * bugs.
 * 
 * 
 */

#ifndef timer_mcuDetails_h
#define timer_mcuDetails_h

#include "XMC4700.h"
#include "xmc_ccu4.h" // timer


typedef struct timerResource_s {
  XMC_CCU4_MODULE_t *module;
  XMC_CCU4_SLICE_t *slice;
  IRQn_Type irqNumber;
} timerResource_t;

#endif // timer_mcuDetails_h
