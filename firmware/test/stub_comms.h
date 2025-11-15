#include <fmt_comms.h>

bool fmt_getMsg_test(Top *message);
bool fmt_sendMsg_test(Top message);

void test_resetComms(void);
void test_setNextSendReturn(bool toReturn);
uint32_t test_getSendCount(void);