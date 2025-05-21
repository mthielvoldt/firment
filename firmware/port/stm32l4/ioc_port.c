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

const RTE_IOC_t iocConfigs[] = AVAILABLE_IOCs;

typedef void (*isrCallback_t)(void);

// How many callbacks do I want to be able to register? 
isrCallback_t callbacks[sizeof(iocConfigs) / sizeof(RTE_IOC_t)] = {0};

static bool iocIdValid(uint8_t iocId)
{
  return iocId < (sizeof(iocConfigs) / sizeof(RTE_IOC_t));
}


/** Init Interrupt-on-change ISR, linking to a specific GPIO.
 *
 */
bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,  // unused in stm32l4
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  if (!iocIdValid(iocId) || activeEdges == EDGE_TYPE_NONE || !callback)
    return false;

  const RTE_IOC_t *ioc = &iocConfigs[iocId];

  callbacks[iocId] = callback;

  GPIO_InitTypeDef config = {
    .Mode = edgeMap[activeEdges],
    .Pin = (0x1 << iocId),
    .Pull = GPIO_NOPULL,  // could be passed in.
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
#if 1
#if IOC_USE_EXTI0
void EXTI0_IRQHandler(void); // suppress missing-declaration warning
void EXTI0_IRQHandler(void)
{
  isrCallback_t cb = callbacks[0];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI1
void EXTI1_IRQHandler(void); // suppress missing-declaration warning
void EXTI1_IRQHandler(void)
{
  isrCallback_t cb = callbacks[1];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI2
void EXTI2_IRQHandler(void); // suppress missing-declaration warning
void EXTI2_IRQHandler(void)
{
  isrCallback_t cb = callbacks[2];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI3
void EXTI3_IRQHandler(void); // suppress missing-declaration warning
void EXTI3_IRQHandler(void)
{
  isrCallback_t cb = callbacks[3];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI4
void EXTI4_IRQHandler(void); // suppress missing-declaration warning
void EXTI4_IRQHandler(void)
{
  isrCallback_t cb = callbacks[4];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI9_5
void EXTI9_5_IRQHandler(void); // suppress missing-declaration warning
void EXTI9_5_IRQHandler(void)
{
  isrCallback_t cb = callbacks[6];
  if (cb)
    cb();
}
#endif
#if IOC_USE_EXTI15_10
void EXTI15_10_IRQHandler(void); // suppress missing-declaration warning
void EXTI15_10_IRQHandler(void)
{
  isrCallback_t cb = callbacks[7];
  if (cb)
    cb();
}
#endif

#endif
