#ifndef fmt_uart_H
#define fmt_uart_H

#include <Driver_USART.h> // CMSIS generic header
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
  uint8_t driverId;
  ARM_DRIVER_USART *driver;
  uint32_t baudHz;
  uint32_t irqPriority;
} uartCfg_t;

bool fmt_initUart(const uartCfg_t *config);


#endif