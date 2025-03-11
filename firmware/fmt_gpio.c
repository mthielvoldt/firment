#include "fmt_gpio.h"
#include "fmt_gpio_port.h"
#include <gpio_pcb.h>

#define ASSERT_PIN_ID_VALID                                \
  if (pinId >= (sizeof(availableGpios) / sizeof(portPin_t))) \
  return false

static portPin_t availableGpios[] = AVAILABLE_GPIOs;

bool fmt_initGpioInPin(uint8_t pinId, inputMode_t mode)
{
  ASSERT_PIN_ID_VALID;
  portPin_t portPin = availableGpios[pinId];
  port_initInputPin((void *)portPin.port, portPin.pin, mode);
  return true;
}

bool fmt_initGpioOutPin(uint8_t pinId, outputMode_t mode)
{
  ASSERT_PIN_ID_VALID;
  portPin_t portPin = availableGpios[pinId];
  port_initOutputPin((void *)portPin.port, portPin.pin, mode);
  return true;
}

bool fmt_readPin(uint8_t pinId)
{
  ASSERT_PIN_ID_VALID;
  portPin_t portPin = availableGpios[pinId];
  return port_readPort((void *)portPin.port);
}

bool fmt_setPin(uint8_t pinId, outLevel_t level)
{
  ASSERT_PIN_ID_VALID;
  portPin_t portPin = availableGpios[pinId];
  port_setPin((void*)portPin.port, portPin.pin, level);
  return true;
}