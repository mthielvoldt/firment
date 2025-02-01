/*
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *
 */

/**
 * @file SPI.c
 * @date 30 July, 2016
 * @version 2.7
 *
 * @brief spi Driver for Infineon XMC devices
 *
 * History
 *
 * Version 2.7 Fixed FIFO functionality
 *
 * Version 2.6 Fixed IRQ handlers names
 *
 * Version 2.5 Adapted to xmc1400 devices<br>
 *
 * Version 2.4 Added fixed to prevent race conditions 
 *               and Initialize/Uninitialize, Power Control 
 *               guidelines related modifications <br>
 *
 * Version 1.0 Initial version<br>
 */

#include "SPI.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,7)   /* driver version */

#define SPI_BIT_ORDER_MSB_LSB (0UL)
#define SPI_BIT_ORDER_LSB_MSB (1UL)

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = 
{
  ARM_SPI_API_VERSION,
  ARM_SPI_DRV_VERSION
};

#if (defined(RTE_Drivers_SPI) && \
     (RTE_SPI0 == 0) && \
     (RTE_SPI1 == 0) && \
     (RTE_SPI2 == 0) && \
     (RTE_SPI3 == 0) && \
     (RTE_SPI4 == 0) && \
     (RTE_SPI5 == 0))
#error "SPI not configured in RTE_Device.h!"
#endif

// Default SPI initialization
static const XMC_SPI_CH_CONFIG_t spi_default_config =
{
  .baudrate = 1000000,
  .bus_mode = XMC_SPI_CH_BUS_MODE_MASTER,
  .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
  .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE
};


static const ARM_SPI_CAPABILITIES DriverCapabilities =
{
  1,  // Simplex Mode (Master and Slave)
  0,  // TI Synchronous Serial Interface
  0,  // Microwire Interface
  0   // Signal Mode Fault event
};


/* SPI0 */
#if (RTE_SPI0 != 0)

#if RTE_SPI0_SLAVE_SELECT_LINE0
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI0_SLAVE_SELECT_LINE1
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI0_SLAVE_SELECT_LINE2
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_2_AF 
#elif RTE_SPI0_SLAVE_SELECT_LINE3
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_3_AF 
#elif RTE_SPI0_SLAVE_SELECT_LINE4
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_4_AF 
#elif RTE_SPI0_SLAVE_SELECT_LINE5
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_5_AF 
#else
#define RTE_SPI0_SLAVE_SELECT_LINE_AF RTE_SPI0_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI0_SLAVE_SELECT_LINE0
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI0_SLAVE_SELECT_LINE1
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_1_PORT 
#elif RTE_SPI0_SLAVE_SELECT_LINE2
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_2_PORT 
#elif RTE_SPI0_SLAVE_SELECT_LINE3
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_3_PORT 
#elif RTE_SPI0_SLAVE_SELECT_LINE4
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_4_PORT 
#elif RTE_SPI0_SLAVE_SELECT_LINE5
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_5_PORT 
#else
#define RTE_SPI0_SLAVE_SELECT_LINE_PORT RTE_SPI0_SLAVE_SELECT_LINE_0_PORT
#endif

static SPI_INFO SPI0_Info = {0};
static XMC_GPIO_CONFIG_t SPI0_miso_conf; 
static XMC_GPIO_CONFIG_t SPI0_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI0_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI0_output_clock_conf;
static XMC_GPIO_CONFIG_t SPI0_selo_conf; 
#if (RTE_SPI0_SLAVE_SELECT_INPUT_ID >= 0)
static XMC_GPIO_CONFIG_t SPI0_seli_conf;
#endif

/* SPI0 Resources */
SPI_RESOURCES SPI0_Resources = 
{
  RTE_SPI0_RX_PORT,
  &SPI0_miso_conf,
  RTE_SPI0_RX_INPUT,
  RTE_SPI0_TX_PORT,
  &SPI0_mosi_conf,
  RTE_SPI0_TX_AF,
  RTE_SPI0_CLK_OUTPUT_PORT,
  &SPI0_output_clock_conf,
  RTE_SPI0_CLK_AF,
  RTE_SPI0_CLK_INPUT_PORT,
  &SPI0_input_clock_conf,
  RTE_SPI0_CLK_INPUT,
  RTE_SPI0_SLAVE_SELECT_LINE_PORT,
  &SPI0_selo_conf,
  RTE_SPI0_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI0_SLAVE_SELECT_INPUT_ID >= 0)
  RTE_SPI0_SLAVE_SELECT_INPUT_PORT,
  &SPI0_seli_conf,
  RTE_SPI0_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif
  XMC_SPI0_CH0,
  (IRQn_Type)USIC0_0_IRQn,
  RTE_SPI0_TX_FIFO_SIZE,
  RTE_SPI0_TX_FIFO_SIZE_NUM,
  RTE_SPI0_RX_FIFO_SIZE,
  RTE_SPI0_RX_FIFO_SIZE_NUM,
  &SPI0_Info,
  RTE_SPI0_SLAVE_SELECT_LINE0,
  RTE_SPI0_SLAVE_SELECT_LINE1,
  RTE_SPI0_SLAVE_SELECT_LINE2,
  RTE_SPI0_SLAVE_SELECT_LINE3,
  RTE_SPI0_SLAVE_SELECT_LINE4,
  RTE_SPI0_SLAVE_SELECT_LINE5,
  0,
  0
};

#if (RTE_SPI0_TX_FIFO_SIZE_NUM != RTE_SPI0_RX_FIFO_SIZE_NUM)
#error "SPI0: Fifo sizes for TX and RX must be equal!"
#endif

#endif

/* SPI1 */
#if (RTE_SPI1 != 0)

#if RTE_SPI1_SLAVE_SELECT_LINE0
#define RTE_SPI1_SLAVE_SELECT_LINE_AF RTE_SPI1_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI1_SLAVE_SELECT_LINE1
#define RTE_SPI1_SLAVE_SELECT_LINE_AF RTE_SPI1_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI1_SLAVE_SELECT_LINE2
#define RTE_SPI1_SLAVE_SELECT_LINE_AF RTE_SPI1_SLAVE_SELECT_LINE_2_AF 
#elif RTE_SPI1_SLAVE_SELECT_LINE3
#define RTE_SPI1_SLAVE_SELECT_LINE_AF RTE_SPI1_SLAVE_SELECT_LINE_3_AF 
#else
#define RTE_SPI1_SLAVE_SELECT_LINE_AF RTE_SPI1_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI1_SLAVE_SELECT_LINE0
#define RTE_SPI1_SLAVE_SELECT_LINE_PORT RTE_SPI1_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI1_SLAVE_SELECT_LINE1
#define RTE_SPI1_SLAVE_SELECT_LINE_PORT RTE_SPI1_SLAVE_SELECT_LINE_1_PORT  
#elif RTE_SPI1_SLAVE_SELECT_LINE2
#define RTE_SPI1_SLAVE_SELECT_LINE_PORT RTE_SPI1_SLAVE_SELECT_LINE_2_PORT 
#elif RTE_SPI1_SLAVE_SELECT_LINE3
#define RTE_SPI1_SLAVE_SELECT_LINE_PORT RTE_SPI1_SLAVE_SELECT_LINE_3_PORT
#else
#define RTE_SPI1_SLAVE_SELECT_LINE_PORT RTE_SPI1_SLAVE_SELECT_LINE_0_PORT
#endif

static SPI_INFO SPI1_Info = {0};
static XMC_GPIO_CONFIG_t SPI1_miso_conf; 
static XMC_GPIO_CONFIG_t SPI1_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI1_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI1_output_clock_conf; 
static XMC_GPIO_CONFIG_t SPI1_selo_conf; 
#if (RTE_SPI1_SLAVE_SELECT_INPUT_ID >= 0)
static XMC_GPIO_CONFIG_t SPI1_seli_conf;
#endif

/* SPI1 Resources */
SPI_RESOURCES SPI1_Resources = 
{
  RTE_SPI1_RX_PORT,
  &SPI1_miso_conf,
  RTE_SPI1_RX_INPUT,
  RTE_SPI1_TX_PORT,
  &SPI1_mosi_conf,
  RTE_SPI1_TX_AF,
  RTE_SPI1_CLK_OUTPUT_PORT,
  &SPI1_output_clock_conf,
  RTE_SPI1_CLK_AF,
  RTE_SPI1_CLK_INPUT_PORT,
  &SPI1_input_clock_conf,
  RTE_SPI1_CLK_INPUT,
  RTE_SPI1_SLAVE_SELECT_LINE_PORT,
  &SPI1_selo_conf,
  RTE_SPI1_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI1_SLAVE_SELECT_INPUT_ID >= 0)
  RTE_SPI1_SLAVE_SELECT_INPUT_PORT,
  &SPI1_seli_conf,
  RTE_SPI1_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif
  XMC_SPI0_CH1,
  (IRQn_Type)USIC0_1_IRQn,
  RTE_SPI1_TX_FIFO_SIZE,
  RTE_SPI1_TX_FIFO_SIZE_NUM,
  RTE_SPI1_RX_FIFO_SIZE,
  RTE_SPI1_RX_FIFO_SIZE_NUM,
  &SPI1_Info,
  RTE_SPI1_SLAVE_SELECT_LINE0,
  RTE_SPI1_SLAVE_SELECT_LINE1,
  RTE_SPI1_SLAVE_SELECT_LINE2,
  RTE_SPI1_SLAVE_SELECT_LINE3,
  0,
  0,
  0,
  0
};

