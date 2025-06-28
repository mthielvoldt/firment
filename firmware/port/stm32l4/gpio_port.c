#include <fmt_gpio_port.h>
#define USE_FULL_ASSERT
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_rcc.h>

const uint32_t stmInputMode[] = {
  [INPUT_MODE_TRISTATE] = GPIO_NOPULL,
  [INPUT_MODE_PULL_DOWN] = GPIO_PULLDOWN,
  [INPUT_MODE_PULL_UP] = GPIO_PULLUP,
};

const uint32_t stmOutputMode[] = {
  [OUTPUT_MODE_PUSH_PULL] = GPIO_MODE_OUTPUT_PP,
  [OUTPUT_MODE_OPEN_DRAIN] = GPIO_MODE_OUTPUT_OD,
};

static void enableRelevantClock(uint32_t const port);

void enableRelevantClock(uint32_t const port) {
  switch (port) {
    case (uint32_t)GPIOA:
    __HAL_RCC_GPIOA_CLK_ENABLE();
    break;
    case (uint32_t)GPIOB:
    __HAL_RCC_GPIOB_CLK_ENABLE();
    break;
    case (uint32_t)GPIOC:
    __HAL_RCC_GPIOC_CLK_ENABLE();
    break;
    case (uint32_t)GPIOD:
    __HAL_RCC_GPIOD_CLK_ENABLE();
    break;
    case (uint32_t)GPIOE:
    __HAL_RCC_GPIOE_CLK_ENABLE();
    break;
    case (uint32_t)GPIOF:
    __HAL_RCC_GPIOF_CLK_ENABLE();
    break;
    case (uint32_t)GPIOG:
    __HAL_RCC_GPIOG_CLK_ENABLE();
    break;
    case (uint32_t)GPIOH:
    __HAL_RCC_GPIOH_CLK_ENABLE();
    break;
  }
}


void port_initInputPin(void *const port, uint8_t pin, inputMode_t mode)
{
  GPIO_InitTypeDef config = {
    .Mode = GPIO_MODE_INPUT,
    .Pin = (0x1 << pin),
    .Pull = stmInputMode[mode],
  };
  enableRelevantClock((uint32_t)port);
  HAL_GPIO_Init((GPIO_TypeDef*)port, &config);
}

void port_initOutputPin(void *const port, const uint8_t pin, outputMode_t mode) {
  GPIO_InitTypeDef config = {
    .Mode = stmOutputMode[mode],
    .Pin = (0x1 << pin),
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Pull = GPIO_NOPULL,
  };
  enableRelevantClock((uint32_t)port);
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
    HAL_GPIO_TogglePin((GPIO_TypeDef*)port, (0x1 << pin));
  }
  else
  {
    HAL_GPIO_WritePin((GPIO_TypeDef*)port, (0x1 << pin), (GPIO_PinState)level);
  }
}