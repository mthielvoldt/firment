/**
 * Provides XMC4-family routing of GPIO data (port addresses).
 * 
 * The used macros are Px_y, eg P5_9  (port 5, pin 9)
 * Here, you don't see any macros defined in this file because the project files
 * that consume this file use the macros defined in xmc_gpio.h directly. 
 * 
 * A different manufacturer's header may need macros defined to re-map the 
 * macro names to firment's naming convention. 
 */

#include <xmc_gpio.h>

typedef struct portPin_s
{
  XMC_GPIO_PORT_t *const port;
  uint8_t pin;
} portPin_t;