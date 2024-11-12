#include "fmt_ioc.h"

#include <XMC4700.h>
#include <xmc_eru.h>
#include <xmc_gpio.h>

#define INTERRUPT_NODE_ID ERU0_3_IRQn
#define INTERRUPT_EVENT_PRIORITY 30
#define ERU_EXTERNAL_EVENT_HANDLER ERU0_3_IRQHandler

// Maps to ERU0.3B0
#define GPIO_INPUT_PORT XMC_GPIO_PORT1
#define GPIO_INPUT_PIN 0U

#define LED_PORT XMC_GPIO_PORT5
#define LED_PIN 9U

void fmt_initIoc(uint32_t port, uint8_t pin, edgeType_t activeEdges, isr_t isr)
{

  const XMC_GPIO_CONFIG_t gpio_config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE};
  const XMC_GPIO_CONFIG_t ledCfg = {
      .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
      .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
      .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM,
  };
  XMC_GPIO_Init(GPIO_INPUT_PORT, GPIO_INPUT_PIN, &gpio_config);
  XMC_GPIO_Init(LED_PORT, LED_PIN, &ledCfg);

  // Configure ERU for rising edge detection on P1.0
  XMC_ERU_ETL_CONFIG_t eru_etl_config = {
      .input_b = XMC_ERU_ETL_INPUT_B0,                     // Select P1.0 as input (based on pin mapping)
      .source = XMC_ERU_ETL_SOURCE_B,                      // Use B input source
      .edge_detection = XMC_ERU_ETL_EDGE_DETECTION_RISING, // Trigger on rising edge
      .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
      .enable_output_trigger = XMC_ERU_ETL_OUTPUT_TRIGGER_ENABLED,
      .output_trigger_channel = XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL3,
  };

  // Configure OGU for interrupt generation
  XMC_ERU_OGU_CONFIG_t eru_ogu_config = {
      .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER, // Generate interrupt on trigger
  };

  XMC_ERU_ETL_Init(ERU0_ETL3, &eru_etl_config); // Initialize ERU channel ETL1
  XMC_ERU_OGU_Init(ERU0_OGU3, &eru_ogu_config); // Initialize ERU OGU channel OGU1

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), INTERRUPT_EVENT_PRIORITY, 0);

  NVIC_SetPriority(INTERRUPT_NODE_ID, encodedPrio);

  NVIC_EnableIRQ(INTERRUPT_NODE_ID);
}

void ERU_EXTERNAL_EVENT_HANDLER(void)
{
  XMC_GPIO_ToggleOutput(LED_PORT, LED_PIN);
}