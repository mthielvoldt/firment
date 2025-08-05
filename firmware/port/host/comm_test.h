/** @file comm_test.h
 * additional test utils for CMSIS drivers defined in arm_transport_drivers.c.
 * This file combines with the standard CMSIS driver headers to comprise the
 * interface to arm_transport_drivers.c
 */

 #include <fmt_sizes.h>
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
 void commTest_reset(void);
 void commTest_queueIncoming(const void *data);
 const uint8_t* commTest_getLastSent(void);
