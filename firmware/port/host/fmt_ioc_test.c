/** fmt_ioc_test.c
 * @file test stub for fmt_ioc.
 */
#include "fmt_ioc_test.h"

typedef void (*callback_t)(void);
callback_t callbacks[UINT8_MAX+1] = {0};
bool pinStates[UINT8_MAX+1] = {false};

bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  callbacks[iocId] = callback;
  return true;
}

void fmt_enableIoc(uint8_t iocId)
{
}

void fmt_disableIoc(uint8_t iocId)
{
}

bool fmt_getIocPinState(uint8_t iocId)
{
  return pinStates[iocId];
}

void test_iocCallCallback(uint8_t iocId)
{
  callbacks[iocId]();
}

void test_iocSetPinState(uint8_t iocId, bool newState)
{
  pinStates[iocId] = newState;
}