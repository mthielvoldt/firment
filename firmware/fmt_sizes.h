#pragma once
#include <stdint.h>

#define PREFIX_SIZE_BYTES 1U  // a prefix indicating subsequent message length,
#define CRC_SIZE_BYTES 2U
#define MAX_PACKET_SIZE_BYTES 64U // Must be less than 259; len sent in 1 byte.

/* in the next line, 14 breaks down as follows: 
4: count, 4 value, 3 for the 3 log types, 2 for top types, 1 prefix.*/
#define MAX_LOG_TEXT_SIZE (MAX_PACKET_SIZE_BYTES - 17)
#define SEND_QUEUE_LENGTH 4U
#define MAX_SENDER_PRIORITY 16U

#define MAX_MESSAGE_SIZE_BYTES (MAX_PACKET_SIZE_BYTES - PREFIX_SIZE_BYTES)

/** Get the position of the CRC in bytes from the first element of the packet.
 * CRC position must be 16-bit aligned (even number) for hardware CRC engines. 
 * so if the length of the buffer (including length prefix,) is odd, there 
 * will be a byte of padding, between the buffer and the CRC.
 * The value of the padding byte (if present) will be checked by the CRC, but it
 * has no effect on the decoded message.
 */ 
static inline uint32_t getCRCPosition(uint8_t *lengthPrefixedBuffer)
{ 
  return ((lengthPrefixedBuffer[0] + PREFIX_SIZE_BYTES + 1) >> 1) << 1;
}