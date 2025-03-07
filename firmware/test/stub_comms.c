#include "stub_comms.h"

static Top storedMsg = {0};
static bool toReturnOnSend = true;

bool fmt_sendMsg_test(Top message)
{
  storedMsg = message;
  return toReturnOnSend;
}

bool fmt_getMsg_test(Top *message)
{
  *message = storedMsg;
  return true;
}

/* Test-utilities */
void test_setNextSendReturn(bool toReturn) { toReturnOnSend = toReturn; }