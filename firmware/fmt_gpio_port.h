/**
 * Interface for the gpio port implementations.  This gets consumed by 
 * Clients: fmt-gpio.c
 * Implementers: gpio_xmc4.c, gpio_spy.c
 * 
 * These functions are for firment's internal use.  fmt_gpio.h is the public 
 * interface.
 */

#include <fmt_gpio.h> // inputMode_t, outputMode_t, outLevel_t
#include <stdint.h>

void port_initInputPin(void *const port, uint8_t pin, inputMode_t mode);
void port_initOutputPin(void *const port, const uint8_t pin, outputMode_t mode);
void port_initSpiPins(void);
uint32_t port_readPort(void *const port);
void port_setPin(void *const port, uint_fast8_t pin, outLevel_t level);