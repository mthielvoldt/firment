#ifndef fmt_uart_port_H
#define fmt_uart_port_H

#include "fmt_uart.h"
#include <stdbool.h>
#include <stdint.h>

bool port_initUartModule(const uartCfg_t *config);

uint32_t port_getUartEventIRQn(uint8_t fmtUartId);

#endif