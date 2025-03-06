
#include "fmt_log.h"
#include "fmt_sizes.h"
#include "fmt_comms.h"

static logLevel_t activeLogLevel = LOG_VERBOSE;

bool fmt_sendLog(logLevel_t level, const char *msg, float number)
{
  static uint32_t logCount = 0;
  logCount++;
  bool success = true;

  if (level >= activeLogLevel) 
  {
    Log logMsg = {.count = logCount, .value = number};

    if (msg != NULL)
    {
      strncpy(logMsg.text, msg, MAX_LOG_TEXT_SIZE);
    }

    fmt_sendMsg((const Top){
        .which_sub = Top_Log_tag,
        .sub = {.Log = logMsg}});
  }
  else
  {
    success = false;
  }
  return success;
}

void fmt_setLogLevel(logLevel_t level)
{
  activeLogLevel = level;
}