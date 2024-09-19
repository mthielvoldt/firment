/** Comms module
 * Concerns:
 * - Executes system actions associated with valid inbound messages.
 * - Constructs and queues outbound messages.
 * - Does NOT do CRC validation - all inbound messages are assumed CRC-valid.
 *
 * Action types triggered by inbound messages:
 * - change a parameter (triggers next action)
 * - reporting value of requested parameter
 * - general-purpose command (responses vary)
 *
 * Actions types assocated with outbound messages:
 * - periodic telemetry (unrequested)
 * - conditional-triggered alerts
 */
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
  {
    sendMsg((const Top){
        .which_sub = Top_WaveformTlm_tag,
        .sub = {
            .WaveformTlm = {
                .currentMa = 5,
                .voltageV = 4.3}}});
    break;
  }
  case 20:
  {
    // sendMsg((const Top){
    //     .which_sub = ,
    //     .sub = {}});
    break;
  }
  case 40:
    break;
  case 60:
    break;
  }
}