/** This file generated based on .proto file. */
#include <project_comms.h>

void fmt_handleRx(void)
{
  Top incomingMessage;
  if (getMsg(&incomingMessage))
  {
    switch (incomingMessage.which_sub)
    {
    case Top_Ack_tag:
    {
      Ack subMsg = incomingMessage.sub.Ack;
      break;
    }

#ifdef USE_WAVEFORM_CTL
    case Top_WaveformCtl_tag:
      handleWaveformCtl(incomingMessage.sub.WaveformCtl);
      break;
#endif

    default:
    }
  }
}