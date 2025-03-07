
#include <messages.pb.h>
/** fmt_sendMsg
 * Queues a message for transmit.
 * Should only be called from a single context - not thread-safe.
 * These function pointers find implementations in: 
 * - fmt_spi.c
 * - stub_comms.c (test double)
 */
extern bool (*fmt_sendMsg)(Top message);

extern bool (*fmt_getMsg)(Top *message);