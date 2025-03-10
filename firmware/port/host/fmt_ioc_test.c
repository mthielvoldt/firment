/** fmt_ioc_test.c
 * @file test stub for fmt_ioc.
 * 
 * @warning Despite accepting activeEdges, this implementation ONLY behaves as
 * though EDGE_TYPE_RISING were set, regardless of what activeEdges is passed.
 */
#include "fmt_ioc_test.h"
#include "Device_IOC.h"
#include <RTE_DeviceConfig.h>

// This table enforces the requirement that USE_IOCn must be defined in order
// to init the ioc with iocId = n.
const uint8_t usableIocs[RTE_IOC_COUNT] = {
  #ifdef USE_IOC0
    [0] = 1,
  #endif
  #ifdef USE_IOC1
    [1] = 1,
  #endif
  #ifdef USE_IOC2
    [2] = 1,
  #endif
  #ifdef USE_IOC3
    [3] = 1,
  #endif
  #ifdef USE_IOC4
    [4] = 1,
  #endif
  #ifdef USE_IOC5
    [5] = 1,
  #endif
  #ifdef USE_IOC6
    [6] = 1,
  #endif
  #ifdef USE_IOC7
    [7] = 1,
  #endif
  #ifdef USE_IOC8
    [8] = 1,
  #endif
  #ifdef USE_IOC9
    [9] = 1,
  #endif
  #ifdef USE_IOC10
   [10] = 1,
  #endif
  #ifdef USE_IOC11
   [11] = 1,
  #endif
  #ifdef USE_IOC12
   [12] = 1,
  #endif
  #ifdef USE_IOC13
   [13] = 1,
  #endif
  #ifdef USE_IOC14
   [14] = 1,
  #endif
  #ifdef USE_IOC15
   [15] = 1,
  #endif
  #ifdef USE_IOC16
   [16] = 1,
  #endif
  #ifdef USE_IOC17
   [17] = 1,
  #endif
  #ifdef USE_IOC18
   [18] = 1,
  #endif
  #ifdef USE_IOC19
   [19] = 1,
  #endif
  #ifdef USE_IOC20
   [20] = 1,
  #endif
  #ifdef USE_IOC21
   [21] = 1,
  #endif
  #ifdef USE_IOC22
   [22] = 1,
  #endif
  #ifdef USE_IOC23
   [23] = 1,
  #endif
  #ifdef USE_IOC24
   [24] = 1,
  #endif
  #ifdef USE_IOC25
   [25] = 1,
  #endif
  #ifdef USE_IOC26
   [26] = 1,
  #endif
  #ifdef USE_IOC27
   [27] = 1,
  #endif
  #ifdef USE_IOC28
   [28] = 1,
  #endif
  #ifdef USE_IOC29
   [29] = 1,
  #endif
  #ifdef USE_IOC30
   [30] = 1,
  #endif
  #ifdef USE_IOC31
   [31] = 1,
  #endif
  #ifdef USE_IOC32
   [32] = 1,
  #endif
  #ifdef USE_IOC33
   [33] = 1,
  #endif
  };

#define NUM_PINS (UINT8_MAX + 1)

typedef void (*callback_t)(void);

typedef struct ioc_s
{
  bool pinState;
  bool iocEnabled;
  int readsUntilToggle;
  callback_t callback;
} ioc_t;

ioc_t ioc[NUM_PINS] = {0};

bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  if (!usableIocs[iocId] != 0)
    return false;
    
  ioc[iocId].readsUntilToggle = MAINTAIN_INDEFINITELY;
  ioc[iocId].callback = callback;
  ioc[iocId].iocEnabled = true;
  return true;
}

void fmt_enableIoc(uint8_t iocId)
{
  ioc[iocId].iocEnabled = true;
}

void fmt_disableIoc(uint8_t iocId)
{
  ioc[iocId].iocEnabled = false;
}

static void togglePinWithCallback(ioc_t *myIoc)
{
  myIoc->pinState ^= 1; // toggle
  if (!myIoc->iocEnabled)
    return;
  
  if (myIoc->pinState)  // currently supports only rising-edge trigger.
  {
    myIoc->callback();
  }
}

bool fmt_getIocPinState(uint8_t iocId)
{
  ioc_t *myIoc = &ioc[iocId];

  if (myIoc->readsUntilToggle >= 0)
  {
    if (myIoc->readsUntilToggle == 0)
    {
      togglePinWithCallback(myIoc);
    }
    myIoc->readsUntilToggle--;
  }
  return myIoc->pinState;
}

void iocTest_callCallback(uint8_t iocId)
{
  ioc[iocId].callback();
}

void iocTest_setPinState(uint8_t iocId, bool newState)
{
  ioc[iocId].pinState = newState;
}

/** sendPinPulse
 * A pulse is a callback trigger followed by a set number of reads through which
 * to maintain the same pin state, after which subsequent reads return the
 * opposite state.
 * @param iocId
 * @param pulseState true will mean the pin will read as active.
 * @param readCount the number of reads that will return the pulseState.  After
 * this number of reads, subsequent reads will return !pulseState.  A negative
 * value will induce reads to return pulseState indefinitely.
 */
void iocTest_sendPinPulse(uint8_t iocId, bool pulseState, int readCount)
{
  ioc_t *myIoc = &ioc[iocId];
  myIoc->readsUntilToggle = readCount;
  myIoc->pinState = pulseState;
  if (myIoc->iocEnabled && pulseState)  // supports only rising-edge trigger
  {
    myIoc->callback();
  }
}