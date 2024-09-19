#include "example_comms.h"
#include "fmt_spi.h"

// This must be a power of 2.
#define CALLS_PER_FULL_ROTATION 512U

void genTelem_periodic(void)
{
  static uint32_t count = 0;

  // execute 8 different operations, one each call in round-robin
  switch (++count & (CALLS_PER_FULL_ROTATION - 1))
  {
  case 0:
    sendMsg((const Top){
        .which_sub = Top_WaveformTlm_tag,
        .sub = {
            .WaveformTlm = {
                .currentMa = 5,
                .voltageV = 4.3}}});

    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    break;
  case 7:
    break;
  }
}