#include <fmt_ioc.h>

#define MAINTAIN_INDEFINITELY -1

void iocTest_callCallback(uint8_t iocId);
void iocTest_setPinState(uint8_t iocId, bool newState);
void iocTest_sendPinPulse(uint8_t iocId, bool pulseState, int readCount);