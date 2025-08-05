#include "fmt_transport.h"

void (*fmt_startTxChain)(void) = NULL;
bool (*fmt_linkTransport)(queue_t *sendQueue, rxCallback_t rxCallback) = NULL;