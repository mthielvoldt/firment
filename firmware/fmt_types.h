#include "xmc_gpio.h"

typedef struct
{
  XMC_GPIO_PORT_t *port;
  XMC_GPIO_CONFIG_t config;
  uint8_t pin;
} portPin_t;

extern const XMC_GPIO_CONFIG_t gpOutPinConfig;