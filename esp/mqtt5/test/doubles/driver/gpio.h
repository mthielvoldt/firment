#pragma once

#include <stdint.h>
#include "../esp_err.h"

typedef enum {
  GPIO_NUM_NC = -1,    /*!< Use to signal not connected to S/W */
  GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
  GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
  GPIO_NUM_2 = 2,     /*!< GPIO2, input and output */
  GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
  GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
  GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
  GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
  GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
  GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
  GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
  GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
  GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
  GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
  GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
  GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
  GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
  GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
  GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
  GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
  GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */
  GPIO_NUM_20 = 20,   /*!< GPIO20, input and output */
  GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
  GPIO_NUM_26 = 26,   /*!< GPIO26, input and output */
  GPIO_NUM_27 = 27,   /*!< GPIO27, input and output */
  GPIO_NUM_28 = 28,   /*!< GPIO28, input and output */
  GPIO_NUM_29 = 29,   /*!< GPIO29, input and output */
  GPIO_NUM_30 = 30,   /*!< GPIO30, input and output */
  GPIO_NUM_31 = 31,   /*!< GPIO31, input and output */
  GPIO_NUM_32 = 32,   /*!< GPIO32, input and output */
  GPIO_NUM_33 = 33,   /*!< GPIO33, input and output */
  GPIO_NUM_34 = 34,   /*!< GPIO34, input and output */
  GPIO_NUM_35 = 35,   /*!< GPIO35, input and output */
  GPIO_NUM_36 = 36,   /*!< GPIO36, input and output */
  GPIO_NUM_37 = 37,   /*!< GPIO37, input and output */
  GPIO_NUM_38 = 38,   /*!< GPIO38, input and output */
  GPIO_NUM_39 = 39,   /*!< GPIO39, input and output */
  GPIO_NUM_40 = 40,   /*!< GPIO40, input and output */
  GPIO_NUM_41 = 41,   /*!< GPIO41, input and output */
  GPIO_NUM_42 = 42,   /*!< GPIO42, input and output */
  GPIO_NUM_43 = 43,   /*!< GPIO43, input and output */
  GPIO_NUM_44 = 44,   /*!< GPIO44, input and output */
  GPIO_NUM_45 = 45,   /*!< GPIO45, input and output */
  GPIO_NUM_46 = 46,   /*!< GPIO46, input and output */
  GPIO_NUM_47 = 47,   /*!< GPIO47, input and output */
  GPIO_NUM_48 = 48,   /*!< GPIO48, input and output */
  GPIO_NUM_MAX,
} gpio_num_t;

typedef enum {
  GPIO_PULLUP_ONLY,               /*!< Pad pull up            */
  GPIO_PULLDOWN_ONLY,             /*!< Pad pull down          */
  GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
  GPIO_FLOATING,                  /*!< Pad floating           */
} gpio_pull_mode_t;

typedef enum {
  GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
  GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
  GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
  GPIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
  GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
  GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
  GPIO_INTR_MAX,
} gpio_int_type_t;

#define BIT64(nr)               (1ULL << (nr))
#define BIT2     0x00000004
#define BIT1     0x00000002
#define BIT0     0x00000001

#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)    ///< bit mask for input
#define GPIO_MODE_DEF_OUTPUT          (BIT1)    ///< bit mask for output
#define GPIO_MODE_DEF_OD              (BIT2)    ///< bit mask for OD mode

typedef enum {
  GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         /*!< GPIO mode : disable input and output             */
  GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             /*!< GPIO mode : input only                           */
  GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           /*!< GPIO mode : output only mode                     */
  GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               /*!< GPIO mode : output only with open-drain mode     */
  GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), /*!< GPIO mode : output and input with open-drain mode*/
  GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         /*!< GPIO mode : output and input mode                */
} gpio_mode_t;

typedef struct {
  uint64_t pin_bit_mask;          /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
  int mode;               /*!< GPIO mode: set input/output mode                     */
  int pull_up_en;       /*!< GPIO pull-up                                         */
  int pull_down_en;   /*!< GPIO pull-down                                       */
  int intr_type;      /*!< GPIO interrupt type                                  */
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
  gpio_hys_ctrl_mode_t hys_ctrl_mode;       /*!< GPIO hysteresis: hysteresis filter on slope input    */
#endif
} gpio_config_t;

esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull);
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level);
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);