#if (RTE_SPI1_TX_FIFO_SIZE_NUM != RTE_SPI1_RX_FIFO_SIZE_NUM)
#error "SPI1: Fifo sizes for TX and RX must be equal!"
#endif

#endif

/* SPI2 */
#if (RTE_SPI2 != 0)


#define RTE_SPI_SLAVE_SELECT_LINE0 RTE_SPI2_SLAVE_SELECT_LINE0
#define RTE_SPI_SLAVE_SELECT_LINE1 RTE_SPI2_SLAVE_SELECT_LINE1
#define RTE_SPI_SLAVE_SELECT_LINE2 RTE_SPI2_SLAVE_SELECT_LINE2 
#define RTE_SPI_SLAVE_SELECT_LINE3 RTE_SPI2_SLAVE_SELECT_LINE3

#if RTE_SPI2_SLAVE_SELECT_LINE0
#define RTE_SPI2_SLAVE_SELECT_LINE_AF RTE_SPI2_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI2_SLAVE_SELECT_LINE1
#define RTE_SPI2_SLAVE_SELECT_LINE_AF RTE_SPI2_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI2_SLAVE_SELECT_LINE2
#define RTE_SPI2_SLAVE_SELECT_LINE_AF RTE_SPI2_SLAVE_SELECT_LINE_2_AF 
#elif RTE_SPI2_SLAVE_SELECT_LINE3
#define RTE_SPI2_SLAVE_SELECT_LINE_AF RTE_SPI2_SLAVE_SELECT_LINE_3_AF 
#else
#define RTE_SPI2_SLAVE_SELECT_LINE_AF RTE_SPI2_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI2_SLAVE_SELECT_LINE0
#define RTE_SPI2_SLAVE_SELECT_LINE_PORT RTE_SPI2_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI2_SLAVE_SELECT_LINE1
#define RTE_SPI2_SLAVE_SELECT_LINE_PORT RTE_SPI2_SLAVE_SELECT_LINE_1_PORT 
#elif RTE_SPI2_SLAVE_SELECT_LINE2
#define RTE_SPI2_SLAVE_SELECT_LINE_PORT RTE_SPI2_SLAVE_SELECT_LINE_2_PORT 
#elif RTE_SPI2_SLAVE_SELECT_LINE3
#define RTE_SPI2_SLAVE_SELECT_LINE_PORT RTE_SPI2_SLAVE_SELECT_LINE_3_PORT 
#else
#define RTE_SPI2_SLAVE_SELECT_LINE_PORT RTE_SPI2_SLAVE_SELECT_LINE_0_PORT
#endif

static SPI_INFO SPI2_Info = {0};
static XMC_GPIO_CONFIG_t SPI2_miso_conf; 
static XMC_GPIO_CONFIG_t SPI2_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI2_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI2_output_clock_conf; 
static XMC_GPIO_CONFIG_t SPI2_selo_conf; 
#if (RTE_SPI2_SLAVE_SELECT_INPUT_ID >= 0)
static XMC_GPIO_CONFIG_t SPI2_seli_conf;
#endif

/* SPI2 Resources */
SPI_RESOURCES SPI2_Resources = 
{
  RTE_SPI2_RX_PORT,
  &SPI2_miso_conf,
  RTE_SPI2_RX_INPUT,
  RTE_SPI2_TX_PORT,
  &SPI2_mosi_conf,
  RTE_SPI2_TX_AF,
  RTE_SPI2_CLK_OUTPUT_PORT,
  &SPI2_output_clock_conf,
  RTE_SPI2_CLK_AF,
  RTE_SPI2_CLK_INPUT_PORT,
  &SPI2_input_clock_conf,
  RTE_SPI2_CLK_INPUT,
  RTE_SPI2_SLAVE_SELECT_LINE_PORT,
  &SPI2_selo_conf,
  RTE_SPI2_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI2_SLAVE_SELECT_INPUT_ID >= 0)
  RTE_SPI2_SLAVE_SELECT_INPUT_PORT,
  &SPI2_seli_conf,
  RTE_SPI2_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif
  XMC_SPI1_CH0,
  (IRQn_Type)USIC1_0_IRQn,
  RTE_SPI2_TX_FIFO_SIZE,
  RTE_SPI2_TX_FIFO_SIZE_NUM,
  RTE_SPI2_RX_FIFO_SIZE,
  RTE_SPI2_RX_FIFO_SIZE_NUM,
  &SPI2_Info,
  RTE_SPI2_SLAVE_SELECT_LINE0,
  RTE_SPI2_SLAVE_SELECT_LINE1,
  RTE_SPI2_SLAVE_SELECT_LINE2,
  RTE_SPI2_SLAVE_SELECT_LINE3,
  0,
  0,
  0,
  0
};

#if (RTE_SPI2_TX_FIFO_SIZE_NUM != RTE_SPI2_RX_FIFO_SIZE_NUM)
#error "SPI2: Fifo sizes for TX and RX must be equal!"
#endif

#endif


/* SPI3 */
#if (RTE_SPI3 != 0)

#if RTE_SPI3_SLAVE_SELECT_LINE0
#define RTE_SPI3_SLAVE_SELECT_LINE_AF RTE_SPI3_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI3_SLAVE_SELECT_LINE1
#define RTE_SPI3_SLAVE_SELECT_LINE_AF RTE_SPI3_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI3_SLAVE_SELECT_LINE2
#define RTE_SPI3_SLAVE_SELECT_LINE_AF RTE_SPI3_SLAVE_SELECT_LINE_2_AF 
#elif RTE_SPI3_SLAVE_SELECT_LINE3
#define RTE_SPI3_SLAVE_SELECT_LINE_AF RTE_SPI3_SLAVE_SELECT_LINE_3_AF 
#else
#define RTE_SPI3_SLAVE_SELECT_LINE_AF RTE_SPI3_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI3_SLAVE_SELECT_LINE0
#define RTE_SPI3_SLAVE_SELECT_LINE_PORT RTE_SPI3_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI3_SLAVE_SELECT_LINE1
#define RTE_SPI3_SLAVE_SELECT_LINE_PORT RTE_SPI3_SLAVE_SELECT_LINE_1_PORT 
#elif RTE_SPI3_SLAVE_SELECT_LINE2
#define RTE_SPI3_SLAVE_SELECT_LINE_PORT RTE_SPI3_SLAVE_SELECT_LINE_2_PORT
#elif RTE_SPI3_SLAVE_SELECT_LINE3
#define RTE_SPI3_SLAVE_SELECT_LINE_PORT RTE_SPI3_SLAVE_SELECT_LINE_3_PORT
#else
#define RTE_SPI3_SLAVE_SELECT_LINE_PORT RTE_SPI3_SLAVE_SELECT_LINE_0_PORT
#endif

static SPI_INFO SPI3_Info = {0};
static XMC_GPIO_CONFIG_t SPI3_miso_conf; 
static XMC_GPIO_CONFIG_t SPI3_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI3_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI3_output_clock_conf; 
static XMC_GPIO_CONFIG_t SPI3_selo_conf; 
#if (RTE_SPI3_SLAVE_SELECT_INPUT_ID >= 0)
static XMC_GPIO_CONFIG_t SPI3_seli_conf;
#endif

/* SPI3 Resources */
SPI_RESOURCES SPI3_Resources = 
{
  RTE_SPI3_RX_PORT,
  &SPI3_miso_conf,
  RTE_SPI3_RX_INPUT,
  RTE_SPI3_TX_PORT,
  &SPI3_mosi_conf,
  RTE_SPI3_TX_AF,
  RTE_SPI3_CLK_OUTPUT_PORT,
  &SPI3_output_clock_conf,
  RTE_SPI3_CLK_AF,
  RTE_SPI3_CLK_INPUT_PORT,
  &SPI3_input_clock_conf,
  RTE_SPI3_CLK_INPUT,
  RTE_SPI3_SLAVE_SELECT_LINE_PORT,
  &SPI3_selo_conf,
  RTE_SPI3_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI3_SLAVE_SELECT_INPUT_ID >= 0) 
  RTE_SPI3_SLAVE_SELECT_INPUT_PORT,
  &SPI3_seli_conf,
  RTE_SPI3_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif  
  XMC_SPI1_CH1,
  (IRQn_Type)USIC1_1_IRQn,
  RTE_SPI3_TX_FIFO_SIZE,
  RTE_SPI3_TX_FIFO_SIZE_NUM,
  RTE_SPI3_RX_FIFO_SIZE,
  RTE_SPI3_RX_FIFO_SIZE_NUM,
  &SPI3_Info,
  RTE_SPI3_SLAVE_SELECT_LINE0,
  RTE_SPI3_SLAVE_SELECT_LINE1,
  RTE_SPI3_SLAVE_SELECT_LINE2,
  RTE_SPI3_SLAVE_SELECT_LINE3,
  0,
  0,
  0,
  0
};

