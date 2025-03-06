/** RTE_Device.h for test environment */

#pragma once

#include <stdint.h>

// TODO: (3) feature envy.  Move to fmt_ioc.h
// before above, it'll need some abstraction. 
typedef struct _RTE_IOC {
  uint32_t *port; 
  uint8_t pin;
  uint32_t IRQn;  // this is ultimately what ERU stuff boils down to. 
} RTE_IOC_t;