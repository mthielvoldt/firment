
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include <fmt_spi.h>
#include <port/fmt_periodic_xmc.h>
#include "example_comms.h"

#define periodicA CCU40_0_IRQHandler
#define periodicA_IRQn CCU40_0_IRQn
#define periodicA_priority 30

int main(void)
{
  initFirment_spi();
  initPeriodicISR(
      CCU40,
      CCU40_CC40,
      XMC_CCU4_SLICE_PRESCALER_1024,
      10000,
      periodicA_IRQn,
      periodicA_priority);

  for (;;)
  {
    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  -
     */

    ISR_handleTx_spi();
  }
}

void periodicA()
{
  genTelem_periodic();
}
