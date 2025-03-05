
#include <messages.pb.h>
/** fmt_sendMsg
 * Queues a message for transmit.
 * Should only be called from a single context - not thread-safe.
 * This function has implementations in: 
 * - fmt_spi.c
 * - (more to come)
 */
bool fmt_sendMsg(Top message);

bool fmt_getMsg(Top *message);