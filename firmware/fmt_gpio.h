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
 * Configuring a new pin for GPIO use requires the following:
 * 1. In project/config/gpio_pcb.h, follow the instructions to add a new GPIO. 
 * 2. Verify this new gpio functions on the intended hardware.
 * 3. Add #define for the macro added to AVAILABLE_GPIOs in step 1. to
 *    firment/firmware/port/host/mcu_gpio.h - This signifies passing step 2.
 * 
 * Testing: 
 * The gpioTest.c will complain if step 3 above is not done.  It's a reminder
 * to perform on-target tests for new circuit paths before merging. 
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