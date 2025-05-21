#include <fmt_gpio_port.h>
#include <stm32l4xx_hal_gpio.h>

const uint32_t stmInputMode[] = {
  [INPUT_MODE_TRISTATE] = GPIO_NOPULL,
  [INPUT_MODE_PULL_DOWN] = GPIO_PULLDOWN,
  [INPUT_MODE_PULL_UP] = GPIO_PULLUP,
};

const uint32_t stmOutputMode[] = {
  [OUTPUT_MODE_PUSH_PULL] = GPIO_MODE_OUTPUT_PP,
  [OUTPUT_MODE_OPEN_DRAIN] = GPIO_MODE_OUTPUT_OD,
};


void port_initInputPin(void *const port, uint8_t pin, inputMode_t mode)
{
  GPIO_InitTypeDef config = {
    .Mode = GPIO_MODE_INPUT,
    .Pin = (0x1 << pin),
    .Pull = stmInputMode[mode],
  };
  HAL_GPIO_Init((GPIO_TypeDef*)port, &config);
}

void port_initOutputPin(void *const port, const uint8_t pin, outputMode_t mode) {
  GPIO_InitTypeDef config = {
    .Mode = stmOutputMode[mode],
    .Pin = (0x1 << pin),
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Pull = GPIO_NOPULL,
  };
  HAL_GPIO_Init((GPIO_TypeDef*)port, &config);
}

uint32_t port_readPort(void *const port)
{
  return ((GPIO_TypeDef*)port)->IDR;
}

/** set pin
 * @note pin is a uint_fast8 because setting a pin should be performant.
 */
void port_setPin(void *const port, uint_fast8_t pin, outLevel_t level)
{
  if (level == OUTPUT_TOGGLE)
  {
    HAL_GPIO_TogglePin((GPIO_TypeDef*)port, pin);
  }
  else
  {
    HAL_GPIO_WritePin((GPIO_TypeDef*)port, pin, (GPIO_PinState)level);
  }
}