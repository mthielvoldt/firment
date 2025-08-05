#ifndef fmt_uart_frame_H
#define fmt_uart_frame_H

#include "fmt_transport.h"  // rxCallback_t
#include "fmt_sizes.h"
#include <stdint.h>

#define START_CODE_SIZE 2U
#define START_CODE {0xBE, 0xAD}
#define START_CODE_POSITION 0

// Must come after #include "fmt_sizes.h"  start code is a special uart feature.
#undef LENGTH_POSITION
#define LENGTH_POSITION (0 + START_CODE_SIZE)
#undef PAYLOAD_POSITION
#define PAYLOAD_POSITION (LENGTH_POSITION + LENGTH_SIZE_BYTES)

#define UART_PACKET_SIZE (MAX_PACKET_SIZE_BYTES + START_CODE_SIZE)


typedef struct rxParams_s
{
  uint32_t position;
  uint32_t length;
} rxParams_t;


void setPacketReadyCallback(rxCallback_t rxCallback);

rxParams_t handleRxSegment(const uint8_t *packet);

rxParams_t getStartCode(void);

inline static uint32_t getPacketLength(const uint8_t *packet)
{
  // Immediately convert to start-code aware reference frame.
  uint32_t crcPosition =
      getCRCPosition(&packet[LENGTH_POSITION]) + LENGTH_POSITION;

  // crcPosition is another name for the length of everything before the CRC.
  return (crcPosition + CRC_SIZE_BYTES);
}


#endif // fmt_uart_frame_H