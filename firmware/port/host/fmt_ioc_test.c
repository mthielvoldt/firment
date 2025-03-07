/** fmt_ioc_test.c
 * @file test stub for fmt_ioc.
 */
#include <fmt_ioc.h>

bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
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
  return false;
}