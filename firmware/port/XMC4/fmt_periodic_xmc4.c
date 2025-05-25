
#include <fmt_periodic.h>
#include <timer_pcbDetails.h>
#include "core_cm4.h" // NVIC
#include <xmc_ccu4.h> // timer

#ifdef XMC4700_F144x2048
#define SYS_FREQ 144000000ul
#else
#error "XMC4 mcu not supported."
#endif
const timerResource_t timerConfigs[] = AVAILABLE_TIMERS;

static bool storeCallback(IRQn_Type irqNumber, callback_t callback);

/** Configure a CCU4 Timer to drive an ISR at a fixed period.
 * Consumes a CCU4 timer timer.slice.  (There are 4 timer.slices per timer.module).
 * The Timer's clock runs at the sys clock frequency. (144MHz for XMC4700).
 * To determine the ISR call frequency: f_sys / prescale / periodTick
 * Example: 144MHz / 1024 prescale / 10000 period = 14.0625 Hz.
 */

bool fmt_initPeriodic(
    uint8_t timerId, uint32_t intervalUs, uint32_t priority, callback_t callback)
{
  if (timerId >= sizeof(timerConfigs) / sizeof(timerResource_t))
    return false;

  timerResource_t timer = timerConfigs[timerId];

  // Currently only supports exact frequencies up to 1kHz.
  XMC_CCU4_SLICE_PRESCALER_t prescaler = XMC_CCU4_SLICE_PRESCALER_128;
  uint16_t periodTick = (unsigned long long)(SYS_FREQ / 128U) * intervalUs / 1000000U;

  XMC_CCU4_SLICE_COMPARE_CONFIG_t compareCfg = {
      .timer_mode = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
      .monoshot = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
      .shadow_xfer_clear = 0,
      .prescaler_mode = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,

  };

  // Store the supplied callback, associating to the correct IRQ handler.
  if (!storeCallback(timer.irqNumber, callback))
    return false;

  XMC_CCU4_Init(timer.module, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
  // XMC_CCU4_SetModuleClock(timer.module, XMC_CCU4_CLOCK_SCU); // SCU is default.
  XMC_CCU4_SLICE_CompareInit(timer.slice, &compareCfg);
  XMC_CCU4_SLICE_SetPrescaler(timer.slice, prescaler);
  XMC_CCU4_SLICE_SetTimerPeriodMatch(timer.slice, periodTick);

  // Trigger CCU40_0_IRQHandler() on Period match event.
  XMC_CCU4_SLICE_EnableEvent(timer.slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  XMC_CCU4_SLICE_SetInterruptNode(
      timer.slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);

  NVIC_SetPriority(timer.irqNumber, priority);
  NVIC_EnableIRQ(timer.irqNumber);

  // kick it off.
  XMC_CCU4_EnableClock(timer.module, 0);
  XMC_CCU4_EnableShadowTransfer(timer.module, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
  XMC_CCU4_SLICE_StartTimer(timer.slice);
  return true;
}

#if 1
#if PERIODIC_USES_CCU40_0
callback_t CCU40_0_callback = NULL;
void CCU40_0_IRQHandler(void);
void CCU40_0_IRQHandler(void)
{
  if (CCU40_0_callback)
    CCU40_0_callback();
}
#endif
#if PERIODIC_USES_CCU40_1
callback_t CCU40_1_callback = NULL;
void CCU40_1_IRQHandler(void);
void CCU40_1_IRQHandler(void)
{
  if (CCU40_1_callback)
    CCU40_1_callback();
}
#endif
#if PERIODIC_USES_CCU40_2
callback_t CCU40_2_callback = NULL;
void CCU40_2_IRQHandler(void);
void CCU40_2_IRQHandler(void)
{
  if (CCU40_2_callback)
    CCU40_2_callback();
}
#endif
#if PERIODIC_USES_CCU40_3
callback_t CCU40_3_callback = NULL;
void CCU40_3_IRQHandler(void);
void CCU40_3_IRQHandler(void)
{
  if (CCU40_3_callback)
    CCU40_3_callback();
}
#endif
#if PERIODIC_USES_CCU41_0
callback_t CCU41_0_callback = NULL;
void CCU41_0_IRQHandler(void);
void CCU41_0_IRQHandler(void)
{
  if (CCU41_0_callback)
    CCU41_0_callback();
}
#endif
#if PERIODIC_USES_CCU41_1
callback_t CCU41_1_callback = NULL;
void CCU41_1_IRQHandler(void);
void CCU41_1_IRQHandler(void)
{
  if (CCU41_1_callback)
    CCU41_1_callback();
}
#endif
#if PERIODIC_USES_CCU41_2
callback_t CCU41_2_callback = NULL;
void CCU41_2_IRQHandler(void);
void CCU41_2_IRQHandler(void)
{
  if (CCU41_2_callback)
    CCU41_2_callback();
}
#endif
#if PERIODIC_USES_CCU41_3
callback_t CCU41_3_callback = NULL;
void CCU41_3_IRQHandler(void);
void CCU41_3_IRQHandler(void)
{
  if (CCU41_3_callback)
    CCU41_3_callback();
}
#endif
#if PERIODIC_USES_CCU42_0
callback_t CCU42_0_callback = NULL;
void CCU42_0_IRQHandler(void);
void CCU42_0_IRQHandler(void)
{
  if (CCU42_0_callback)
    CCU42_0_callback();
}
#endif
#if PERIODIC_USES_CCU42_1
callback_t CCU42_1_callback = NULL;
void CCU42_1_IRQHandler(void);
void CCU42_1_IRQHandler(void)
{
  if (CCU42_1_callback)
    CCU42_1_callback();
}
#endif
#if PERIODIC_USES_CCU42_2
callback_t CCU42_2_callback = NULL;
void CCU42_2_IRQHandler(void);
void CCU42_2_IRQHandler(void)
{
  if (CCU42_2_callback)
    CCU42_2_callback();
}
#endif
#if PERIODIC_USES_CCU42_3
callback_t CCU42_3_callback = NULL;
void CCU42_3_IRQHandler(void);
void CCU42_3_IRQHandler(void)
{
  if (CCU42_3_callback)
    CCU42_3_callback();
}
#endif
#if PERIODIC_USES_CCU43_0
callback_t CCU43_0_callback = NULL;
void CCU43_0_IRQHandler(void);
void CCU43_0_IRQHandler(void)
{
  if (CCU43_0_callback)
    CCU43_0_callback();
}
#endif
#if PERIODIC_USES_CCU43_1
callback_t CCU43_1_callback = NULL;
void CCU43_1_IRQHandler(void);
void CCU43_1_IRQHandler(void)
{
  if (CCU43_1_callback)
    CCU43_1_callback();
}
#endif
#if PERIODIC_USES_CCU43_2
callback_t CCU43_2_callback = NULL;
void CCU43_2_IRQHandler(void);
void CCU43_2_IRQHandler(void)
{
  if (CCU43_2_callback)
    CCU43_2_callback();
}
#endif
#if PERIODIC_USES_CCU43_3
callback_t CCU43_3_callback = NULL;
void CCU43_3_IRQHandler(void);
void CCU43_3_IRQHandler(void)
{
  if (CCU43_3_callback)
    CCU43_3_callback();
}
#endif
#if PERIODIC_USES_CCU80_0
callback_t CCU80_0_callback = NULL;
void CCU80_0_IRQHandler(void);
void CCU80_0_IRQHandler(void)
{
  if (CCU80_0_callback)
    CCU80_0_callback();
}
#endif
#if PERIODIC_USES_CCU80_1
callback_t CCU80_1_callback = NULL;
void CCU80_1_IRQHandler(void);
void CCU80_1_IRQHandler(void)
{
  if (CCU80_1_callback)
    CCU80_1_callback();
}
#endif
#if PERIODIC_USES_CCU80_2
callback_t CCU80_2_callback = NULL;
void CCU80_2_IRQHandler(void);
void CCU80_2_IRQHandler(void)
{
  if (CCU80_2_callback)
    CCU80_2_callback();
}
#endif
#if PERIODIC_USES_CCU80_3
callback_t CCU80_3_callback = NULL;
void CCU80_3_IRQHandler(void);
void CCU80_3_IRQHandler(void)
{
  if (CCU80_3_callback)
    CCU80_3_callback();
}
#endif
#if PERIODIC_USES_CCU81_0
callback_t CCU81_0_callback = NULL;
void CCU81_0_IRQHandler(void);
void CCU81_0_IRQHandler(void)
{
  if (CCU81_0_callback)
    CCU81_0_callback();
}
#endif
#if PERIODIC_USES_CCU81_1
callback_t CCU81_1_callback = NULL;
void CCU81_1_IRQHandler(void);
void CCU81_1_IRQHandler(void)
{
  if (CCU81_1_callback)
    CCU81_1_callback();
}
#endif
#if PERIODIC_USES_CCU81_2
callback_t CCU81_2_callback = NULL;
void CCU81_2_IRQHandler(void);
void CCU81_2_IRQHandler(void)
{
  if (CCU81_2_callback)
    CCU81_2_callback();
}
#endif
#if PERIODIC_USES_CCU81_3
callback_t CCU81_3_callback = NULL;
void CCU81_3_IRQHandler(void);
void CCU81_3_IRQHandler(void)
{
  if (CCU81_3_callback)
    CCU81_3_callback();
}
#endif
#endif

bool storeCallback(IRQn_Type irqNumber, callback_t callback)
{
  switch (irqNumber)
  {
#if PERIODIC_USES_CCU40_0
  case CCU40_0_IRQn:
  {
    CCU40_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU40_1
  case CCU40_1_IRQn:
  {
    CCU40_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU40_2
  case CCU40_2_IRQn:
  {
    CCU40_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU40_3
  case CCU40_3_IRQn:
  {
    CCU40_3_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU41_0
  case CCU41_0_IRQn:
  {
    CCU41_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU41_1
  case CCU41_1_IRQn:
  {
    CCU41_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU41_2
  case CCU41_2_IRQn:
  {
    CCU41_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU41_3
  case CCU41_3_IRQn:
  {
    CCU41_3_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU42_0
  case CCU42_0_IRQn:
  {
    CCU42_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU42_1
  case CCU42_1_IRQn:
  {
    CCU42_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU42_2
  case CCU42_2_IRQn:
  {
    CCU42_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU42_3
  case CCU42_3_IRQn:
  {
    CCU42_3_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU43_0
  case CCU43_0_IRQn:
  {
    CCU43_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU43_1
  case CCU43_1_IRQn:
  {
    CCU43_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU43_2
  case CCU43_2_IRQn:
  {
    CCU43_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU43_3
  case CCU43_3_IRQn:
  {
    CCU43_3_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU80_0
  case CCU80_0_IRQn:
  {
    CCU80_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU80_1
  case CCU80_1_IRQn:
  {
    CCU80_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU80_2
  case CCU80_2_IRQn:
  {
    CCU80_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU80_3
  case CCU80_3_IRQn:
  {
    CCU80_3_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU81_0
  case CCU81_0_IRQn:
  {
    CCU81_0_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU81_1
  case CCU81_1_IRQn:
  {
    CCU81_1_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU81_2
  case CCU81_2_IRQn:
  {
    CCU81_2_callback = callback;
    break;
  }
#endif
#if PERIODIC_USES_CCU81_3
  case CCU81_3_IRQn:
  {
    CCU81_3_callback = callback;
    break;
  }
#endif
  default:
  {
    return false;
  }
  }
  return true;
}