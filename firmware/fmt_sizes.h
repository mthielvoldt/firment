#pragma once
#include <stdint.h>

#define PREFIX_SIZE_BYTES 1U  // a prefix indicating subsequent message length,
#define CRC_SIZE_BYTES 2U
#define MAX_PACKET_SIZE_BYTES 64U // Must be less than 259; len sent in 1 byte.

/* in the next line, 14 breaks down as follows: 
4: count, 4 value, 3 for the 3 log types, 2 for top types, 1 prefix.*/
#define MAX_LOG_TEXT_SIZE (MAX_PACKET_SIZE_BYTES - 17)
#define SEND_QUEUE_LENGTH 4U
#define RX_QUEUE_LENGTH 9U
#define MAX_SENDER_PRIORITY 16U

#define MAX_MESSAGE_SIZE_BYTES (MAX_PACKET_SIZE_BYTES - PREFIX_SIZE_BYTES)
