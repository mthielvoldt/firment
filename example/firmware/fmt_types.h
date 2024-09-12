#include "xmc_gpio.h"

typedef struct
{
  XMC_GPIO_PORT_t *port;
  uint8_t pin;
} portPin_t;

extern const XMC_GPIO_CONFIG_t gpOutPinConfig;