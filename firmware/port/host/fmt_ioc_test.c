/** fmt_ioc_test.c
 * @file test stub for fmt_ioc.
 *
 * @warning Despite accepting activeEdges, this implementation ONLY behaves as
 * though EDGE_TYPE_RISING were set, regardless of what activeEdges is passed.
 */
#include "fmt_ioc_test.h"
#include <RTE_DeviceConfig.h>

const uint8_t availableIocs[] = AVAILABLE_IOCs;
const uint8_t enabledIsrCount =
    IOC_USE_ISR_0 + IOC_USE_ISR_1 + IOC_USE_ISR_2 + IOC_USE_ISR_3 +
    IOC_USE_ISR_4 + IOC_USE_ISR_5 + IOC_USE_ISR_6 + IOC_USE_ISR_7;

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
  if (iocId >= sizeof(availableIocs) || iocId >= enabledIsrCount)
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

  if (myIoc->pinState) // currently supports only rising-edge trigger.
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
  if (myIoc->iocEnabled && pulseState) // supports only rising-edge trigger
  {
    myIoc->callback();
  }
}