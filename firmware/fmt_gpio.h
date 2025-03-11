/** Firment GPIO abstraction
 * Provides portable read, write and init functions.
 * Each pin is mapped to an abstract uint8_t pinId, allowing hardware-agnostic
 * access.
 * 
 * Mapping between pinId used here and target MCU ports/pins is done in 
 * <projectSrc>/config/gpio_pcb.h  (because it's pcb-determined information)
 * 
 * Recommended use:
 * Init functions should be called from the modules that use these pins.
 * Configuring a new pin for GPIO use requres the following:
 * 1. in firment/firmware/port/<family>/<mcu>/mcu_gpio.h 
 *  1a. #define GPIO_n <PORT, PIN> in the MCU Mfg's language.
 *  1b. Add this GPIO_n to AVAILABLE_GPIOs
 * 2. Verify this new gpio functions on the intended hardware.
 * 3. Repeat 1a, 1b in firment/firmware/port/mcu_gpio.h
 * 
 * Testing: 
 * This module has a mock here: firment/firmware/port/host/fmt_gpio_mock.c
 * This mock will complain if a pin is initialized that isn't #defined in 
 * host/mcu_gpio.h 
 * 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum _inputMode {
  INPUT_MODE_TRISTATE,
  INPUT_MODE_PULL_UP,
  INPUT_MODE_PULL_DOWN,
} inputMode_t;

typedef enum _outputMode {
  OUTPUT_MODE_PUSH_PULL,
  OUTPUT_MODE_OPEN_DRAIN,
} outputMode_t;

typedef enum _outLevel {
  OUTPUT_HIGH,
  OUTPUT_LOW,
  OUTPUT_TOGGLE,
} outLevel_t;

bool fmt_initGpioInPin(uint8_t pinId, inputMode_t mode);

bool fmt_initGpioOutPin(uint8_t pinId, outputMode_t mode);

bool fmt_readPin(uint8_t pinId);

bool fmt_setPin(uint8_t pinId, outLevel_t level);