#include "stub_comms.h"

#define SEND_QUEUE_LENGTH 10

static Top storedMsgs[SEND_QUEUE_LENGTH] = {0};
static unsigned storedCount = 0;
static bool toReturnOnSend = true;
static uint32_t sendCount = 0U;

bool fmt_sendMsg_test(Top message)
{
  sendCount++;
  if (storedCount < SEND_QUEUE_LENGTH)
  {
    storedMsgs[storedCount] = message;
    storedCount++;
    return toReturnOnSend;
  }
  return false;
}

bool fmt_getMsg_test(Top *message)
{
  if (storedCount > 0)
  {
    *message = storedMsgs[--storedCount];
    return true;
  }
  return false;
}

/* Test-utilities */
void test_resetComms(void) { 
  storedCount = 0U;
  toReturnOnSend = true;
  sendCount = 0U;
}

void test_setNextSendReturn(bool toReturn) { toReturnOnSend = toReturn; }

uint32_t test_getSendCount(void) { return sendCount; }
