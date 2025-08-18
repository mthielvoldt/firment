#define HAL_FAMILY_ENABLED
#define HAL_CORTEX_ENABLED // HAL_NVIC_EnableIRQ()
#define HAL_DMA_ENABLED    // DMA_HandleTypeDef (needed by _hal_uart.h)
#define HAL_RCC_ENABLED
#define HAL_UART_ENABLED
#include <comm_pcbDetails.h>
#ifdef FMT_USES_UART

// This file's interface
#include <fmt_uart_port.h>
#include <priority.h>
#include <fmt_assert.h>

// Dependencies
#include <fmt_gpio_port.h> // port_initUartPins()
#include <MX_Device.h>     // uart_pcbDetails.h > MX_UARTx

typedef struct
{
  uint32_t irqNumber;
  UART_HandleTypeDef *huart;
  DMA_HandleTypeDef *hdma;
  USART_TypeDef *module;
} hwInfo_t;

static hwInfo_t getHWInfo(uint8_t driverId);

/* STM32.c doesn't provide IRQHandlers (you're expected to use CubeMX)*/

#if defined(DMAMUX1)
#define UART_RESOURCES(n)                                       \
  static const uint32_t dmaRequest = DMA_REQUEST_USART##n##_RX; \
  UART_HandleTypeDef huart##n;                                  \
  DMA_HandleTypeDef hdma_usart##n##_rx;                         \
  void USART##n##_IRQHandler(void);                             \
  void USART##n##_IRQHandler(void)                              \
  {                                                             \
    HAL_UART_IRQHandler(&huart##n);                             \
  }                                                             \
  void DMA1_Channel1_IRQHandler(void);                          \
  void DMA1_Channel1_IRQHandler(void)                           \
  {                                                             \
    HAL_DMA_IRQHandler(&hdma_usart##n##_rx);                    \
  }

#else
#define UART_RESOURCES(n)           \
  UART_HandleTypeDef huart##n;      \
  void USART##n##_IRQHandler(void); \
  void USART##n##_IRQHandler(void)  \
  {                                 \
    HAL_UART_IRQHandler(&huart##n); \
  }

#endif

#if MX_UART1
UART_RESOURCES(1)
#elif MX_UART2
UART_RESOURCES(2)
#elif MX_UART3
UART_RESOURCES(3)
#else
#error "One of the macros MX_UART{1,2,3} must be defined as 1."
#endif

/**
 * Provided so firment's port library can be used to fully replace a different
 * lib's use of the UART.  This would be done with link ordering in CMakeLists
 * Note this function's prototype is in _hal_uart.h
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  (void)huart;
}

bool port_initUartModule(const uartCfg_t *config)
{
  hwInfo_t info = getHWInfo(config->driverId);
  UART_HandleTypeDef *huart = info.huart;
  DMA_HandleTypeDef *hdma = info.hdma;

  /*
  Initialize huart->Init with a minimal valid config because the CMSIS
  driver calls HAL_UART_Init() several times, only setting a couple settings at
  at time, so if these huart elements start out invalid, the first CMSIS Init,
  (or Control) call will fail early and not do its job.
  NOTE this sets all non-designated elements to 0 (defaults) */
  huart->Instance = info.module;
  huart->Init = (UART_InitTypeDef){
      .BaudRate = config->baudHz,
      .Mode = UART_MODE_TX_RX,
  };
  huart->AdvancedInit = (UART_AdvFeatureInitTypeDef){};

  port_initUartPins();

#if MX_UART1
  __HAL_RCC_USART1_CLK_ENABLE();
#elif MX_UART2
  __HAL_RCC_USART2_CLK_ENABLE();
#elif MX_UART3
  __HAL_RCC_USART3_CLK_ENABLE();
#endif

#if defined(DMAMUX1)
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma->Instance = DMA1_Channel1;
  hdma->Init = (const DMA_InitTypeDef){
      .Request = dmaRequest,
      .Direction = DMA_PERIPH_TO_MEMORY,
      .PeriphInc = DMA_PINC_DISABLE,
      .MemInc = DMA_MINC_ENABLE,
      .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
      .MemDataAlignment = DMA_MDATAALIGN_BYTE,
      .Mode = DMA_NORMAL,
      .Priority = DMA_PRIORITY_LOW,
  };
  // DeInit in case DMA was initialized earlier by now-defunct comms code.
  // HAL_DMA_Init() is called later by the ARM driver's Control(), 
  // At that time, hdma->Init set above will be used.
  ASSERT_EQUAL(HAL_DMA_DeInit(hdma), HAL_OK);

  // __HAL_LINKDMA(huart, hdmarx, *hdma);
  huart->hdmarx = hdma;
  hdma->Parent = huart;

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, FMT_TRANSPORT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
#endif

  HAL_NVIC_EnableIRQ(info.irqNumber);

  return true;
}

uint32_t port_getUartEventIRQn(uint8_t fmtUartId)
{
  return getHWInfo(fmtUartId).irqNumber;
}

hwInfo_t getHWInfo(uint8_t driverId)
{
#if defined(DMAMUX1)
#define CASE_UART(n)                  \
  case n:                             \
    return (hwInfo_t){                \
        .irqNumber = USART##n##_IRQn, \
        .huart = &huart##n,           \
        .hdma = &hdma_usart##n##_rx,  \
        .module = USART##n,           \
    };
#else
#define CASE_UART(n)                  \
  case n:                             \
    return (hwInfo_t){                \
        .irqNumber = USART##n##_IRQn, \
        .huart = &huart##n,           \
        .module = USART##n,           \
    };
#endif

  switch (driverId)
  {
#if MX_UART0
    CASE_UART(0)
#endif
#if MX_UART1
    CASE_UART(1)
#endif
#if MX_UART2
    CASE_UART(2)
#endif
#if MX_UART3
    CASE_UART(3)
#endif
#if MX_UART4
    CASE_UART(4)
#endif
  default:
    return (hwInfo_t){0, NULL};
  }
#undef CASE_UART
}

#endif // FMT_USES_UART
