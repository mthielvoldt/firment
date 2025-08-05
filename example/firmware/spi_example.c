
#include <fmt_rx.h>
#include <ghostProbe.h>
#include <fmt_periodic.h>
#include <fmt_sysInit.h>
#include "control.h"
#include "frequency.h"
#include "priority.h"
#include "project_comms.h"
#include <timer_pcbDetails.h>


static void periodicA(void);

int main(void)
{
  fmt_initSys();
  
  comm_init();

  // Set periodicA to 1kHz frequency.
  fmt_initPeriodic(PERIODIC_A_TIMER_ID, PERIODIC_A_PERIOD_US, 
    periodicA_priority, periodicA);


  ctl_init(WAVE_UPDATE_FREQ);
  gp_init(GHOST_PROBE_CALL_FREQ);

  for (uint32_t count = 0;;)
  {
    count++;
    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  - handle logging.
     *  - free memory accounting.
     */
  }
}

// 1kHz
static void periodicA(void)
{
  comm_handleTelemetry();
  fmt_handleRx();
  ctl_updateVoltageISR();
  gp_periodic();
}
