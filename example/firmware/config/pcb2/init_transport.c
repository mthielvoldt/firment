/**
 * Concerns:
 * - Knows which pin is routed on PCB to which purpose (CTS, msgWait).
 * - Knows what role this chip plays on the spi bus (Main, sub).
 * - Communicates this information to the spi driver.
 */
#include <init_transport.h>
#include <fmt_uart.h>
#include "uart_pcbDetails.h" // replaces RTEDevice.h for ARM SPI driver
#include <priority.h>

extern ARM_DRIVER_USART Driver_USART3;

const uartCfg_t config = {
    .driverId = 3,
    .driver = &Driver_USART3,
    .baudHz = 115200,
    .irqPriority = FMT_TRANSPORT_PRIORITY,
};

bool project_initTransport(void)
{
  fmt_startTxChain = uart_startTxChain;
  fmt_linkTransport = uart_linkTransport;
  return fmt_initUart(&config);
}