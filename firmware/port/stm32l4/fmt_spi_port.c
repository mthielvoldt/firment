// This is a MFG-specific header file name.
#include <fmt_spi_port.h>
#include <fmt_gpio_port.h> // port_initSpiPins()
#include <fmt_spi.h>       // port_getSpiEventIRQn declaration
#include <MX_Device.h>     // spi_pcbDetails.h > MX_SPIx
#include <stm32l476xx.h>
#include <stm32l4xx_hal_cortex.h> // HAL_NVIC_EnableIRQ()
#include <stm32l4xx_hal_dma.h> // DMA_HandleTypeDef (needed by _hal_spi.h)
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_spi.h>
#include <stm32l4xx_hal_spi_ex.h>

typedef struct
{
  uint32_t irqNumber;
  SPI_HandleTypeDef *hspi;
  SPI_TypeDef *spiModule;
} hwInfo_t;

hwInfo_t getHWInfo(uint8_t spiDriverId);

/* STM32.c doesn't provide IRQHandlers (you're expected to use CubeMX)*/
#define SPI_RESOURCES(n)          \
  SPI_HandleTypeDef hspi##n;      \
  void SPI##n##_IRQHandler(void); \
  void SPI##n##_IRQHandler(void)  \
  {                               \
    HAL_SPI_IRQHandler(&hspi##n); \
  }

#if MX_SPI1
SPI_RESOURCES(1)
#elif MX_SPI2
SPI_RESOURCES(2)
#elif MX_SPI3
SPI_RESOURCES(3)
#else
#error "One of the macros MX_SPI{1,2,3} must be defined as 1."
#endif

bool port_initSpiModule(spiCfg_t *cfg)
{
  hwInfo_t info = getHWInfo(cfg->spiDriverId);
  SPI_HandleTypeDef *hspi = info.hspi;

  hspi->Instance = info.spiModule;
  hspi->Init = (SPI_InitTypeDef){
      .Mode = SPI_MODE_MASTER,
      .Direction = SPI_DIRECTION_2LINES,
      .DataSize = SPI_DATASIZE_16BIT,
      .CLKPolarity = SPI_POLARITY_LOW,
      .CLKPhase = SPI_PHASE_1EDGE,
      .NSS = SPI_NSS_HARD_OUTPUT,
      .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64,
      .FirstBit = SPI_FIRSTBIT_MSB,
      .TIMode = SPI_TIMODE_DISABLE,
      .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
      .CRCPolynomial = 7,
      .CRCLength = SPI_CRC_LENGTH_DATASIZE,
      .NSSPMode = SPI_NSS_PULSE_ENABLE,
  };
  if (HAL_SPI_Init(hspi) != HAL_OK)
    return false;

  /** Init GPIO pins. SPI_STM32.c does not do this for us (like XMC's does). */
  port_initSpiPins();

#if MX_SPI1
  __HAL_RCC_SPI1_CLK_ENABLE();
#elif MX_SPI2
  __HAL_RCC_SPI2_CLK_ENABLE();
#elif MX_SPI3
  __HAL_RCC_SPI3_CLK_ENABLE();
#endif

  HAL_NVIC_EnableIRQ(info.irqNumber);

  return true;
}

uint32_t port_getSpiEventIRQn(uint8_t spiDriverId)
{
  return getHWInfo(spiDriverId).irqNumber;
}

hwInfo_t getHWInfo(uint8_t spiDriverId)
{
#define CASE_SPI(n)                 \
  case n:                           \
    return (hwInfo_t){              \
        .irqNumber = SPI##n##_IRQn, \
        .hspi = &hspi##n,           \
        .spiModule = SPI##n,        \
    };

  switch (spiDriverId)
  {
#if MX_SPI0
    CASE_SPI(0)
#endif
#if MX_SPI1
    CASE_SPI(1)
#endif
#if MX_SPI2
    CASE_SPI(2)
#endif
#if MX_SPI3
    CASE_SPI(3)
#endif
#if MX_SPI4
    CASE_SPI(4)
#endif
  default:
    return (hwInfo_t){0, NULL};
  }
}
