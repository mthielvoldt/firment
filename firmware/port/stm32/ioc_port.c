/**
 */
#include "fmt_ioc.h"
#include "ioc_mcuDetails.h" // maps wiring possibilities to indexible resources.
#include <ioc_pcbDetails.h>
#include <stm32l4xx_hal_gpio.h> // GPIO_MODE_...

const uint32_t edgeMap[] = {
    [EDGE_TYPE_NONE] = GPIO_MODE_INPUT,
    [EDGE_TYPE_FALLING] = GPIO_MODE_IT_FALLING,
    [EDGE_TYPE_RISING] = GPIO_MODE_IT_RISING,
    [EDGE_TYPE_BOTH] = GPIO_MODE_IT_RISING_FALLING,
};

typedef enum iocErr_e
{
  IOC_OK,
  IOC_ERR_RESOURCE_UNAVAILABLE,
  IOC_ERR_CALLBACK_ALREADY_ASSIGNED,
} iocErr_t;
typedef void (*isrCallback_t)(void);

const RTE_IOC_t iocConfigs[] = AVAILABLE_IOCs;
#define IOC_CONFIGS_COUNT (sizeof(iocConfigs) / sizeof(RTE_IOC_t))

static iocErr_t storeCallback(uint8_t pin, isrCallback_t callback);
static bool iocIdValid(uint8_t iocId);

/** Init Interrupt-on-change ISR, linking to a specific GPIO.
 *
 */
bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel, // unused in stm32l4
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  if (!iocIdValid(iocId) || activeEdges == EDGE_TYPE_NONE || !callback)
    return false;

  const RTE_IOC_t *ioc = &iocConfigs[iocId];

  if (storeCallback(ioc->pin, callback) != IOC_OK)
    return false;

  GPIO_InitTypeDef config = {
      .Mode = edgeMap[activeEdges],
      .Pin = (0x1 << ioc->pin),
      .Pull = GPIO_NOPULL, // could be passed in.
  };
  HAL_GPIO_Init(ioc->port, &config);

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, 0);

  NVIC_SetPriority(ioc->irqNum, encodedPrio);
  NVIC_EnableIRQ(ioc->irqNum);
  return true;
}

void fmt_enableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  EXTI->IMR1 |= 1UL << ioc->pin;
}

void fmt_disableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  EXTI->IMR1 &= ~(1UL << ioc->pin);
}

bool fmt_getIocPinState(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return false;
  const RTE_IOC_t *ioc = &iocConfigs[iocId];
  return (ioc->port->IDR & (1UL << ioc->pin)) > 0UL;
}

/******************* ISRs *******************/
#define EXTI_HANDLER(lower, label)             \
  static isrCallback_t callback##lower = NULL; \
  static uint32_t pinEnabled##lower = 0;       \
  void EXTI##label##_IRQHandler(void);         \
  void EXTI##label##_IRQHandler(void)          \
  {                                            \
    EXTI->PR1 |= pinEnabled##lower;            \
    if (callback##lower)                       \
      (*callback##lower)();                    \
  }
#if IOC_USE_EXTI0
EXTI_HANDLER(0, 0)
#endif
#if IOC_USE_EXTI1
EXTI_HANDLER(1, 1)
#endif
#if IOC_USE_EXTI2
EXTI_HANDLER(2, 2)
#endif
#if IOC_USE_EXTI3
EXTI_HANDLER(3, 3)
#endif
#if IOC_USE_EXTI4
EXTI_HANDLER(4, 4)
#endif
#if IOC_USE_EXTI9_5
EXTI_HANDLER(5, 9_5)
#endif
#if IOC_USE_EXTI15_10
EXTI_HANDLER(10, 15_10)
#endif

#define CASE_PIN(upper, lower)                  \
  if (pin >= lower && pin <= upper)             \
  {                                             \
    if (callback##lower == NULL)                \
    {                                           \
      pinEnabled##lower = (1 << pin);           \
      callback##lower = callback;               \
      return IOC_OK;                            \
    }                                           \
    else                                        \
      return IOC_ERR_CALLBACK_ALREADY_ASSIGNED; \
  }

static iocErr_t storeCallback(uint8_t pin, isrCallback_t callback)
{

#if IOC_USE_EXTI0
  CASE_PIN(0, 0)
#endif
#if IOC_USE_EXTI1
  CASE_PIN(1, 1)
#endif
#if IOC_USE_EXTI2
  CASE_PIN(2, 2)
#endif
#if IOC_USE_EXTI3
  CASE_PIN(3, 3)
#endif
#if IOC_USE_EXTI4
  CASE_PIN(4, 4)
#endif
#if IOC_USE_EXTI9_5
  EXTI_HANDLER(9, 5)
#endif
#if IOC_USE_EXTI15_10
  CASE_PIN(15, 10)
#endif

  return IOC_ERR_RESOURCE_UNAVAILABLE;
}

static bool iocIdValid(uint8_t iocId)
{
  return iocId < IOC_CONFIGS_COUNT;
}
