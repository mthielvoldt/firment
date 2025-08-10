/**
 * Provides 
 * - function pointers allowing transport to be swapped at run-time
 * - the appropriate init function based on FMT_USES_{SPI, UART}
 */

// This interface
#include "fmt_transport.h"

// Dependencies
#include <priority.h>

void (*fmt_startTxChain)(void) = NULL;
bool (*fmt_linkTransport)(queue_t *sendQueue, rxCallback_t rxCallback) = NULL;

#include <comm_pcbDetails.h>
#if defined(FMT_USES_SPI)

#include <fmt_spi.h>
#include <spi_pcbDetails.h> // replaces RTEDevice.h for ARM SPI driver
#include <ioc_pcbDetails.h>
#include <ioc_mcuDetails.h> // IOC_x_to_ISR_y mcu-supported routing options

extern ARM_DRIVER_SPI FMT_DRIVER;

spiCfg_t spiConfig = {
    .spiDriverId = FMT_DRIVER_ID,
    .spiModule = &FMT_DRIVER,
    .msgWaitingIocId = MSG_WAIT_IOC_ID, // IOC_14: P1_0 on XMC4700
    .msgWaitingIocOut = MSG_WAIT_IOC_OUT,
    .clearToSendIocId = CTS_IOC_ID, // IOC_4:  P0_4 on XMC4700
    .clearToSendIocOut = CTS_IOC_OUT,
    .baudHz = FMT_BAUD_HZ,
    .busMode = BUS_MODE_MAIN,
    .ssActiveLow = true,
    .irqPriority = FMT_TRANSPORT_PRIORITY,
};

bool fmt_initTransport(void)
{
  fmt_startTxChain = spi_startTxChain;
  fmt_linkTransport = spi_linkTransport;
  return fmt_initSpi(spiConfig);
}

#elif defined(FMT_USES_UART)

#include <fmt_uart.h>
#include "uart_pcbDetails.h" // replaces RTEDevice.h for ARM SPI driver

extern ARM_DRIVER_USART FMT_DRIVER;

const uartCfg_t config = {
    .driverId = FMT_DRIVER_ID,
    .driver = &FMT_DRIVER,
    .baudHz = FMT_BAUD_HZ,
    .irqPriority = FMT_TRANSPORT_PRIORITY,
};

bool fmt_initTransport(void)
{
  fmt_startTxChain = uart_startTxChain;
  fmt_linkTransport = uart_linkTransport;
  return fmt_initUart(&config);
}

#endif