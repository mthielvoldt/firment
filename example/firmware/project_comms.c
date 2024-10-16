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
#include "project_comms.h"
#include "control.h"

// This must be a power of 2.
#define CALLS_PER_FULL_ROTATION 1024U

// optimization: stop storing the config.
static portPin_t led = {0};

void comm_init(portPin_t ledPin)
{
  led = ledPin;
  XMC_GPIO_Init(led.port, led.pin, &gpOutPinConfig);
}

void comm_handleTelemetry(void)
{
  static uint32_t count = 0;
  static uint32_t rotations = 0;

  // execute 8 different operations, one each call in round-robin
  switch (++count & (CALLS_PER_FULL_ROTATION - 1))
  {
  case 0:
  {
    rotations++;
    telem_t telem = ctl_getTelem();
    sendMsg((const Top){
        .which_sub = Top_WaveformTlm_tag,
        .sub = {
            .WaveformTlm = {
                .currentMa = telem.currentMa,
                .voltageV = telem.voltage}}});

    // sendMsg((const Top){
    //     .which_sub = Top_WaveformTlm_tag,
    //     .sub = {
    //         .WaveformTlm = {
    //             .currentMa = telem.currentMa,
    //             .voltageV = telem.voltage}}});

    // XMC_GPIO_ToggleOutput(led.port, led.pin);
    break;
  }
  case 20:
  {
    sendMsg((const Top){
        .which_sub = Top_Log_tag,
        .sub = {
            .Log = {
                .text = "This message is a test.",
                .value = rotations}}});
    break;
  }
  case 40:
    break;
  case 60:
    break;
  }
}

void handleWaveformCtl(WaveformCtl msg)
{
  XMC_GPIO_ToggleOutput(led.port, led.pin);
}