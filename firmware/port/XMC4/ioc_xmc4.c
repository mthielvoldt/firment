#include "fmt_ioc.h"
#include <ioc_mcuDetails.h>
#include <ioc_pcbDetails.h>
#include <xmc_gpio.h>

const XMC_ERU_ETL_EDGE_DETECTION_t edgeMap[] = {
    [EDGE_TYPE_NONE] = XMC_ERU_ETL_EDGE_DETECTION_DISABLED,
    [EDGE_TYPE_FALLING] = XMC_ERU_ETL_EDGE_DETECTION_FALLING,
    [EDGE_TYPE_RISING] = XMC_ERU_ETL_EDGE_DETECTION_RISING,
    [EDGE_TYPE_BOTH] = XMC_ERU_ETL_EDGE_DETECTION_BOTH,
};

const RTE_IOC_t iocConfigs[] = AVAILABLE_IOCs;

typedef void (*isrCallback_t)(void);
isrCallback_t callbacks[ERU_COUNT * OUTPUT_COUNT_PER_ERU] = {0};

static bool iocIdValid(uint8_t iocId)
{
  return iocId < (sizeof(iocConfigs) / sizeof(RTE_IOC_t));
}
static uint8_t getIsrId(uint8_t eruId, uint8_t output)
{
  return eruId * OUTPUT_COUNT_PER_ERU + output;
}

/** Init Interrupt-on-change ISR, linking to a specific GPIO.
 *
 */
bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  if (!iocIdValid(iocId) || outputChannel >= OUTPUT_COUNT_PER_ERU)
    return false;

  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  uint8_t irqNum = ioc->usableIrqNums[outputChannel];

  callbacks[getIsrId(ioc->eruId, outputChannel)] = callback;

  const XMC_GPIO_CONFIG_t gpio_config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE};
  XMC_GPIO_Init(ioc->port, ioc->pin, &gpio_config);

  // Configure ERU for rising edge detection on P1.0
  XMC_ERU_ETL_CONFIG_t eru_etl_config = {
      .input_b = ioc->inputChannel,
      .source = ioc->source,
      .edge_detection = edgeMap[activeEdges],
      .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
      .enable_output_trigger = XMC_ERU_ETL_OUTPUT_TRIGGER_ENABLED,
      .output_trigger_channel = outputChannel,
  };

  // Configure OGU for interrupt generation
  XMC_ERU_OGU_CONFIG_t eru_ogu_config = {
      .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER,
  };

  XMC_ERU_ETL_Init(ioc->eru, ioc->etlNum, &eru_etl_config);
  XMC_ERU_OGU_Init(ioc->eru, outputChannel, &eru_ogu_config);

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, 0);

  NVIC_SetPriority(irqNum, encodedPrio);

  NVIC_EnableIRQ(irqNum);
  return true;
}

void fmt_enableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  ioc->eru->EXICON_b[ioc->etlNum].PE = true;
}

void fmt_disableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  ioc->eru->EXICON_b[ioc->etlNum].PE = false;
}

bool fmt_getIocPinState(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return false;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  return XMC_GPIO_GetInput(ioc->port, ioc->pin);
}

/******************* ISRs *******************/
#if 1
#if IOC_USE_ISR_0
void ERU0_0_IRQHandler(void); // suppress missing-declaration warning
void ERU0_0_IRQHandler(void)
{
  isrCallback_t cb = callbacks[0];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_1
void ERU0_1_IRQHandler(void); // suppress missing-declaration warning
void ERU0_1_IRQHandler(void)
{
  isrCallback_t cb = callbacks[1];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_2
void ERU0_2_IRQHandler(void); // suppress missing-declaration warning
void ERU0_2_IRQHandler(void)
{
  isrCallback_t cb = callbacks[2];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_3
void ERU0_3_IRQHandler(void); // suppress missing-declaration warning
void ERU0_3_IRQHandler(void)
{
  isrCallback_t cb = callbacks[3];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_4
void ERU1_0_IRQHandler(void); // suppress missing-declaration warning
void ERU1_0_IRQHandler(void)
{
  isrCallback_t cb = callbacks[4];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_5
void ERU1_1_IRQHandler(void); // suppress missing-declaration warning
void ERU1_1_IRQHandler(void)
{
  isrCallback_t cb = callbacks[5];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_6
void ERU1_2_IRQHandler(void); // suppress missing-declaration warning
void ERU1_2_IRQHandler(void)
{
  isrCallback_t cb = callbacks[6];
  if (cb)
    cb();
}
#endif
#if IOC_USE_ISR_7
void ERU1_3_IRQHandler(void); // suppress missing-declaration warning
void ERU1_3_IRQHandler(void)
{
  isrCallback_t cb = callbacks[7];
  if (cb)
    cb();
}
#endif
#endif
