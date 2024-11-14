/** Firment GPIo abstraction
 * Provides portable read, write and init functions.
 */

#pragma once
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

void fmt_initInputPin(void *const port, uint8_t pin, inputMode_t mode);

void fmt_initOutputPin(void *const port, uint8_t pin, outputMode_t mode);

uint32_t fmt_readPort(void *const port);

void fmt_setPin(void *const port, uint_fast8_t pin, outLevel_t level);