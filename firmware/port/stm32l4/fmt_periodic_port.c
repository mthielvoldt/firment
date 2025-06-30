
#include <fmt_periodic.h>
#include <timer_pcbDetails.h>
#include <stm32l4xx_hal.h> // NVIC
#include <stm32l4xx_hal_tim.h>
#include <stm32l4xx_hal_tim_ex.h>

const timerResource_t timerConfigs[] = AVAILABLE_TIMERS;

bool enableTimerAndStoreCallback(IRQn_Type irqNumber, callback_t callback);

bool fmt_initPeriodic(
    uint8_t timerId, uint32_t intervalUs, uint32_t priority, callback_t callback)
{
  if (timerId >= sizeof(timerConfigs) / sizeof(timerResource_t))
    return false;

  timerResource_t timer = timerConfigs[timerId];

  // Store the supplied callback, associating to the correct IRQ handler.
  bool success = enableTimerAndStoreCallback(timer.irqNumber, callback);
  if (!success)
    return false;

  TIM_HandleTypeDef config[] = {{
      .Instance = timer.base,
      .Channel = HAL_TIM_ACTIVE_CHANNEL_1,
      .Init = {
          .Prescaler = (8 - 1), // (80 MHz) / 8 = 10MHz
          .CounterMode = TIM_COUNTERMODE_UP,
          .Period = (10000 - 1), // 10MHz / 10k = 1kHz
          .ClockDivision = TIM_CLOCKDIVISION_DIV1,
          .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE,
      },
  }};
  HAL_StatusTypeDef status = HAL_TIM_Base_Init(config);
  if (status != HAL_OK)
    return false;

  TIM_ClockConfigTypeDef clock[] = {{
      .ClockFilter = 0,
      .ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED,
      .ClockPrescaler = TIM_CLOCKPRESCALER_DIV1,
      .ClockSource = TIM_CLOCKSOURCE_INTERNAL,
  }};
  status = HAL_TIM_ConfigClockSource(config, clock);
  if (status != HAL_OK)
    return false;

  TIM_MasterConfigTypeDef syncConfig[] = {{
      .MasterOutputTrigger = TIM_TRGO_RESET,
      .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE,
  }};
  status = HAL_TIMEx_MasterConfigSynchronization(config, syncConfig);
  if (status != HAL_OK)
    return false;

  HAL_NVIC_SetPriority(timer.irqNumber, priority, 0);
  HAL_NVIC_EnableIRQ(timer.irqNumber);

  // kick it off.
  HAL_TIM_Base_Start_IT(config);
  return true;
}

#define TIM_IRQ_HANDLER(n)                                    \
  callback_t TIM##n##_callback = NULL;                        \
  void TIM##n##_IRQHandler(void);                             \
  void TIM##n##_IRQHandler(void)                              \
  {                                                           \
    if (TIM##n##_callback)                                    \
      TIM##n##_callback();                                    \
    TIM##n->SR = ~TIM_FLAG_UPDATE; /*clear the update flag.*/ \
  }

#if PERIODIC_USES_TIM2
// TIM2_IRQHandler
TIM_IRQ_HANDLER(2)
#endif

#if PERIODIC_USES_TIM3
// TIM3_IRQHandler
TIM_IRQ_HANDLER(3)
#endif

#if PERIODIC_USES_TIM4
// TIM4_IRQHandler
TIM_IRQ_HANDLER(4)
#endif

#if PERIODIC_USES_TIM5
// TIM5_IRQHandler
TIM_IRQ_HANDLER(5)
#endif

#define ENABLE_AND_STORE_CALLBACK(n) \
  case TIM##n##_IRQn:                \
    TIM##n##_callback = callback;    \
    __HAL_RCC_TIM##n##_CLK_ENABLE(); \
    break;

bool enableTimerAndStoreCallback(IRQn_Type irqNumber, callback_t callback)
{
  switch (irqNumber)
  {
#if PERIODIC_USES_TIM2
    ENABLE_AND_STORE_CALLBACK(2)
#endif
#if PERIODIC_USES_TIM3
    ENABLE_AND_STORE_CALLBACK(3)
#endif
#if PERIODIC_USES_TIM4
    ENABLE_AND_STORE_CALLBACK(4)
#endif
#if PERIODIC_USES_TIM5
    ENABLE_AND_STORE_CALLBACK(5)
#endif
  default:
    return false;
  }
  return true;
}