#include <fmt_gpio_port.h>
#include "xmc_gpio.h"

const XMC_GPIO_MODE_t xmcInputMode[] = {
  [INPUT_MODE_TRISTATE] = XMC_GPIO_MODE_INPUT_TRISTATE,
  [INPUT_MODE_PULL_DOWN] = XMC_GPIO_MODE_INPUT_PULL_DOWN,
  [INPUT_MODE_PULL_UP] = XMC_GPIO_MODE_INPUT_PULL_UP,
};

const XMC_GPIO_MODE_t xmcOutputMode[] = {
  [OUTPUT_MODE_PUSH_PULL] = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
  [OUTPUT_MODE_OPEN_DRAIN] = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN,
};

const XMC_GPIO_OUTPUT_LEVEL_t xmcOutputLevel[] = {
  [OUTPUT_HIGH] = XMC_GPIO_OUTPUT_LEVEL_HIGH,
  [OUTPUT_LOW] = XMC_GPIO_OUTPUT_LEVEL_LOW,
  [OUTPUT_TOGGLE] = XMC_GPIO_OUTPUT_LEVEL_HIGH + XMC_GPIO_OUTPUT_LEVEL_LOW,
};

void port_initInputPin(void *const port, uint8_t pin, inputMode_t mode)
{
  XMC_GPIO_CONFIG_t config = {
    .mode = xmcInputMode[mode],
  };
  XMC_GPIO_Init((XMC_GPIO_PORT_t *const)port, pin, &config);
}

void port_initOutputPin(void *const port, const uint8_t pin, outputMode_t mode) {
  XMC_GPIO_CONFIG_t config = {
    .mode = xmcOutputMode[mode],
  };
  XMC_GPIO_Init((XMC_GPIO_PORT_t *const)port, pin, &config);
}

uint32_t port_readPort(void *const port)
{
  return ((XMC_GPIO_PORT_t *const)port)->IN;
}

/** set pin
 * @note pin is a uint_fast8 because setting a pin should be performant.
 */
void port_setPin(void *const port, uint_fast8_t pin, outLevel_t level)
{
  XMC_GPIO_SetOutputLevel(port, pin, xmcOutputLevel[level]);
}