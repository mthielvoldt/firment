
#include <fmt_periodic.h>
#include <timer_pcbDetails.h>
#include "core_cm4.h" // NVIC
#include <stm32l4xx_hal_tim.h>

const timerResource_t timerConfigs[] = AVAILABLE_TIMERS;

bool fmt_initPeriodic(
    uint8_t timerId, uint32_t intervalUs, uint32_t priority, callback_t callback)
{
  if (timerId >= sizeof(timerConfigs) / sizeof(timerResource_t))
    return false;

  timerResource_t timer = timerConfigs[timerId];

  TIM_HandleTypeDef config[1] = {{
      .Instance = timer.base,
      .Channel = HAL_TIM_ACTIVE_CHANNEL_1,
      .Init = {
          .Prescaler = 8,
          .CounterMode = TIM_COUNTERMODE_UP,
          .Period = 0,
          .ClockDivision = TIM_CLOCKDIVISION_DIV1,
      },
  }};
  TIM_ClockConfigTypeDef clock[1] = {{
    .ClockFilter = 0,
    .ClockPolarity = 0,
    .ClockPrescaler = TIM_CLOCKPRESCALER_DIV1,
    .ClockSource = TIM_CLOCKSOURCE_INTERNAL,
  }};

  HAL_TIM_Base_Init(config);
  
  NVIC_SetPriority(timer.irqNumber, priority);
  NVIC_EnableIRQ(timer.irqNumber);
  
  // kick it off.
  HAL_TIM_Base_Start_IT(config);
  return true;
}