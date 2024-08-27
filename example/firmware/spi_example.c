
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "xmc_gpio.h"
#include "xmc_common.h"

int main(void) {
  XMC_GPIO_PORT_t * const port = XMC_GPIO_PORT0;
  const uint8_t pin = 13;
  const XMC_GPIO_CONFIG_t pinConfig = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE
  };

  XMC_GPIO_Init(port, pin, &pinConfig);
  for (;;) {

    XMC_GPIO_ToggleOutput(port, pin);
    for (volatile int i = 0; i < 400000; i++);
    
  }
}