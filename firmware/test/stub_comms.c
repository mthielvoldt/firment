#include "stub_comms.h"

static Top storedMsg = {0};
static bool toReturnOnSend = false;

bool fmt_sendMsg(Top message)
{
  storedMsg = message;
  return toReturnOnSend;
}

bool fmt_getMsg(Top *message)
{
  *message = storedMsg;
  return true;
}

void test_setNextSendReturn(bool toReturn) { toReturnOnSend = toReturn; }