#if (RTE_SPI3_TX_FIFO_SIZE_NUM != RTE_SPI3_RX_FIFO_SIZE_NUM)
#error "SPI3: Fifo sizes for TX and RX must be equal!"
#endif

#endif


/* SPI4 */
#if (RTE_SPI4 != 0)

#if RTE_SPI4_SLAVE_SELECT_LINE0
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI4_SLAVE_SELECT_LINE1
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI4_SLAVE_SELECT_LINE2
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_2_AF 
#elif RTE_SPI4_SLAVE_SELECT_LINE3
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_3_AF 
#elif RTE_SPI4_SLAVE_SELECT_LINE4
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_4_AF 
#else
#define RTE_SPI4_SLAVE_SELECT_LINE_AF RTE_SPI4_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI4_SLAVE_SELECT_LINE0
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI4_SLAVE_SELECT_LINE1
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_1_PORT 
#elif RTE_SPI4_SLAVE_SELECT_LINE2
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_2_PORT
#elif RTE_SPI4_SLAVE_SELECT_LINE3
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_3_PORT 
#elif RTE_SPI4_SLAVE_SELECT_LINE4
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_4_PORT 
#else
#define RTE_SPI4_SLAVE_SELECT_LINE_PORT RTE_SPI4_SLAVE_SELECT_LINE_0_PORT 
#endif

static SPI_INFO SPI4_Info = {0};
static XMC_GPIO_CONFIG_t SPI4_miso_conf; 
static XMC_GPIO_CONFIG_t SPI4_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI4_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI4_output_clock_conf; 
static XMC_GPIO_CONFIG_t SPI4_selo_conf;  
#if (RTE_SPI4_SLAVE_SELECT_INPUT_ID >= 0) 
static XMC_GPIO_CONFIG_t SPI4_seli_conf;
#endif

/* SPI4 Resources */
SPI_RESOURCES SPI4_Resources = 
{
  RTE_SPI4_RX_PORT,
  &SPI4_miso_conf,
  RTE_SPI4_RX_INPUT,
  RTE_SPI4_TX_PORT,
  &SPI4_mosi_conf,
  RTE_SPI4_TX_AF,
  RTE_SPI4_CLK_OUTPUT_PORT,
  &SPI4_output_clock_conf,
  RTE_SPI4_CLK_AF,
  RTE_SPI4_CLK_INPUT_PORT,
  &SPI4_input_clock_conf,
  RTE_SPI4_CLK_INPUT,
  RTE_SPI4_SLAVE_SELECT_LINE_PORT,
  &SPI4_selo_conf,
  RTE_SPI4_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI4_SLAVE_SELECT_INPUT_ID >= 0) 
  RTE_SPI4_SLAVE_SELECT_INPUT_PORT,
  &SPI4_seli_conf,
  RTE_SPI4_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif  
  XMC_SPI2_CH0,
  (IRQn_Type)USIC2_0_IRQn,
  RTE_SPI4_TX_FIFO_SIZE,
  RTE_SPI4_TX_FIFO_SIZE_NUM,
  RTE_SPI4_RX_FIFO_SIZE,
  RTE_SPI4_RX_FIFO_SIZE_NUM,
  &SPI4_Info,
  RTE_SPI4_SLAVE_SELECT_LINE0,
  RTE_SPI4_SLAVE_SELECT_LINE1,
  RTE_SPI4_SLAVE_SELECT_LINE2,
  RTE_SPI4_SLAVE_SELECT_LINE3,
  RTE_SPI4_SLAVE_SELECT_LINE4,
  0,
  0,
  0
};

#if (RTE_SPI4_TX_FIFO_SIZE_NUM != RTE_SPI4_RX_FIFO_SIZE_NUM)
#error "SPI4: Fifo sizes for TX and RX must be equal!"
#endif

#endif


/* SPI5 */
#if (RTE_SPI5 != 0)

#if RTE_SPI5_SLAVE_SELECT_LINE0
#define RTE_SPI5_SLAVE_SELECT_LINE_AF RTE_SPI5_SLAVE_SELECT_LINE_0_AF
#elif RTE_SPI5_SLAVE_SELECT_LINE1
#define RTE_SPI5_SLAVE_SELECT_LINE_AF RTE_SPI5_SLAVE_SELECT_LINE_1_AF 
#elif RTE_SPI5_SLAVE_SELECT_LINE2
#define RTE_SPI5_SLAVE_SELECT_LINE_AF RTE_SPI5_SLAVE_SELECT_LINE_2_AF 
#else
#define RTE_SPI5_SLAVE_SELECT_LINE_AF RTE_SPI5_SLAVE_SELECT_LINE_0_AF
#endif

#if RTE_SPI5_SLAVE_SELECT_LINE0
#define RTE_SPI5_SLAVE_SELECT_LINE_PORT RTE_SPI5_SLAVE_SELECT_LINE_0_PORT
#elif RTE_SPI5_SLAVE_SELECT_LINE1
#define RTE_SPI5_SLAVE_SELECT_LINE_PORT RTE_SPI5_SLAVE_SELECT_LINE_1_PORT
#elif RTE_SPI5_SLAVE_SELECT_LINE2
#define RTE_SPI5_SLAVE_SELECT_LINE_PORT RTE_SPI5_SLAVE_SELECT_LINE_2_PORT
#else
#define RTE_SPI5_SLAVE_SELECT_LINE_PORT RTE_SPI5_SLAVE_SELECT_LINE_0_PORT
#endif


static SPI_INFO SPI5_Info = {0};
static XMC_GPIO_CONFIG_t SPI5_miso_conf; 
static XMC_GPIO_CONFIG_t SPI5_mosi_conf; 
static XMC_GPIO_CONFIG_t SPI5_input_clock_conf; 
static XMC_GPIO_CONFIG_t SPI5_output_clock_conf; 
static XMC_GPIO_CONFIG_t SPI5_selo_conf;  
#if (RTE_SPI5_SLAVE_SELECT_INPUT_ID >= 0) 
static XMC_GPIO_CONFIG_t SPI5_seli_conf;
#endif

/* SPI5 Resources */
SPI_RESOURCES SPI5_Resources = 
{
  RTE_SPI5_RX_PORT,
  &SPI5_miso_conf,
  RTE_SPI5_RX_INPUT,
  RTE_SPI5_TX_PORT,
  &SPI5_mosi_conf,
  RTE_SPI5_TX_AF,
  RTE_SPI5_CLK_OUTPUT_PORT,
  &SPI5_output_clock_conf,
  RTE_SPI5_CLK_AF,
  RTE_SPI5_CLK_INPUT_PORT,
  &SPI5_input_clock_conf,
  RTE_SPI5_CLK_INPUT,
  RTE_SPI5_SLAVE_SELECT_LINE_PORT,
  &SPI5_selo_conf,
  RTE_SPI5_SLAVE_SELECT_LINE_AF,
#if (RTE_SPI5_SLAVE_SELECT_INPUT_ID >= 0) 
  RTE_SPI5_SLAVE_SELECT_INPUT_PORT,
  &SPI5_seli_conf,
  RTE_SPI5_SLAVE_SELECT_INPUT,
#else
  NULL, 0, 
  NULL,
  0,
#endif  
  XMC_SPI2_CH1,
  (IRQn_Type)USIC2_1_IRQn,
  RTE_SPI5_TX_FIFO_SIZE,
  RTE_SPI5_TX_FIFO_SIZE_NUM,
  RTE_SPI5_RX_FIFO_SIZE,
  RTE_SPI5_RX_FIFO_SIZE_NUM,
  &SPI5_Info,
  RTE_SPI5_SLAVE_SELECT_LINE0,
  RTE_SPI5_SLAVE_SELECT_LINE1,
  RTE_SPI5_SLAVE_SELECT_LINE2,
  0,
  0,
  0,
  0,
  0
};

#if (RTE_SPI5_TX_FIFO_SIZE_NUM != RTE_SPI5_RX_FIFO_SIZE_NUM)
#error "SPI5: Fifo sizes for TX and RX must be equal!"
#endif

#endif


