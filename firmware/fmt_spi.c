// This file's interface
#include "fmt_spi.h"       // SPI-specific interface (for init)

// Dependencies owned by Firment
#include "fmt_assert.h"
#include "fmt_ioc.h" // fmt_initIoc
#include "fmt_sizes.h"
#include <fmt_spi_port.h> // port_initSpiModule()  port_getSpiEventIRQn()
#include <core_port.h>    // NVIC_...()
#include <cmsis_gcc.h>    // __BKPT()

// Dependencies 3rd party
#include <string.h> // memset()

static queue_t *sendQueue = NULL;
static uint8_t rxPacket[MAX_PACKET_SIZE_BYTES] = {0};
static ARM_DRIVER_SPI *spi;
static uint8_t clearToSendIocId; // An Interrupt-on-Change config.
static uint8_t msgWaitingIocId;
static rxCallback_t rxCallback = NULL;

/* Declarations of private functions */
static void spiEventHandlerISR(uint32_t event);
void subMsgWaitingISR(void);
void subClearToSendISR(void);

/* Public function definitions */
bool fmt_initSpi(spiCfg_t cfg)
{
  spi = cfg.spiModule;
  clearToSendIocId = cfg.clearToSendIocId;
  msgWaitingIocId = cfg.msgWaitingIocId;
  uint32_t spiEventIRQn = port_getSpiEventIRQn(cfg.spiDriverId);
  ASSERT_SUCCESS(spiEventIRQn);
  ASSERT_SUCCESS(port_initSpiModule(&cfg));

  ASSERT_ARM_OK(spi->Initialize(spiEventHandlerISR));
  ASSERT_ARM_OK(spi->PowerControl(ARM_POWER_FULL));

  /* The CMSIS SPI driver interface omits any control over the priority of the
  data-ready ISR.  This should be project-specified. The above calls to
  Initialize and PowerControl sets a default priority, we'll re-set it now.*/

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), cfg.irqPriority, 0U);
  NVIC_SetPriority(spiEventIRQn, encodedPrio);

  /** Warning:
   * CMSIS says we *may* OR (|) the mode parameters (excluding Miscellaneous
   * controls).  However the XMC4000 library *Requires* the mode parameters are
   * all OR'd.  It gates setting some parameters based on other parameters,
   * which only works if they're all OR'd together and sent in a single call. */
  uint32_t modeParams =
      ARM_SPI_MODE_MASTER |
      ARM_SPI_CPOL0_CPHA1 |
      ARM_SPI_LSB_MSB |
      ARM_SPI_SS_MASTER_SW |
      ARM_SPI_DATA_BITS(8);
  ASSERT_ARM_OK(spi->Control(modeParams, cfg.baudHz));

  ASSERT_ARM_OK(spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE));

  ASSERT_SUCCESS(
      fmt_initIoc(cfg.clearToSendIocId, cfg.clearToSendIocOut, EDGE_TYPE_RISING,
                  cfg.irqPriority, subClearToSendISR));
  ASSERT_SUCCESS(
      fmt_initIoc(cfg.msgWaitingIocId, cfg.msgWaitingIocOut, EDGE_TYPE_RISING,
                  cfg.irqPriority, subMsgWaitingISR));

  // This can kick off transfers immediately.
  fmt_enableIoc(cfg.msgWaitingIocId);

  return true;
}

bool spi_linkTransport(queue_t *_sendQueue, rxCallback_t _rxCallback)
{
  if (_sendQueue && _rxCallback)
  {
    sendQueue = _sendQueue;
    rxCallback = _rxCallback;
    return true;
  }
  return false;
}

void spi_startTxChain(void)
{
  static uint8_t txPacket[MAX_PACKET_SIZE_BYTES];

  /*
  If we've finished transmitting the previous message, but there are still
  messages in the queue, pull a new packet out of the queue and keep Txing. */

  // fmt_sendMsg calls this fn at any time, so check spi ready.
  bool spiReady = !spi->GetStatus().busy;
  if (spiReady && sendQueue)
  {
    bool clearToSend = fmt_getIocPinState(clearToSendIocId);

    if (clearToSend)
    {
      bool txWaiting = numItemsInQueue(sendQueue);
      bool rxWaiting = fmt_getIocPinState(msgWaitingIocId);

      if (txWaiting || rxWaiting)
      {
        if (txWaiting)
        {
          dequeueFront(sendQueue, txPacket);
        }
        else // Implies rxWaiting.
        {
          // Send empty (zero-length) message.
          memset(txPacket, 0, MAX_PACKET_SIZE_BYTES);
        }

        /** Note: If application has multiple subs, this driver will need the
         * "MultiSlave wrapper" <SPI_MultiSlave.h> added underneath it.
         * see https://arm-software.github.io/CMSIS-Driver/latest/driver_SPI.html */
        spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);

        /** Note: in SPI we're using fixed-width data frames to simplify staying
         * synchronized in the presence of data corruption.
         * Note: this call only starts the transfer, it doesn't block.*/
        spi->Transfer(txPacket, rxPacket, MAX_PACKET_SIZE_BYTES);
      }
    }
    else // not clear to send.  Enable cts ISR to restart when cts re-asserted.
    {
      fmt_enableIoc(clearToSendIocId);
    }
  }
}

void subMsgWaitingISR(void)
{
  fmt_startTxChain();
}

void subClearToSendISR(void)
{
  // disable self (one-shot behavior)
  fmt_disableIoc(clearToSendIocId);

  // Re-start the transaction chain.
  fmt_startTxChain();
}

/* PRIVATE (static) functions */

/** Event Handler ISR
 * Runs when the SPI module detects one of the following events:
 * - Transaction completes
 * - Incoming Data is Lost (Should not fire, applies in sub mode only)
 * - Mode Fault (sub-select line deactivated at invalid time)
 */
static void spiEventHandlerISR(uint32_t event)
{
  switch (event)
  {
  case ARM_SPI_EVENT_TRANSFER_COMPLETE:
    spi->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    if (rxPacket[LENGTH_POSITION] && rxCallback)
    {
      rxCallback(rxPacket);
    }
    /* This will trigger a Send as soon as CTS pin has a rising edge.
    We do this instead of calling fmt_startTxChain() because the ESP doesn't lower
    CTS until a few us AFTER a transaction completes, so this event handler gets
    there too early, while CTS is still high, but the ESP isn't actually ready.
    One consequence is that we now depend on the CTS signal pulsing low between
    each transaction.*/
    fmt_enableIoc(clearToSendIocId);
    break;
  case ARM_SPI_EVENT_DATA_LOST:
    /*  Occurs in slave mode when data is requested/sent by master
        but send/receive/transfer operation has not been started
        and indicates that data is lost. Occurs also in master mode
        when driver cannot transfer data fast enough. */
    __BKPT(0); /* TODO: Handle this error */
    break;
  case ARM_SPI_EVENT_MODE_FAULT:
    /*  Occurs in master mode when Slave Select is deactivated and
        indicates Master Mode Fault. */
    __BKPT(1); /* TODO: Handle this error */
    break;
  }
}
