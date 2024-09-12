#include "fmt_types.h"

const XMC_GPIO_CONFIG_t gpOutPinConfig = {
      .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
      .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
      .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM,
  };