/* SPI Resources */
static SPI_RESOURCES  *spi[6] = 
{
#if (RTE_SPI0 != 0)
  &SPI0_Resources,
#else
  NULL,
#endif
#if (RTE_SPI1 != 0)
  &SPI1_Resources,
#else
  NULL,
#endif
#if (RTE_SPI2 != 0)
  &SPI2_Resources,
#else
  NULL,
#endif
#if (RTE_SPI3 != 0)
  &SPI3_Resources,
#else
  NULL,
#endif
#if (RTE_SPI4 != 0)
  &SPI4_Resources,
#else
  NULL,
#endif
#if (RTE_SPI5 != 0)
  &SPI5_Resources,
#else
  NULL,
#endif
};

/**
  \fn          ARM_DRV_VERSION SPIX_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION SPIX_GetVersion(void) 
{
  return DriverVersion;
}

/**
  \fn          ARM_SPI_CAPABILITIES SPI_GetCapabilities (SPI_RESOURCES  *spi)
  \brief       Get driver capabilities.
  \param[in]   spi    Pointer to SPI resources
  \return      \ref SPI_CAPABILITIES
*/
static inline ARM_SPI_CAPABILITIES SPI_GetCapabilities(SPI_RESOURCES *const spi) 
{
  (void)spi;
  return  DriverCapabilities;
}

#if (RTE_SPI0 != 0)
static ARM_SPI_CAPABILITIES SPI0_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static ARM_SPI_CAPABILITIES SPI1_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static ARM_SPI_CAPABILITIES SPI2_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static ARM_SPI_CAPABILITIES SPI3_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static ARM_SPI_CAPABILITIES SPI4_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static ARM_SPI_CAPABILITIES SPI5_GetCapabilities(void) 
{
  return SPI_GetCapabilities(spi[5]);
}
#endif

/**
  \fn          ARM_SPI_STATUS SPI_Initialize (ARM_SPI_SignalEvent_t cb_event,
                                              SPI_RESOURCES  *spi)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event    Pointer to \ref ARM_SPI_SignalEvent
  \param[in]   spi    Pointer to SPI resources
  \return      \ref ARM_SPI_STATUS
*/
static int32_t SPI_Initialize(ARM_SPI_SignalEvent_t cb_event, SPI_RESOURCES *const spi)
{
  if (((spi->info->flags) & SPI_INITIALIZED) == 0)
  {  
    // Initialize SPI Run-Time Resources
    spi->info->cb_event          = cb_event;
    spi->info->status.busy       = 0;
    spi->info->status.data_lost  = 0;
    spi->info->status.mode_fault = 0;
    spi->info->flags |= SPI_INITIALIZED;
  }

  return ARM_DRIVER_OK;
}

