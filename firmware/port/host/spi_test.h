/**
 * This is currently a spy on its way to a fake.
 */

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

 int32_t getCallCount(fnType_t fnType);
 void spiTest_reset(void);
 void spiTest_queueIncoming(const void *data, size_t size);
