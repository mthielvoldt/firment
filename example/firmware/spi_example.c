
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "fmt_spi.h"
#include "example_comms.h"
#include "core_cm4.h" // NVIC
#include "xmc_ccu4.h" // timer

/** Initializes the following peripherals:
 * - GPIO
 * - SPI (USIC)
 * - NVIC
 */
void initNVIC()
{
  XMC_CCU4_MODULE_t *module = CCU40;
  XMC_CCU4_SLICE_t *slice = CCU40_CC40;
  uint16_t period = 20000;

  // XMC_CCU4_SLICE_EVENT_CONFIG_t eventCfg = {
  //     .duration = XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
  //     .edge = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_NONE,
  //     .level = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
  //     .mapped_input = XMC_CCU4_SLICE_INPUT_A};
  // XMC_CCU4_SLICE_ConfigureEvent(slice, XMC_CCU4_SLICE_EVENT_0, &eventCfg);

  XMC_CCU4_SLICE_COMPARE_CONFIG_t compareCfg = {
      .timer_mode = 0,
      .prescaler_mode = 0,
  };

  XMC_CCU4_Init(module, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
  XMC_CCU4_SLICE_CompareInit(slice, &compareCfg);
  XMC_CCU4_SLICE_SetTimerPeriodMatch(slice, period);
  XMC_CCU4_SetModuleClock(module, XMC_CCU4_CLOCK_SCU);

  XMC_CCU4_SLICE_EnableEvent(slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);

  // what is an sr id (0-3)?
  XMC_CCU4_SLICE_SetInterruptNode(
      slice, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);

  NVIC_SetPriority(CCU40_0_IRQn, 30);
  NVIC_EnableIRQ(CCU40_0_IRQn);

  // kick it off.
  XMC_CCU4_EnableClock(module, 0);

  XMC_CCU4_EnableShadowTransfer(module, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
  XMC_CCU4_SLICE_SetPrescaler(slice, XMC_CCU4_SLICE_PRESCALER_1024);
  XMC_CCU4_StartPrescaler(module);
  XMC_CCU4_SLICE_StartTimer(slice);
}

int main(void)
{
  initFirment_spi();
  initNVIC();

  for (;;)
  {

    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  -
     */

    ISR_handleTx_spi();
  }
}

void CCU40_0_IRQHandler()
{
  genTelem_periodic();
}