#if (RTE_SPI0 != 0)
static int32_t SPI0_Initialize(ARM_SPI_SignalEvent_t cb_event) 
{
  return SPI_Initialize(cb_event, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
  return SPI_Initialize(cb_event, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Initialize(ARM_SPI_SignalEvent_t cb_event) 
{
  return SPI_Initialize(cb_event, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Initialize(ARM_SPI_SignalEvent_t cb_event) 
{
  return SPI_Initialize (cb_event, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Initialize(ARM_SPI_SignalEvent_t cb_event) 
{
  return SPI_Initialize (cb_event, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Initialize(ARM_SPI_SignalEvent_t cb_event) 
{
  return SPI_Initialize (cb_event, spi[5]);
}
#endif


  /**
  \fn          int32_t SPI_Uninitialize (SPI_RESOURCES *spi)
  \brief       De-initialize SPI Interface.
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Uninitialize(SPI_RESOURCES *const spi) 
{
  spi->info->flags = 0;

  return ARM_DRIVER_OK;
}

#if (RTE_SPI0 != 0)
static int32_t SPI0_Uninitialize(void) 
{
  return SPI_Uninitialize(spi[0]);
}
#endif  

#if (RTE_SPI1 != 0)
static int32_t SPI1_Uninitialize(void) 
{
  return SPI_Uninitialize(spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Uninitialize(void)
{
  return SPI_Uninitialize(spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Uninitialize(void) 
{
  return SPI_Uninitialize(spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Uninitialize(void) 
{
  return SPI_Uninitialize(spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Uninitialize(void) 
{
  return SPI_Uninitialize(spi[5]);
}
#endif

/**
  \fn          ARM_SPI_STATUS SPI_PowerControl (ARM_POWER_STATE  state,
                                                  SPI_RESOURCES   *spi)
  \brief       Controls SPI Interface Power.
  \param[in]   state    Power state
  \param[in]   spi Pointer to SPI resources
  \return      \ref SPI_STATUS
*/
static int32_t SPI_PowerControl(ARM_POWER_STATE state, SPI_RESOURCES *const spi) 
{
 
  if (state == ARM_POWER_FULL) 
  {
    if ((spi->info->flags & SPI_INITIALIZED) == 0U) 
    {
      return ARM_DRIVER_ERROR;
    }
    if ((spi->info->flags & SPI_POWERED)!= 0U) 
    {
      return ARM_DRIVER_OK;
    }

    if (spi->tx_fifo_size_num > spi->rx_fifo_size_num)
    {
      spi->info->tx_fifo_pointer = (((uint32_t)(spi->spi) & 0x200U) != 0) ? 32 : 0;
      spi->info->rx_fifo_pointer = spi->info->tx_fifo_pointer + spi->tx_fifo_size_num;
    }
    else
    {
      spi->info->rx_fifo_pointer = (((uint32_t)(spi->spi) & 0x200U) != 0) ? 32 : 0;
      spi->info->tx_fifo_pointer = spi->info->rx_fifo_pointer + spi->rx_fifo_size_num;
    }

    XMC_SPI_CH_Init(spi->spi,&spi_default_config);
    spi->spi->TCSR &=~ USIC_CH_TCSR_HPCMD_Msk;
    XMC_USIC_CH_TXFIFO_Configure(spi->spi, spi->info->tx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->tx_fifo_size_reg, 1); 
   
    NVIC_ClearPendingIRQ(spi->irq_rx_num);
#if(UC_FAMILY == XMC4)
    NVIC_SetPriority(spi->irq_rx_num,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U, 0U)); 
#else
    NVIC_SetPriority(spi->irq_rx_num, 3U); 
#endif

#if (UC_SERIES == XMC14)    
    XMC_SCU_SetInterruptControl(spi->irq_rx_num,(XMC_SCU_IRQCTRL_t)(((spi->irq_rx_num) << 8U) | 3U));
#endif
    NVIC_EnableIRQ(spi->irq_rx_num);  

    spi->info->flags |= SPI_POWERED;
  } 
  else if(state == ARM_POWER_OFF) 
  {
    NVIC_DisableIRQ(spi->irq_rx_num);  

    // Disable events
    XMC_SPI_CH_DisableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION | XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_RXFIFO_DisableEvent(spi->spi, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
        
    // Disable peripheral, terminates any pending data transfers
    while (XMC_SPI_CH_Stop(spi->spi) == XMC_SPI_CH_STATUS_BUSY)
    {
      ;
    }
 
    // Invalidate receive buffers
    (void)XMC_SPI_CH_GetReceivedData(spi->spi);
    (void)XMC_SPI_CH_GetReceivedData(spi->spi);
    
    XMC_USIC_CH_TXFIFO_Configure(spi->spi, 0, XMC_USIC_CH_FIFO_DISABLED, 0); 
    XMC_USIC_CH_RXFIFO_Configure(spi->spi, 0, XMC_USIC_CH_FIFO_DISABLED, 0); 
    
    spi->info->flags &= ~SPI_POWERED;
  } 
  else 
  {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}
#if (RTE_SPI0 != 0)
static int32_t SPI0_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_PowerControl(ARM_POWER_STATE state) 
{
  return SPI_PowerControl(state, spi[5]);
}
#endif

/**
  \fn          int32_t SPI_Send (const void   *data,
                                       uint32_t         num,
                                       SPI_RESOURCES   *spi)
  \brief       Write data to SPI transmitter.
  \param[in]   data  Pointer to buffer with data to write to SPI transmitter
  \param[in]   num  Data buffer size in bytes
  \param[in]   spi Pointer to SPI resources
  \return      number of data bytes written, error code if negative
*/
static int32_t SPI_Send(const void *data, uint32_t num, SPI_RESOURCES *const spi)
{
  uint8_t temp;
  uint8_t SRno1;

  if ((data == NULL) || (num == 0)) 
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((spi->info->flags & SPI_POWERED) == 0)
  {
    // SPI is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (spi->info->status.busy)
  {
    return ARM_DRIVER_ERROR_BUSY;
  }     

  SRno1 = (((uint32_t)(spi->spi) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno1 += (((uint32_t)(spi->spi) & 0x4000U) != 0) ? 1 : 0;
#endif

  spi->info->status.busy       = 1;
  spi->info->status.data_lost  = 0;
  spi->info->status.mode_fault = 0;

  spi->info->xfer.rx_buf = NULL;
  spi->info->xfer.tx_buf = (uint8_t *)data;
  spi->info->xfer.num  = num * spi->info->xfer.data_bytes;
  spi->info->xfer.rx_cnt = 0;
  spi->info->xfer.tx_cnt = 0;

  XMC_SPI_CH_EnableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);
  XMC_SPI_CH_SetInterruptNodePointer(spi->spi, SRno1);
  XMC_SPI_CH_ClearStatusFlag(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);

  if(spi->tx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(spi->spi, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno1);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno1);
  }
  else
  { 
    XMC_USIC_CH_TXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_EnableEvent(spi->spi, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE); 
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno1);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno1);
    
    if(num <= spi->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, num - 1U); 
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, spi->rx_fifo_size_num >> 1); 
    }     
  } 

  if (spi->tx_fifo_size_reg == NO_FIFO)
  { 
    if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
    {
      temp = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
      spi->info->xfer.tx_buf++;
    }
    else
    {
      temp = *(spi->info->xfer.tx_buf++);
    }
    spi->info->xfer.tx_cnt++;
    XMC_SPI_CH_Transmit(spi->spi, temp, XMC_SPI_CH_MODE_STANDARD);          
  }
  else
  {   
    XMC_SPI_CH_DisableDataTransmission(spi->spi);
    /* Write to FIFO till Fifo is full */
    while((XMC_USIC_CH_TXFIFO_IsFull(spi->spi) == false))
    {
      if (spi->info->xfer.tx_cnt < spi->info->xfer.num)
      {      
        if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
        {
          temp = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
          spi->info->xfer.tx_buf++;
        }
        else
        {
          temp = *(spi->info->xfer.tx_buf++);
        }

        XMC_USIC_CH_TXFIFO_PutData(spi->spi, temp);
        spi->info->xfer.tx_cnt++;
      }
      else
      {
        break;
      }   
    } 
    XMC_SPI_CH_EnableDataTransmission(spi->spi);
  }        
  return ARM_DRIVER_OK;
}

#if (RTE_SPI0 != 0)
static int32_t SPI0_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Send(const void *data, uint32_t num) 
{
  return SPI_Send(data, num, spi[5]);
}
#endif


/**
  \fn          int32_t SPI_Receive (void *data,
                                      uint32_t size,
                                      SPI_RESOURCES *spi)
  \brief       Read data from SPI receiver.
  \param[out]  data  Pointer to buffer for data read from SPI receiver
  \param[in]   size  Data buffer size in bytes
  \param[in]   spi Pointer to SPI resources
  \return      number of data bytes read, error code if negative
*/
static int32_t SPI_Receive(void  *data, uint32_t num, SPI_RESOURCES *const spi) 
{                              
  uint8_t SRno1;

  if ((data == NULL) || (num == 0)) 
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((spi->info->flags & SPI_POWERED) == 0)
  {
    // SPI is not powered 
    return ARM_DRIVER_ERROR;
  }
  
  if (spi->info->status.busy)
  {
    return ARM_DRIVER_ERROR_BUSY;                                    
  }

  SRno1 = (((uint32_t)(spi->spi) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno1 += (((uint32_t)(spi->spi) & 0x4000U) != 0) ? 1 : 0;
#endif

  spi->info->status.busy       = 1;
  spi->info->status.data_lost  = 0;
  spi->info->status.mode_fault = 0;

  spi->info->xfer.rx_buf = (uint8_t *)data;
  spi->info->xfer.tx_buf = NULL;
  spi->info->xfer.num  = num * spi->info->xfer.data_bytes;
  spi->info->xfer.rx_cnt = 0;
  spi->info->xfer.tx_cnt = 0;

  XMC_SPI_CH_EnableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);
  XMC_SPI_CH_SetInterruptNodePointer(spi->spi, SRno1);
  XMC_SPI_CH_ClearStatusFlag(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);

  if(spi->tx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(spi->spi, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno1);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno1);
  }
  else
  { 
    XMC_USIC_CH_TXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_EnableEvent(spi->spi,XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE); 
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno1);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno1);
    
    if(num <= spi->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, num - 1U); 
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, spi->rx_fifo_size_num >> 1); 
    }     
  } 

  if(spi->tx_fifo_size_reg == NO_FIFO)
  { 
    spi->info->xfer.tx_cnt++;
    XMC_SPI_CH_Transmit(spi->spi, spi->info->xfer.def_val, XMC_SPI_CH_MODE_STANDARD);         
  }
  else
  {    
    XMC_SPI_CH_DisableDataTransmission(spi->spi);
    /* Write to FIFO till Fifo is full */
    while((XMC_USIC_CH_TXFIFO_IsFull(spi->spi) == false))
    {
      if(spi->info->xfer.tx_cnt < spi->info->xfer.num)
      {      
        spi->info->xfer.tx_cnt++;
        XMC_USIC_CH_TXFIFO_PutData(spi->spi, spi->info->xfer.def_val);            
      }
      else
      {
        break;
      }   
    } 
    XMC_SPI_CH_EnableDataTransmission(spi->spi);
  } 
        
  return ARM_DRIVER_OK;
}

#if (RTE_SPI0 != 0)
static int32_t SPI0_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Receive(void *data, uint32_t num) 
{
  return SPI_Receive(data, num, spi[5]);
}
#endif

/**
  \fn          int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t  num, SPI_RESOURCES  *spi)
  \brief       Start sending/receiving data to/from SPI transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   spi     Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Transfer(const void *data_out, void *data_in, uint32_t num, SPI_RESOURCES *const spi) 
{
  uint8_t data;
  uint8_t SRno1;
  
  if ((data_out == NULL) || (data_in == NULL) || (num == 0))
  { 
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  if ((spi->info->flags & SPI_POWERED) == 0)
  {
    // SPI is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (spi->info->status.busy)
  {
    return ARM_DRIVER_ERROR_BUSY;                                     
  }

  SRno1 = (((uint32_t)(spi->spi) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno1 += (((uint32_t)(spi->spi) & 0x4000U) != 0) ? 1 : 0;
#endif

  spi->info->status.busy       = 1;
  spi->info->status.data_lost  = 0;
  spi->info->status.mode_fault = 0;

  spi->info->xfer.rx_buf = (uint8_t *)data_in;
  spi->info->xfer.tx_buf = (uint8_t *)data_out;
  spi->info->xfer.num  = num * spi->info->xfer.data_bytes;
  spi->info->xfer.rx_cnt = 0;
  spi->info->xfer.tx_cnt = 0;

  XMC_SPI_CH_EnableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);
  XMC_SPI_CH_SetInterruptNodePointer(spi->spi, SRno1);
  XMC_SPI_CH_ClearStatusFlag(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);
  
  if(spi->tx_fifo_size_reg==NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(spi->spi, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno1);
    XMC_USIC_CH_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno1);
  }
  else
  { 
    XMC_USIC_CH_TXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_Flush(spi->spi);
    XMC_USIC_CH_RXFIFO_EnableEvent(spi->spi, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE); 
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno1);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(spi->spi, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno1);
    
    if(num <= spi->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, num - 1U); 
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(spi->spi, spi->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, spi->rx_fifo_size_num >> 1); 
    }     
  } 

  if (spi->tx_fifo_size_reg == NO_FIFO)
  { 
    if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
    {
      data = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
      spi->info->xfer.tx_buf++;
    }
    else
    {
      data = *(spi->info->xfer.tx_buf++);
    }
    spi->info->xfer.tx_cnt++;
    XMC_SPI_CH_Transmit(spi->spi, data, XMC_SPI_CH_MODE_STANDARD);          
  }
  else
  {   
    XMC_SPI_CH_DisableDataTransmission(spi->spi);
    /* Write to FIFO till Fifo is full */
    while((XMC_USIC_CH_TXFIFO_IsFull(spi->spi) == false))
    {
      if (spi->info->xfer.tx_cnt < spi->info->xfer.num)
      {      
        if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
        {
          data = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
          spi->info->xfer.tx_buf++;
        }
        else
        {
          data = *(spi->info->xfer.tx_buf++);
        }

        XMC_USIC_CH_TXFIFO_PutData(spi->spi, data);
        spi->info->xfer.tx_cnt++;
      }
      else
      {
        break;
      }   
    } 
    XMC_SPI_CH_EnableDataTransmission(spi->spi);
  }        
        
  return ARM_DRIVER_OK;
}

#if (RTE_SPI0 != 0)
static int32_t SPI0_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Transfer(const void *data_out, void *data_in, uint32_t num) 
{
  return SPI_Transfer(data_out, data_in, num, spi[5]);
}
#endif
  

/**
  \fn          uint32_t SPI_GetDataCount (SPI_RESOURCES *spi)
  \brief       Get received data count.
  \param[in]   spi   Pointer to SPI resources
  \return      number of data items 
*/
static inline uint32_t SPI_GetDataCount(SPI_RESOURCES *const spi) 
{
  return (spi->info->xfer.rx_cnt / spi->info->xfer.data_bytes);
}

#if (RTE_SPI0 != 0)
static uint32_t SPI0_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static uint32_t SPI1_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static uint32_t SPI2_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static uint32_t SPI3_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static uint32_t SPI4_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static uint32_t SPI5_GetDataCount(void) 
{
  return SPI_GetDataCount(spi[5]);
}
#endif

/**
  \fn          ARM_SPI_STATUS SPI_GetStatus (SPI_RESOURCES *spi)
  \brief       Get SPI status.
  \param[in]   spi     Pointer to SPI resources
  \return      SPI status \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_GetStatus(SPI_RESOURCES *const spi) 
{
  ARM_SPI_STATUS status;

  status.busy         = spi->info->status.busy;
  status.data_lost    = spi->info->status.data_lost;
  status.mode_fault   = spi->info->status.mode_fault;
  
  return status;
}

#if (RTE_SPI0 != 0)
static ARM_SPI_STATUS SPI0_GetStatus(void) 
{
  return SPI_GetStatus(spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static ARM_SPI_STATUS SPI1_GetStatus(void) 
{
  return SPI_GetStatus(spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static ARM_SPI_STATUS SPI2_GetStatus(void) 
{
  return SPI_GetStatus(spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static ARM_SPI_STATUS SPI3_GetStatus(void) 
{
  return SPI_GetStatus(spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static ARM_SPI_STATUS SPI4_GetStatus(void) 
{
  return SPI_GetStatus(spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static ARM_SPI_STATUS SPI5_GetStatus(void) 
{
  return SPI_GetStatus(spi[5]);
}
#endif
  

/**
  \fn          int32_t SPI_Control (uint32_t control, uint32_t arg, SPI_RESOURCES  *spi)
  \brief       Control SPI Interface.
  \param[in]   control  Operation
  \param[in]   arg    Argument of operation (optional)
  \param[in]   spi    Pointer to SPI resources
  \return      common \ref execution_status and driver specific \ref spi_execution_status
*/
static int32_t SPI_Control(uint32_t control, uint32_t arg, SPI_RESOURCES *const spi) 
{
  uint32_t data_bits;

  if ((spi->info->flags & SPI_POWERED) == 0U) 
  {
    return ARM_DRIVER_ERROR;
  }

  switch (control & ARM_SPI_CONTROL_Msk) 
  {
    
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    
    case ARM_SPI_MODE_INACTIVE:
      spi->info->mode = ARM_SPI_MODE_INACTIVE;
      XMC_SPI_CH_Stop(spi->spi);
      break;
    
    case ARM_SPI_MODE_MASTER:
      spi->info->mode = ARM_SPI_MODE_MASTER;
      XMC_SPI_CH_Stop(spi->spi);
      XMC_SPI_CH_SetBaudrate(spi->spi, arg);
      spi->info->xfer.baudrate = arg;
      XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_DIN0,spi->miso_mosi_input);
      XMC_SPI_CH_Start(spi->spi);
    
      spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_mosi_miso_alternate_function);
      spi->pin_output_clck_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_output_clck_alternate_function);
      if((control & ARM_SPI_SS_MASTER_MODE_Msk) == ARM_SPI_SS_MASTER_HW_OUTPUT )
      {
        spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_slave_select_output_alternate_function);
      }
      else
      {
        spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
      } 
