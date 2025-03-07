
#include "fmt_periodic_port.h"
#include "core_cm4.h" // NVIC

void initPeriodicISR(
  XMC_CCU4_MODULE_t *module,
  XMC_CCU4_SLICE_t *slice,
  XMC_CCU4_SLICE_PRESCALER_t prescaler,
  uint16_t periodTick,
  IRQn_Type IRQn,
  uint32_t priority)
{

  // XMC_CCU4_SLICE_EVENT_CONFIG_t eventCfg = {
  //     .duration = XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
  //     .edge = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_NONE,
  //     .level = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
  //     .mapped_input = XMC_CCU4_SLICE_INPUT_A};
  // XMC_CCU4_SLICE_ConfigureEvent(slice, XMC_CCU4_SLICE_EVENT_0, &eventCfg);

  XMC_CCU4_SLICE_COMPARE_CONFIG_t compareCfg = {
      .timer_mode = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
      .monoshot = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
      .shadow_xfer_clear = 0,
      .prescaler_mode = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,

  };

  XMC_CCU4_Init(module, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
  // XMC_CCU4_SetModuleClock(module, XMC_CCU4_CLOCK_SCU); // SCU is default.
  XMC_CCU4_SLICE_CompareInit(slice, &compareCfg);
  XMC_CCU4_SLICE_SetPrescaler(slice, prescaler);
  XMC_CCU4_SLICE_SetTimerPeriodMatch(slice, periodTick);

  // Trigger CCU40_0_IRQHandler() on Period match event.
  XMC_CCU4_SLICE_EnableEvent(slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  XMC_CCU4_SLICE_SetInterruptNode(
      slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);

  NVIC_SetPriority(IRQn, priority);
  NVIC_EnableIRQ(IRQn);

  // kick it off.
  XMC_CCU4_EnableClock(module, 0);
  XMC_CCU4_EnableShadowTransfer(module, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
  XMC_CCU4_SLICE_StartTimer(slice);
}