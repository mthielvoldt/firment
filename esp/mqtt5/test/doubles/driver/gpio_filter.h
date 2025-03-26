#include "gpio.h"

typedef enum {
  GLITCH_FILTER_CLK_SRC_APB,      /*!< Select APB clock as the source clock */
  GLITCH_FILTER_CLK_SRC_DEFAULT,  /*!< Select APB clock as the default clock choice */
} soc_periph_glitch_filter_clk_src_t;

typedef struct {
  int clk_src; /*!< Clock source for the glitch filter */
  gpio_num_t gpio_num;    /*!< GPIO number */
} gpio_pin_glitch_filter_config_t;

typedef struct gpio_glitch_filter_t *gpio_glitch_filter_handle_t;

esp_err_t gpio_new_pin_glitch_filter(
  const gpio_pin_glitch_filter_config_t *config, 
  gpio_glitch_filter_handle_t *ret_filter);

esp_err_t gpio_glitch_filter_enable(gpio_glitch_filter_handle_t filter);