#if(UC_FAMILY == XMC4)

      if (arg > 20000000)
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;       
      }
      else if (arg > 6000000)
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
      }
      else
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;        
      }
#endif
      spi->pin_miso_mosi_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
      // Configure pin  
      XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
      XMC_GPIO_Init(spi->output_clck_port.port, spi->output_clck_port.pin,spi->pin_output_clck_config); 
      XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);    
      XMC_GPIO_Init(spi->miso_mosi_port.port,spi->miso_mosi_port.pin,spi->pin_miso_mosi_config);  
      break;
    
    case ARM_SPI_MODE_SLAVE:
      if (spi->pin_slave_select_input_config != NULL)
      {
        spi->info->mode = ARM_SPI_MODE_SLAVE;
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_DIN0,spi->miso_mosi_input);
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_SLAVE_SCLKIN,spi->clck_input);
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_SLAVE_SELIN,spi->slave_select_input);
        spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_mosi_miso_alternate_function);
        spi->pin_miso_mosi_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_input_clck_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_slave_select_input_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
#if(UC_FAMILY == XMC4)
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE;
#endif
        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->input_clck_port.port,spi->input_clck_port.pin, spi->pin_input_clck_config); 
        XMC_GPIO_Init(spi->slave_select_input_port.port,spi->slave_select_input_port.pin,spi->pin_slave_select_input_config);     
        XMC_GPIO_Init(spi->miso_mosi_port.port,spi->miso_mosi_port.pin,spi->pin_miso_mosi_config);    
      }
      else
      {
        return ARM_SPI_ERROR_MODE;
      }
      break;
    
    case ARM_SPI_MODE_MASTER_SIMPLEX:
      // Enable Half duplex
      spi->info->mode = ARM_SPI_MODE_MASTER_SIMPLEX;
      // Configure TX pin   
      XMC_SPI_CH_Stop(spi->spi);
      XMC_SPI_CH_SetBaudrate(spi->spi, arg);
      spi->info->xfer.baudrate = arg;
      XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_DIN0,spi->miso_mosi_input); 
      XMC_SPI_CH_Start(spi->spi);
      spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN| spi->pin_mosi_miso_alternate_function);
      spi->pin_output_clck_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_output_clck_alternate_function);
      if((control & ARM_SPI_SS_MASTER_MODE_Msk) == ARM_SPI_SS_MASTER_HW_OUTPUT )
      {
        spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_slave_select_output_alternate_function);
      }
      else
      {
        spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL );
      }
#if(UC_FAMILY == XMC4)
      if (arg > 20000000)
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;       
      }
      else if (arg > 6000000)
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
      }
      else
      {
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
        spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
        spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
      }
#endif
      // Configure pin  
      XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
      XMC_GPIO_Init(spi->output_clck_port.port,spi->output_clck_port.pin, spi->pin_output_clck_config); 
      XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);      
      break;
    
    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      if (spi->pin_slave_select_input_config != NULL)
      {    
        spi->info->mode =  ARM_SPI_MODE_SLAVE_SIMPLEX;
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_DIN0,spi->miso_mosi_input);
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_SLAVE_SCLKIN,spi->clck_input);
        XMC_SPI_CH_SetInputSource(spi->spi,XMC_SPI_CH_INPUT_SLAVE_SELIN,spi->slave_select_input);
        spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN| spi->pin_mosi_miso_alternate_function);
        spi->pin_input_clck_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_slave_select_input_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;  
#if(UC_FAMILY == XMC4)
        spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE;
#endif
        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->input_clck_port.port,spi->input_clck_port.pin,spi->pin_input_clck_config); 
        XMC_GPIO_Init(spi->slave_select_input_port.port,spi->slave_select_input_port.pin,spi->pin_slave_select_input_config);   
      }
      else
      {
        return ARM_SPI_ERROR_MODE;
      }
      break;
  
    case ARM_SPI_SET_BUS_SPEED:             // Set Bus Speed in bps; arg = value
      if(spi->info->mode == ARM_SPI_MODE_MASTER)
      {
        spi->pin_mosi_miso_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_output_clck_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_slave_select_output_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;

        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->output_clck_port.port, spi->output_clck_port.pin,spi->pin_output_clck_config); 
        XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);    
        
        XMC_SPI_CH_Stop(spi->spi);
        XMC_SPI_CH_SetBaudrate(spi->spi, arg);
        spi->info->xfer.baudrate = arg;
        XMC_SPI_CH_Start(spi->spi);
        
        spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_mosi_miso_alternate_function);
        spi->pin_output_clck_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_output_clck_alternate_function);
        if((control & ARM_SPI_SS_MASTER_MODE_Msk) == ARM_SPI_SS_MASTER_HW_OUTPUT )
        {
          spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_slave_select_output_alternate_function);
  
        }
        else
        {
          spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL );
        }
#if(UC_FAMILY == XMC4)
        if (arg > 20000000)
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;       
        }
        else if (arg > 6000000)
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        }
        else
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
        }
