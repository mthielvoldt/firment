#include <comm_pcbDetails.h>
#ifdef FMT_USES_UART

// This file's interface
#include <fmt_uart_port.h>

// Dependencies
#include <fmt_gpio_port.h> // port_initUartPins()
#include <MX_Device.h>     // uart_pcbDetails.h > MX_UARTx
#include <stm32l4xx.h>
#include <stm32l4xx_hal_cortex.h> // HAL_NVIC_EnableIRQ()
#include <stm32l4xx_hal_dma.h>    // DMA_HandleTypeDef (needed by _hal_uart.h)
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_uart.h>
#include <stm32l4xx_hal_uart_ex.h>

typedef struct
{
  uint32_t irqNumber;
  UART_HandleTypeDef *huart;
  USART_TypeDef *module;
} hwInfo_t;

static hwInfo_t getHWInfo(uint8_t driverId);

/* STM32.c doesn't provide IRQHandlers (you're expected to use CubeMX)*/
#define UART_RESOURCES(n)          \
  UART_HandleTypeDef huart##n;      \
  void USART##n##_IRQHandler(void); \
  void USART##n##_IRQHandler(void)  \
  {                               \
    HAL_UART_IRQHandler(&huart##n); \
  }

#if MX_UART1
UART_RESOURCES(1)
#elif MX_UART2
UART_RESOURCES(2)
#elif MX_UART3
UART_RESOURCES(3)
#else
#error "One of the macros MX_UART{1,2,3} must be defined as 1."
#endif

bool port_initUartModule(const uartCfg_t *config)
{
  hwInfo_t info = getHWInfo(config->driverId);
  UART_HandleTypeDef *huart = info.huart;

  huart->Instance = info.module;
  huart->Init = (UART_InitTypeDef){
      .BaudRate = config->baudHz,
      .WordLength = UART_WORDLENGTH_8B,
      .StopBits = UART_STOPBITS_1,
      .Parity = UART_PARITY_NONE,
      .Mode = UART_MODE_TX_RX,
      .HwFlowCtl = UART_HWCONTROL_NONE,
      .OverSampling = UART_OVERSAMPLING_16,
      .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE,
  };

  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(huart) != HAL_OK)
    return false;

  port_initUartPins();

#if MX_UART1
  __HAL_RCC_USART1_CLK_ENABLE();
#elif MX_UART2
  __HAL_RCC_USART2_CLK_ENABLE();
#elif MX_UART3
  __HAL_RCC_USART3_CLK_ENABLE();
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
#define CASE_UART(n)                 \
  case n:                           \
    return (hwInfo_t){              \
        .irqNumber = USART##n##_IRQn, \
        .huart = &huart##n,           \
        .module = USART##n,        \
    };

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
