/**
 * This is currently a spy on its way to a fake.
 */

 #include <fmt_sizes.h>
 #include <Driver_SPI.h>
 #include <stdint.h>

typedef enum fnType_e
{
  TRANSFER = 0,
  CONTROL,
  INITIALIZE,
  UNINITIALIZE,
  POWER_CONTROL,
  GET_STATUS,
  FUNCTION_TYPE_COUNT
} fnType_t;

// typedef struct sendStatus_s {
//   uint8_t lastFromTarget[MAX_PACKET_SIZE_BYTES];
//   uint32_t transferCount;
// } sendStatus_t;

 int32_t getCallCount(fnType_t fnType);
 void spiTest_reset(void);
 void spiTest_queueIncoming(const void *data);
 const uint8_t* spiTest_getLastSent(void);