#endif
        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->output_clck_port.port, spi->output_clck_port.pin,spi->pin_output_clck_config); 
        XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);              
      }
      
      if(spi->info->mode == ARM_SPI_MODE_MASTER_SIMPLEX)
      {
        spi->pin_mosi_miso_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_output_clck_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        spi->pin_slave_select_output_config->mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->output_clck_port.port,spi->output_clck_port.pin, spi->pin_output_clck_config); 
        XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);  
      
        XMC_SPI_CH_Stop(spi->spi);
        XMC_SPI_CH_SetBaudrate(spi->spi, arg);
        spi->info->xfer.baudrate = arg;
        XMC_SPI_CH_Start(spi->spi); 
          
        
        spi->pin_mosi_miso_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN| spi->pin_mosi_miso_alternate_function);
        spi->pin_output_clck_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_output_clck_alternate_function);
        if((control & ARM_SPI_SS_MASTER_MODE_Msk) == ARM_SPI_SS_MASTER_HW_OUTPUT )
        {
          spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | spi->pin_slave_select_output_alternate_function);  
        }
        else
        {
          spi->pin_slave_select_output_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL );
        }
#if(UC_FAMILY == XMC4)
        if (arg > 20000000)
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;       
        }
        else if (arg > 6000000)
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
        }
        else
        {
          spi->pin_mosi_miso_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
          spi->pin_output_clck_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
          spi->pin_slave_select_output_config->output_strength= XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
        }
