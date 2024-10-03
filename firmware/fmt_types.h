#include "xmc_gpio.h"

#define ERR_DATA_LOST 0
#define ERR_SPI_FAULT 1
#define ERR_PB_ENCODE 2

typedef struct
{
  XMC_GPIO_PORT_t *port;
  XMC_GPIO_CONFIG_t config;
  uint8_t pin;
} portPin_t;

extern const XMC_GPIO_CONFIG_t gpOutPinConfig;