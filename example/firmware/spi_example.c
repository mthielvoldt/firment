
#ifdef XMC4400_F100x512
#include "XMC4400.h"
#endif

#ifdef XMC4700_F144x2048
#include "XMC4700.h"
#endif

#include "fmt_spi.h"
#include "example_comms.h"


/** Initializes the following peripherals:
 * - GPIO
 * - SPI (USIC)
 * - NVIC
 */
int main(void)
{
  initFirment_spi();

  for (;;)
  {

    /** Do some intelligent housekeeping. For example:
     *  - record how much time I spend here as a cpu-idle metric.
     *  -
     */

    genTelem_periodic();
    for (int i = 0; i < 100000; i++)
      ;
  }
}
