/**
 * TODO: (6)
 * - Rename to fmt_gpio. 
 * - Replace both port copies with one copy in firmware/.
 * - Fix with TDD (broken with forward-declare right now)
 */
#include <stdint.h>

// forward-declare a gpio_config type.
typedef struct gpio_config* gpio_config_p;

typedef struct
{
  void *port;
  gpio_config_p config;
  uint8_t pin;
} portPin_t;