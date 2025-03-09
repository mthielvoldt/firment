
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "ISR_Config.h"
#include "config/spi_config.h"
#include <fmt_rx.h>
#include <ghostProbe.h>
#include <fmt_periodic_port.h>
#include "control.h"
#include "project_comms.h"
#include "frequency.h"
#include "version.h"

int main(void)
{
  project_initSpi();
  
  comm_init((portPin_t){.port = XMC_GPIO_PORT5, .pin = 9U});

  // Set periodicA to 1kHz frequency.
  initPeriodicISR(
      CCU40,
      CCU40_CC40,
      XMC_CCU4_SLICE_PRESCALER_128,
      PRESCALE_128_TICKS_IN_1MS,
      periodicA_IRQn,
      periodicA_priority);

  ctl_init(WAVE_UPDATE_FREQ);
  gp_init(GHOST_PROBE_CALL_FREQ);

  for (;;)
  {
    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  - handle logging.
     *  - free memory accounting.
     */
  }
}

// 1kHz
void periodicA()
{
  comm_handleTelemetry();
  fmt_handleRx();
  ctl_updateVoltageISR();
  gp_periodic();
}