#endif
        // Configure pin  
        XMC_GPIO_Init(spi->mosi_miso_port.port,spi->mosi_miso_port.pin, spi->pin_mosi_miso_config);
        XMC_GPIO_Init(spi->output_clck_port.port,spi->output_clck_port.pin, spi->pin_output_clck_config); 
        XMC_GPIO_Init(spi->slave_select_output_port.port,spi->slave_select_output_port.pin,spi->pin_slave_select_output_config);        
      
      }
      
      return ARM_DRIVER_OK;

    case ARM_SPI_GET_BUS_SPEED:             // Get Bus Speed in bps
      return (int32_t)spi->info->xfer.baudrate;

    case ARM_SPI_SET_DEFAULT_TX_VALUE:      // Set default Transmit value; arg = value
      spi->info->xfer.def_val= arg;
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS:                // Control Slave Select; arg = 0:inactive, 1:active 
      if (arg == ARM_SPI_SS_INACTIVE)
      {       
        XMC_GPIO_SetOutputHigh(spi->slave_select_output_port.port,spi->slave_select_output_port.pin);       
      }
      else
      {
        XMC_GPIO_SetOutputLow(spi->slave_select_output_port.port,spi->slave_select_output_port.pin);
      }
      return ARM_DRIVER_OK;

    case ARM_SPI_ABORT_TRANSFER:            // Abort current data transfer
      XMC_SPI_CH_Stop(spi->spi);
      spi->info->status.busy = 0; 
      XMC_SPI_CH_Start(spi->spi);
      return ARM_DRIVER_OK;
  
  }
  
  if (spi->info->mode ==  ARM_SPI_MODE_MASTER) 
  {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) 
    {
      case ARM_SPI_SS_MASTER_UNUSED:        // SPI Slave Select when Master: Not used (default)
        spi->info->mode  &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        spi->info->mode  |=  ARM_SPI_SS_MASTER_UNUSED;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT:      // SPI Slave Select when Master: Hardware monitored Input
        spi->info->mode  &= ~ARM_SPI_SS_MASTER_MODE_Msk;      
        spi->info->mode  |=  ARM_SPI_SS_MASTER_HW_INPUT;
        return ARM_SPI_ERROR_SS_MODE;

      case ARM_SPI_SS_MASTER_SW:            // SPI Slave Select when Master: Software controlled
        spi->info->mode  &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        spi->info->mode  |=  ARM_SPI_SS_MASTER_SW;
        break;

      case ARM_SPI_SS_MASTER_HW_OUTPUT:     // SPI Slave Select when Master: Hardware controlled Output
        spi->info->mode  &= ~ARM_SPI_SS_MASTER_MODE_Msk;      
       
        if(spi->spi_slave_select_line0 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_0);
        }
        else if (spi->spi_slave_select_line1 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_1);
        }
        else if (spi->spi_slave_select_line2 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_2);
        }
        else if (spi->spi_slave_select_line3 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_3);
        }
        else if (spi->spi_slave_select_line4 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_4);
        }
        else if (spi->spi_slave_select_line5 == 1)
        {
          XMC_SPI_CH_EnableSlaveSelect(spi->spi, XMC_SPI_CH_SLAVE_SELECT_5);
        }
        spi->info->mode  |=  ARM_SPI_SS_MASTER_HW_OUTPUT;
        break;
      }
    }

    if (spi->info->mode ==  ARM_SPI_MODE_SLAVE) 
    {
      switch (control & ARM_SPI_SS_SLAVE_MODE_Msk) 
      {
        case ARM_SPI_SS_SLAVE_HW:             // SPI Slave Select when Slave: Hardware monitored (default)
          spi->info->mode  &= ~ARM_SPI_SS_SLAVE_MODE_Msk;
          spi->info->mode |= ARM_SPI_SS_SLAVE_HW;
          break;
        case ARM_SPI_SS_SLAVE_SW:             // SPI Slave Select when Slave: Software controlled
          spi->info->mode  &= ~ARM_SPI_SS_SLAVE_MODE_Msk;
          return ARM_SPI_ERROR_SS_MODE;
      }
    }

    // Configure Frame Format
    switch (control & ARM_SPI_FRAME_FORMAT_Msk) 
    {
      case ARM_SPI_CPOL0_CPHA0:
        XMC_SPI_CH_ConfigureShiftClockOutput(spi->spi,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_ENABLED,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
        break;

      case ARM_SPI_CPOL0_CPHA1:
        XMC_SPI_CH_ConfigureShiftClockOutput(spi->spi,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_DISABLED,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
        break;

      case ARM_SPI_CPOL1_CPHA0:
        XMC_SPI_CH_ConfigureShiftClockOutput(spi->spi,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_DISABLED,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
        break;

      case ARM_SPI_CPOL1_CPHA1:
        XMC_SPI_CH_ConfigureShiftClockOutput(spi->spi,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_ENABLED,
                                             XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
        break;
      
      default:
        return ARM_SPI_ERROR_FRAME_FORMAT;
    }

    // Configure Number of Data Bits
    data_bits = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
    if ((data_bits <= 32)) 
    {
       XMC_SPI_CH_SetWordLength(spi->spi, 8U);
       if((control & ARM_SPI_SS_MASTER_MODE_Msk) != ARM_SPI_SS_MASTER_SW)
       {
         XMC_SPI_CH_SetFrameLength(spi->spi, data_bits);
       }

       spi->info->xfer.data_bytes = (data_bits + 7) >> 3;
    } 
    else
    {
      return ARM_SPI_ERROR_DATA_BITS;
    }

    // Configure Bit Order
    if ((control & ARM_SPI_BIT_ORDER_Msk) == ARM_SPI_LSB_MSB) 
    {
      XMC_SPI_CH_SetBitOrderLsbFirst(spi->spi);
      spi->info->xfer.bit_order = SPI_BIT_ORDER_LSB_MSB;
    }
    else
    {
      XMC_SPI_CH_SetBitOrderMsbFirst(spi->spi);
      spi->info->xfer.bit_order = SPI_BIT_ORDER_MSB_LSB;
    }
  
    return ARM_DRIVER_OK;
}                             

#if (RTE_SPI0 != 0)
static int32_t SPI0_Control(uint32_t control, uint32_t arg) 
{
  return SPI_Control(control, arg, spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
static int32_t SPI1_Control(uint32_t control, uint32_t arg) 
{
  return SPI_Control(control, arg, spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
static int32_t SPI2_Control(uint32_t control, uint32_t arg) 
{
  return SPI_Control(control, arg, spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
static int32_t SPI3_Control(uint32_t control, uint32_t arg) 
{
  return SPI_Control(control, arg, spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
static int32_t SPI4_Control(uint32_t control,uint32_t arg) 
{
  return SPI_Control(control, arg, spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
static int32_t SPI5_Control(uint32_t control, uint32_t arg)
{
  return SPI_Control(control, arg, spi[5]);
}
#endif
  
/**
  \fn          void SPI_IRQHandler (SPI_RESOURCES  *spi)
  \brief       SPI Interrupt handler.
  \param[in]   spi  Pointer to SPI resources
*/
static void SPI_IRQHandler(SPI_RESOURCES *const spi) 
{
  uint8_t data;
  
  if(XMC_SPI_CH_GetStatusFlag(spi->spi)& XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION)
  {
    spi->info->status.data_lost  = 1;
    if (spi->info->cb_event) spi->info->cb_event(ARM_SPI_EVENT_DATA_LOST);
  }

  // There is still data to receive.
  if(spi->info->xfer.rx_cnt < spi->info->xfer.num)
  {
    // Not using FIFO. Transfer one byte to rx_buf.
    if(spi->rx_fifo_size_reg == NO_FIFO)
    {
      spi->info->xfer.rx_cnt++;
      data = (uint8_t)XMC_SPI_CH_GetReceivedData(spi->spi);
      
      if (spi->info->xfer.rx_buf != NULL) 
      {
        if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
        {
          *(uint8_t *)((uint32_t)spi->info->xfer.rx_buf ^ (spi->info->xfer.data_bytes - 1)) = data;
          spi->info->xfer.rx_buf++;
        }
        else
        {
          *(spi->info->xfer.rx_buf++) = data;
        }
      }
    }
    // Using FIFO.  Transfer as many bytes as are in FIFO to rx_buf. 
    else
    {
      while(XMC_USIC_CH_RXFIFO_IsEmpty(spi->spi) == false)
      { 
        if(spi->info->xfer.rx_cnt < spi->info->xfer.num)
        { 
          spi->info->xfer.rx_cnt++;
          data = (uint8_t)XMC_USIC_CH_RXFIFO_GetData(spi->spi);
          if (spi->info->xfer.rx_buf != NULL) 
          {
            if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
            {
              *(uint8_t *)((uint32_t)spi->info->xfer.rx_buf ^ (spi->info->xfer.data_bytes - 1)) = data;
              spi->info->xfer.rx_buf++;
            }
            else
            {
              *(spi->info->xfer.rx_buf++) = data;
            }
          }
        }
        else
        {
          break;
        } 
      } 
    }
    // After flushing the Rx FIFO to rx_buf, We're at the end.
    if (spi->info->xfer.num == spi->info->xfer.rx_cnt)
    {
      if(spi->rx_fifo_size_reg == NO_FIFO)
      {   
        XMC_SPI_CH_DisableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION | 
                                          XMC_SPI_CH_EVENT_ALTERNATIVE_RECEIVE | 
                                          XMC_SPI_CH_EVENT_STANDARD_RECEIVE);
      }
      else
      {
        XMC_SPI_CH_DisableEvent(spi->spi, XMC_SPI_CH_STATUS_FLAG_DATA_LOST_INDICATION);
        XMC_USIC_CH_RXFIFO_DisableEvent(spi->spi, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
      }
     
      // Clear  busy flag and set receive transfer complete event
      spi->info->status.busy = 0;      
      if (spi->info->cb_event) spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);    
    }
    // After draining the RX FIFO to rx_buf above, We're still not at xfer.num.
    // This means xfer.num was bigger than the Rx FIFO?
    else
    {
      // What's left to transfer will fit in half the fifo size.
      // So set the FIFO to trigger when there's enough space to fit everything?
      if((spi->info->xfer.num - spi->info->xfer.rx_cnt) <= (spi->rx_fifo_size_num >> 1))
      {
        XMC_USIC_CH_RXFIFO_SetSizeTriggerLimit(spi->spi, (XMC_USIC_CH_FIFO_SIZE_t)spi->rx_fifo_size_reg, spi->info->xfer.num - spi->info->xfer.rx_cnt - 1U);
        XMC_USIC_CH_RXFIFO_ClearEvent(spi->spi, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
        NVIC_ClearPendingIRQ(spi->irq_rx_num);
      }  
     
      if(spi->tx_fifo_size_reg == NO_FIFO)
      {   
        if(spi->info->xfer.tx_cnt < spi->info->xfer.num)
        {
          spi->info->xfer.tx_cnt++;
          if (spi->info->xfer.tx_buf != NULL) 
          {           
            if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
            {
              data = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
              spi->info->xfer.tx_buf++;
            }
            else
            {
              data = *(spi->info->xfer.tx_buf++);
            }

            XMC_SPI_CH_Transmit(spi->spi, *(spi->info->xfer.tx_buf++), XMC_SPI_CH_MODE_STANDARD);
          }
          else
          {
            XMC_SPI_CH_Transmit(spi->spi,(uint8_t)spi->info->xfer.def_val, XMC_SPI_CH_MODE_STANDARD);
          }   
        }
      }
      else
      {
       
        XMC_SPI_CH_DisableDataTransmission(spi->spi);
        
        /* Write to FIFO till Fifo is full */
        while((XMC_USIC_CH_TXFIFO_IsFull(spi->spi) == false))
        {
          if(spi->info->xfer.tx_cnt < spi->info->xfer.num)
          {    
            spi->info->xfer.tx_cnt++;          
            if (spi->info->xfer.tx_buf != NULL) 
            {
              if (spi->info->xfer.bit_order == SPI_BIT_ORDER_MSB_LSB)
              {
                data = *(uint8_t *)((uint32_t)spi->info->xfer.tx_buf ^ (spi->info->xfer.data_bytes - 1));
                spi->info->xfer.tx_buf++;
              }
              else
              {
                data = *(spi->info->xfer.tx_buf++);
              }

              XMC_USIC_CH_TXFIFO_PutData(spi->spi, data);
            }
            else
            {
              XMC_USIC_CH_TXFIFO_PutData(spi->spi,(uint8_t)spi->info->xfer.def_val);
            }
          }
          else
          {
            break;
          }      
        }    
        
        XMC_SPI_CH_EnableDataTransmission(spi->spi);
        
      }    
    }
  }
}

#if (RTE_SPI0 != 0)
void SPI0_ISR(void) 
{
  SPI_IRQHandler(spi[0]);
}
#endif

#if (RTE_SPI1 != 0)
void SPI1_ISR(void) 
{
  SPI_IRQHandler(spi[1]);
}
#endif

#if (RTE_SPI2 != 0)
void SPI2_ISR(void) 
{
  SPI_IRQHandler(spi[2]);
}
#endif

#if (RTE_SPI3 != 0)
void SPI3_ISR(void) 
{
  SPI_IRQHandler(spi[3]);
}
#endif

#if (RTE_SPI4 != 0)
void SPI4_ISR(void) 
{
  SPI_IRQHandler(spi[4]);
}
#endif

#if (RTE_SPI5 != 0)
void SPI5_ISR(void) 
{
  SPI_IRQHandler(spi[5]);
}
#endif

#if (RTE_SPI0 != 0)
ARM_DRIVER_SPI Driver_SPI0 = 
{
  SPIX_GetVersion,
  SPI0_GetCapabilities,
  SPI0_Initialize,
  SPI0_Uninitialize,
  SPI0_PowerControl,
  SPI0_Send, 
  SPI0_Receive,
  SPI0_Transfer,
  SPI0_GetDataCount,
  SPI0_Control,
  SPI0_GetStatus
};
#endif

#if (RTE_SPI1 != 0)
ARM_DRIVER_SPI Driver_SPI1 = 
{
  SPIX_GetVersion,
  SPI1_GetCapabilities,
  SPI1_Initialize,
  SPI1_Uninitialize,
  SPI1_PowerControl,
  SPI1_Send, 
  SPI1_Receive,
  SPI1_Transfer,
  SPI1_GetDataCount,
  SPI1_Control,
  SPI1_GetStatus
};
#endif

#if (RTE_SPI2 != 0)
ARM_DRIVER_SPI Driver_SPI2 = 
{
  SPIX_GetVersion,
  SPI2_GetCapabilities,
  SPI2_Initialize,
  SPI2_Uninitialize,
  SPI2_PowerControl,
  SPI2_Send, 
  SPI2_Receive,
  SPI2_Transfer,
  SPI2_GetDataCount,
  SPI2_Control,
  SPI2_GetStatus
};
#endif

#if (RTE_SPI3 != 0)
ARM_DRIVER_SPI Driver_SPI3 = 
{
  SPIX_GetVersion,
  SPI3_GetCapabilities,
  SPI3_Initialize,
  SPI3_Uninitialize,
  SPI3_PowerControl,
  SPI3_Send, 
  SPI3_Receive,
  SPI3_Transfer,
  SPI3_GetDataCount,
  SPI3_Control,
  SPI3_GetStatus
};
#endif

#if (RTE_SPI4 != 0)
ARM_DRIVER_SPI Driver_SPI4 = 
{
  SPIX_GetVersion,
  SPI4_GetCapabilities,
  SPI4_Initialize,
  SPI4_Uninitialize,
  SPI4_PowerControl,
  SPI4_Send, 
  SPI4_Receive,
  SPI4_Transfer,
  SPI4_GetDataCount,
  SPI4_Control,
  SPI4_GetStatus
};
#endif

#if (RTE_SPI5 != 0)
ARM_DRIVER_SPI Driver_SPI5 = 
{
  SPIX_GetVersion,
  SPI5_GetCapabilities,
  SPI5_Initialize,
  SPI5_Uninitialize,
  SPI5_PowerControl,
  SPI5_Send, 
  SPI5_Receive,
  SPI5_Transfer,
  SPI5_GetDataCount,
  SPI5_Control,
  SPI5_GetStatus
};
#endif
