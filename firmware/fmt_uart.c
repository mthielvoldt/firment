/** @file fmt_uart.c
 * @brief Middleware between fmt_comms and CMSIS Driver_USART.h.
 * Handles the sending and receiving, but not queueing or validation.
 *
 * Because UART doesn't have any separate framing signal (like SPI's sub select)
 * we need to provide a framing mechanism to recover from sync errors.
 *
 * [Start Code][length][... payload ...][CRC]
 *
 * The start code increases the chance that the next packet after losing sync
 * will get through.  Of course, the start code can appear in the data, so there
 * are cases where the next packet doesn't re-sync the frame.  A timeout is also
 * used to re-sync.
 *
 * Outbound (tx) messages have length prefix, so the count to transmit is
 * known at the time transmit starts, and can be passed to the CMSIS driver.
 *
 * When no packet is already in reception, we
 *
 */

// This file's interfaces
#include "fmt_uart.h"

// Dependencies in Firment
#include "fmt_assert.h"
#include "fmt_uart_frame.h" // this replaces fmt_sizes.h
#include <fmt_uart_port.h> // port_initUart() port_getUartEventIRQn()
#include <core_port.h>
#include <cmsis_gcc.h>

static queue_t *sendQueue = NULL;
static ARM_DRIVER_USART *uart = NULL;
static uint8_t rxPacket[UART_PACKET_SIZE] = {0};
static uint32_t rxErrorCount = 0;
static bool initialized = false;

static void uartEventHandlerISR(uint32_t event);
static inline bool rxErrors(uint32_t event);

bool fmt_initUart(const uartCfg_t *config)
{
  initialized = false;
  // This init starts transactions, so queue needs to be already initialized.
  if (!sendQueue)
    return false;

  uart = config->driver;
  uint32_t uartEventIRQn = port_getUartEventIRQn(config->driverId);

  ASSERT_SUCCESS(uartEventIRQn);
  ASSERT_SUCCESS(port_initUartModule(config));

  ASSERT_ARM_OK(uart->Initialize(uartEventHandlerISR));
  ASSERT_ARM_OK(uart->PowerControl(ARM_POWER_FULL));
  uint32_t modeControl =
      ARM_USART_MODE_ASYNCHRONOUS |
      ARM_USART_DATA_BITS_8 |
      ARM_USART_FLOW_CONTROL_NONE |
      ARM_USART_PARITY_NONE |
      ARM_USART_STOP_BITS_1;
  ASSERT_ARM_OK(uart->Control(modeControl, config->baudHz));

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), config->irqPriority, 0U);
  NVIC_SetPriority(uartEventIRQn, encodedPrio);

  rxParams_t rxParams = getStartCode();
  uart->Receive(&rxPacket[rxParams.position], rxParams.length);

  initialized = true;
  return true;
}

bool uart_linkTransport(queue_t *_sendQueue, rxCallback_t rxCallback)
{
  if (_sendQueue && rxCallback)
  {
    sendQueue = _sendQueue;
    setPacketReadyCallback(rxCallback);
    return true;
  }
  return false;
}

void uart_startTxChain(void)
{
  static uint8_t txPacket[UART_PACKET_SIZE] = START_CODE;

  bool ready = !uart->GetStatus().tx_busy;
  if (ready)
  {
    if (sendQueue && dequeueFront(sendQueue, &txPacket[LENGTH_POSITION]))
    {
      uart->Send(txPacket, getPacketLength(txPacket));
    }
  }
  // If uart is busy, EVENT_TX_COMPLETE will call this again when uart is ready.
  // So there is no need to handle that condition here; it's already handled.
}

/**
 * Called by the ARM_DRIVER.
 * @arg event is a bitfield of flags indicating what event(s) happened.
 *
 */
void uartEventHandlerISR(uint32_t event)
{
  bool eventHandled = false;

  if ((event & ARM_USART_EVENT_RECEIVE_COMPLETE) || rxErrors(event))
  {
    rxParams_t nextSegment;
    if (rxErrors(event))
    {
      rxErrorCount++;
      nextSegment = getStartCode();
      // __BKPT(0);
    }
    else
    {
      nextSegment = handleRxSegment(rxPacket);
    }
    eventHandled = true;
    uart->Receive(&rxPacket[nextSegment.position], nextSegment.length);
  }

  if (event & ARM_USART_EVENT_SEND_COMPLETE)
  {
    eventHandled = true;
    fmt_startTxChain();
  }

  // If this function was called for an unhandled reason, let's learn about it.
  if (!eventHandled)
    __BKPT(1);
}

static inline bool rxErrors(uint32_t event)
{
  return event & (ARM_USART_EVENT_RX_BREAK |
                  ARM_USART_EVENT_RX_FRAMING_ERROR |
                  ARM_USART_EVENT_RX_OVERFLOW |
                  ARM_USART_EVENT_RX_PARITY_ERROR |
                  ARM_USART_EVENT_RX_TIMEOUT);
}
