/**
 * @file fmt_periodic.h
 * @brief Calls a provided function at a fixed interval from a HW-timer ISR.
 */

#ifndef fmt_periodic_h
#define fmt_periodic_h

#include <stdbool.h>
#include <stdint.h>

typedef void(*callback_t)(void);

bool fmt_initPeriodic(
  uint8_t timerId, uint32_t intervalUs, uint32_t priority, callback_t callback); 

#endif // fmt_periodic_h