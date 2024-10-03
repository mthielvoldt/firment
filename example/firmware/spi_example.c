
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include <fmt_spi.h>
#include <port/fmt_periodic_xmc.h>
#include "control.h"
#include "example_comms.h"
#include "frequency.h"

#define periodicA CCU40_0_IRQHandler
#define periodicA_IRQn CCU40_0_IRQn
#define periodicA_priority 30
#define spiTxBuf_priority 25


// Todo: specify size of SendQueue, and pass to initSpi().
// Todo: specify HW resources for spi (channel, etc.)

int main(void)
{
  /* Port 3, pins:
  7:  MISO
  8:  MOSI
  9:  SCK
  10: CS: USIC2_CH0.SELO0 (ALT1 output)
  */
  spiCfg_t spiConfig = getDefaultSpiCfg();

  spiConfig.priority = spiTxBuf_priority;

  initFirment_spi(spiConfig);

  // Set periodicA to 1kHz frequency.
  initPeriodicISR(
      CCU40,
      CCU40_CC40,
      XMC_CCU4_SLICE_PRESCALER_128,
      PRESCALE_128_TICKS_IN_1MS,
      periodicA_IRQn,
      periodicA_priority);

  ctl_init(WAVE_UPDATE_FREQ);

  for (;;)
  {
    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  -
     */

  }
}

// 1kHz
void periodicA()
{
  genTelem_periodic();
  ctl_updateVoltageISR();
}
