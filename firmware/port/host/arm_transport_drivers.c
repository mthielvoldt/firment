#include <comm_test.h>
#include <Driver_SPI.h>
#include <Driver_USART.h>
#include <string.h>

static int32_t callCounts[FUNCTION_TYPE_COUNT] = {0};
static uint8_t toTargetBuff[MAX_PACKET_SIZE_BYTES] = {0};
static uint8_t fromTargetBuff[MAX_PACKET_SIZE_BYTES] = {0};
static ARM_SPI_SignalEvent_t spiEventCallback = NULL;
static ARM_USART_SignalEvent_t uartEventCallback = NULL;

// static sendStatus_t sendStatus = {0};

int32_t getCallCount(fnType_t fnType)
{
  return callCounts[fnType];
}

void commTest_reset(void)
{
  memset(callCounts, 0, sizeof(callCounts));
  memset(toTargetBuff, 0, sizeof(toTargetBuff));
  memset(fromTargetBuff, 0, sizeof(fromTargetBuff));
}

void commTest_queueIncoming(const void *data)
{
  memcpy(toTargetBuff, data, MAX_PACKET_SIZE_BYTES);
}

const uint8_t *commTest_getLastSent(void)
{
  return fromTargetBuff;
}
/** PRIVATE FUNCTIONS  */

// TODO: templatize these functions.
static int32_t InitializeSpi(ARM_SPI_SignalEvent_t cb_event)
{
  callCounts[INITIALIZE]++;
  if (cb_event == NULL)
    return ARM_DRIVER_ERROR_PARAMETER;
  spiEventCallback = cb_event;
  return ARM_DRIVER_OK;
}

static int32_t InitializeUart(ARM_USART_SignalEvent_t cb_event)
{
  callCounts[INITIALIZE]++;
  if (cb_event == NULL)
    return ARM_DRIVER_ERROR_PARAMETER;
  uartEventCallback = cb_event;
  return ARM_DRIVER_OK;
}

static int32_t Uninitialize(void);
static int32_t PowerControl(ARM_POWER_STATE state)
{
  callCounts[POWER_CONTROL]++;
  return ARM_DRIVER_OK;
}
static int32_t Transfer(const void *data_out, void *data_in, uint32_t num)
{
  callCounts[TRANSFER]++;
  if (num == MAX_PACKET_SIZE_BYTES)
  {
    memcpy(data_in, toTargetBuff, num);
    memcpy(fromTargetBuff, data_out, num);
    spiEventCallback(ARM_SPI_EVENT_TRANSFER_COMPLETE);
    return ARM_DRIVER_OK;
  }
  else
  {
    return ARM_DRIVER_ERROR;
  }
}
static int32_t Receive(void *data, uint32_t num)
{
  return ARM_DRIVER_OK;
}
static int32_t Control(uint32_t control, uint32_t arg)
{
  callCounts[CONTROL]++;
  return ARM_DRIVER_OK;
}
static ARM_SPI_STATUS GetStatus(void)
{
  callCounts[GET_STATUS]++;
  return (ARM_SPI_STATUS){.busy = 0, .data_lost = 0, .mode_fault = 0};
}

const ARM_DRIVER_SPI Driver_SPI0 = {
    .Initialize = InitializeSpi,
    .PowerControl = PowerControl,
    .Transfer = Transfer,
    .Control = Control,
    .GetStatus = GetStatus,
};
const ARM_DRIVER_SPI Driver_SPI1 = Driver_SPI0;
const ARM_DRIVER_SPI Driver_SPI2 = Driver_SPI0;
const ARM_DRIVER_SPI Driver_SPI3 = Driver_SPI0;
const ARM_DRIVER_SPI Driver_SPI4 = Driver_SPI0;
const ARM_DRIVER_SPI Driver_SPI5 = Driver_SPI0;

const ARM_DRIVER_USART Driver_UART0 = {
    .Initialize = InitializeUart,
    .PowerControl = PowerControl,
    .Control = Control,
    .Receive = Receive,
};
