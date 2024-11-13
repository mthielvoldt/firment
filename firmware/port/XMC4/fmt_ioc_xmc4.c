#include "fmt_ioc.h"
#include <xmc_gpio.h>

const XMC_ERU_ETL_EDGE_DETECTION_t edgeMap[] = {
  [EDGE_TYPE_NONE] = XMC_ERU_ETL_EDGE_DETECTION_DISABLED,
  [EDGE_TYPE_FALLING] = XMC_ERU_ETL_EDGE_DETECTION_FALLING,
  [EDGE_TYPE_RISING] = XMC_ERU_ETL_EDGE_DETECTION_RISING,
  [EDGE_TYPE_BOTH] = XMC_ERU_ETL_EDGE_DETECTION_BOTH,
};

/** Init Interrupt-on-change ISR, linking to a specific GPIO.
 * 
 */
void fmt_initIoc(
    RTE_IOC_t inputConfig,
    edgeType_t activeEdges, 
    uint8_t outputChannel,
    uint8_t IRQn,
    uint32_t priority)
{
  const XMC_GPIO_CONFIG_t gpio_config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE};
  XMC_GPIO_Init(inputConfig.port, inputConfig.pin, &gpio_config);

  // Configure ERU for rising edge detection on P1.0
  XMC_ERU_ETL_CONFIG_t eru_etl_config = {
      .input_b = inputConfig.inputChannel,
      .source = inputConfig.source,
      .edge_detection = edgeMap[activeEdges],
      .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
      .enable_output_trigger = XMC_ERU_ETL_OUTPUT_TRIGGER_ENABLED,
      .output_trigger_channel = outputChannel,
  };

  // Configure OGU for interrupt generation
  XMC_ERU_OGU_CONFIG_t eru_ogu_config = {
      .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER,
  };

  XMC_ERU_ETL_Init(inputConfig.eru, inputConfig.etlNum, &eru_etl_config);
  XMC_ERU_OGU_Init(inputConfig.eru, outputChannel, &eru_ogu_config);

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, 0);

  NVIC_SetPriority(IRQn, encodedPrio);

  NVIC_EnableIRQ(IRQn);
}
