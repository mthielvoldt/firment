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
#include <fmt_log.h>
#include <fmt_flash.h>
#include <fmt_gpio.h>
#include "config/gpio_pcbDetails.h"
#include <core_port.h>  // NVIC_SystemReset()

#define CALLS_PER_FULL_ROTATION 1000U


bool comm_init(void)
{
  bool success = true;
  success &= fmt_initGpioOutPin(LED_0_PIN_ID, OUTPUT_MODE_PUSH_PULL);
  success &= fmt_initGpioOutPin(LED_1_PIN_ID, OUTPUT_MODE_PUSH_PULL);
  return success;
}

void comm_handleTelemetry(void)
{
  static uint32_t count = 0;
  static uint32_t rotations = 0;

  // execute periodic telemetry sends in round-robin.
  switch (count++)
  {
  case 0:
  {
    rotations++;
    fmt_setPin(LED_1_PIN_ID, OUTPUT_TOGGLE);

    telem_t telem = ctl_getTelem();
    fmt_sendMsg((const Top){
        .which_sub = Top_WaveformTlm_tag,
        .sub = {
            .WaveformTlm = {
                .currentMa = telem.currentMa,
                .voltageV = telem.voltage}}});

    break;
  }
  case 200:
  {
    // fmt_sendLog(LOG_INFO, "A test. ", rotations);
    break;
  }
  case 400:
    break;
  case CALLS_PER_FULL_ROTATION:
    count = 0;
    break;
  }
}

void handleWaveformCtl(WaveformCtl msg)
{
  static float count = 0;
  count++;
  fmt_setPin(LED_0_PIN_ID, OUTPUT_TOGGLE);
  fmt_sendLog(LOG_INFO, "WaveformCtl rx cnt: ", count);

  const waveShape_t toControlShape[] = {
      [WaveShape_UNSPECIFIED] = WAVE_SHAPE_DC,
      [WaveShape_SINE] = WAVE_SHAPE_SINE,
      [WaveShape_SQUARE] = WAVE_SHAPE_SQUARE,
      [WaveShape_SAWTOOTH] = WAVE_SHAPE_SAWTOOTH,
      [WaveShape_DC] = WAVE_SHAPE_DC,
  };
  // opportunity here to limit input and notify if command exceeds limits.
  const waveCfg_t cfg = {
      .id = msg.channel,
      .amplitudeV = msg.enabled ? msg.amplitudeV : 0.0F,
      .frequencyHz = msg.frequencyHz,
      .offsetV = msg.enabled ? msg.offsetV : 0.0F,
      .shape = toControlShape[msg.shape]};
  ctl_setWaveform(cfg);
}

void handleReset(Reset msg)
{
  NVIC_SystemReset();
}
