
#define HEADER_SIZE_BYTES 1U  // contains message length
#define CRC_SIZE_BYTES 2U
#define MAX_PACKET_SIZE_BYTES 64U // Must be less than 259; len sent in 1 byte.
#define SEND_QUEUE_LENGTH 4U
#define MAX_SENDER_PRIORITY 16U

#define MAX_MESSAGE_SIZE_BYTES (MAX_PACKET_SIZE_BYTES - HEADER_SIZE_BYTES)