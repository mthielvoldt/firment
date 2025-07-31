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
#include <comm_pcbDetails.h> // FMT_USES_<transport>
#ifdef FMT_USES_UART         // gates this whole file.

// This file's interfaces
#include "fmt_uart.h"      // UART-specifc interface for init.
#include "fmt_transport.h" // This file's transport-generic interface

// Dependencies in Firment
#include "assert.h"
#include "fmt_sizes.h"
#include "queue.h"
#include <fmt_uart_port.h> // port_initUart() port_getUartEventIRQn()
#include <core_port.h>
#include <cmsis_gcc.h>

#define START_CODE_SIZE 1U
#define START_CODE 0xBE
#define START_CODE_POSITION 0

// Must come after #include "fmt_sizes.h"  start code is a special uart feature.
#undef LENGTH_POSITION
#define LENGTH_POSITION (0 + START_CODE_SIZE)
#undef PAYLOAD_POSITION
#define PAYLOAD_POSITION (LENGTH_POSITION + LENGTH_SIZE_BYTES)

#define UART_PACKET_SIZE (MAX_PACKET_SIZE_BYTES + START_CODE_SIZE)

static queue_t *sendQueue = NULL;
static rxCallback_t rxCallback = NULL;
static ARM_DRIVER_USART *uart = NULL;
static uint8_t rxPacket[UART_PACKET_SIZE] = {0};
static uint32_t rxErrorCount = 0;

static void uartEventHandlerISR(uint32_t event);
static void handleRx(void);
static bool lengthValid(void);
static void getStartCode(void);
static void getLengthPrefix(void);
static void getPayload(void);
static inline uint32_t getPacketLength(const uint8_t *packet);

bool fmt_initUart(const uartCfg_t *config)
{
  uart = config->driver;
  uint32_t uartEventIRQn = port_getUartEventIRQn(config->driverId);

  ASSERT_SUCCESS(uartEventIRQn);
  ASSERT_SUCCESS(port_initUartModule(config));

  ASSERT_ARM_OK(uart->Initialize(uartEventHandlerISR));
  ASSERT_ARM_OK(uart->PowerControl(ARM_POWER_FULL));

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), config->irqPriority, 0U);
  NVIC_SetPriority(uartEventIRQn, encodedPrio);

  uint32_t modeControl =
      ARM_USART_MODE_ASYNCHRONOUS |
      ARM_USART_DATA_BITS_8 |
      ARM_USART_FLOW_CONTROL_NONE |
      ARM_USART_PARITY_NONE |
      ARM_USART_STOP_BITS_1;
  ASSERT_ARM_OK(uart->Control(modeControl, config->baudHz));

  return true;
}

bool fmt_linkTransport(queue_t *_sendQueue, rxCallback_t _rxCallback)
{
  if (_sendQueue && _rxCallback)
  {
    sendQueue = _sendQueue;
    rxCallback = _rxCallback;
    return true;
  }
  return false;
}

void fmt_startTxChain(void)
{
  static uint8_t txPacket[UART_PACKET_SIZE] = {START_CODE};

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
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
  {
    eventHandled = true;
    handleRx();
  }

  if (event & ARM_USART_EVENT_SEND_COMPLETE)
  {
    eventHandled = true;
    fmt_startTxChain();
  }

  if (event & (ARM_USART_EVENT_RX_BREAK |
               ARM_USART_EVENT_RX_FRAMING_ERROR |
               ARM_USART_EVENT_RX_OVERFLOW |
               ARM_USART_EVENT_RX_PARITY_ERROR |
               ARM_USART_EVENT_RX_TIMEOUT))
  {
    eventHandled = true;
    rxErrorCount++;
    getStartCode();
    __BKPT(0);
  }

  // If this function was called for an unhandled reason, let's learn about it.
  if (!eventHandled)
    __BKPT(1);
}

static enum {
  AWAITING_START_CODE,
  AWAITING_LENGTH_BYTE,
  AWAITING_PAYLOAD,
} rxState = AWAITING_START_CODE;
/** @fn handleRx()
 *
 *
 */
void handleRx(void)
{
  switch (rxState)
  {
  case AWAITING_START_CODE:
  {
    if (rxPacket[START_CODE_POSITION] == START_CODE)
      getLengthPrefix();
    else
      getStartCode();
    break;
  }
  case AWAITING_LENGTH_BYTE:
  {
    if (lengthValid())
      getPayload();
    else
      getStartCode();
    break;
  }
  case AWAITING_PAYLOAD:
  {
    // Message fully present now, hand off.
    rxCallback(&rxPacket[LENGTH_POSITION]);
    getStartCode();
    break;
  }
  }
}

inline bool lengthValid(void)
{
  uint8_t length = rxPacket[LENGTH_POSITION];
  return (length > 0) && (length <= MAX_MESSAGE_SIZE_BYTES);
}

void getStartCode(void)
{
  uart->Receive(&rxPacket[START_CODE_POSITION], START_CODE_SIZE);
  rxState = AWAITING_START_CODE;
}
void getLengthPrefix(void)
{
  uart->Receive(&rxPacket[LENGTH_POSITION], LENGTH_SIZE_BYTES);
  rxState = AWAITING_LENGTH_BYTE;
}
void getPayload(void)
{
  uint32_t payloadLen = getPacketLength(rxPacket) - PAYLOAD_POSITION;
  uart->Receive(&rxPacket[PAYLOAD_POSITION], payloadLen);
  rxState = AWAITING_PAYLOAD;
}

static inline uint32_t getPacketLength(const uint8_t *packet)
{
  // Immediately convert to start-code aware reference frame.
  uint32_t crcPosition =
      getCRCPosition(&packet[LENGTH_POSITION]) + LENGTH_POSITION;
  
  // crcPosition is another name for the length of everything before the CRC.
  return (crcPosition + CRC_SIZE_BYTES);
}

#endif // FMT_USES_UART
