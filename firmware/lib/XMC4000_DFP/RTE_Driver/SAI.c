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
 * @file SAI.c
 * @date 30 July, 2016
 * @version 1.0
 *
 * @brief SAI CMSIS Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#include "SAI.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_SAI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION i2s_driver_version = 
{
    ARM_SAI_API_VERSION,
    ARM_SAI_DRV_VERSION
};

#if (defined(RTE_Drivers_I2S) && \
     (RTE_I2S0 == 0) && \
     (RTE_I2S1 == 0) && \
     (RTE_I2S2 == 0) && \
     (RTE_I2S3 == 0) && \
     (RTE_I2S4 == 0) && \
     (RTE_I2S5 == 0))
#error "I2S not configured in RTE_Device.h!"
#endif

// Default I2S initialization
static const XMC_I2S_CH_CONFIG_t i2s_default_config =
{
  .baudrate = 1411200U, // 16 * 2 * 44.1kHz
  .bus_mode = XMC_I2S_CH_BUS_MODE_MASTER,
  .data_bits = 8,
  .frame_length = 16,
  .data_delayed_sclk_periods = 1,
  .wa_inversion = XMC_I2S_CH_WA_POLARITY_DIRECT
};

/* I2S0 */
#if (RTE_I2S0 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s0_driver_capabilities = 
{
  1, /* supports asynchronous Transmit/Receive */
  0, /* supports synchronous Transmit/Receive */
  0, /* supports user defined Protocol */
  1, /* supports I2S Protocol */
  0, /* supports MSB/LSB justified Protocol */
  0, /* supports PCM short/long frame Protocol */
  0, /* supports AC'97 Protocol */
  1, /* supports Mono mode */
  0, /* supports Companding */
  RTE_I2S0_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
  0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};

static I2S_INFO I2S0_INFO = {0};

I2S_RESOURCES I2S0_Resources = 
{
  RTE_I2S0_TX_PORT,
  RTE_I2S0_TX_AF,
  RTE_I2S0_RX_PORT,
  RTE_I2S0_RX_INPUT,
  RTE_I2S0_CLK_OUTPUT_PORT,
  RTE_I2S0_CLK_AF,
  RTE_I2S0_CLK_INPUT_PORT,
  RTE_I2S0_CLK_INPUT,
  RTE_I2S0_WS_OUTPUT_PORT,
  RTE_I2S0_WS_AF,
  RTE_I2S0_WS_INPUT_PORT,
  RTE_I2S0_WS_INPUT_ID_INPUT,
#if RTE_I2S0_MCLK_OUTPUT_ENABLE
  RTE_I2S0_MCLK_OUTPUT_PORT,
  RTE_I2S0_MCLK_AF,
#else
  NULL, 0, 0,
#endif  
  (IRQn_Type)USIC0_0_IRQn,
  RTE_I2S0_TX_FIFO_SIZE_NUM,
  RTE_I2S0_TX_FIFO_SIZE,
  RTE_I2S0_RX_FIFO_SIZE_NUM,
  RTE_I2S0_RX_FIFO_SIZE,
  0,
  &I2S0_INFO,
  XMC_I2S0_CH0
};
#endif

/* I2S1 */
#if (RTE_I2S1 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s1_driver_capabilities = 
{
    1, /* supports asynchronous Transmit/Receive */
    0, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    RTE_I2S1_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
    0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};

static I2S_INFO I2S1_INFO = {0};

I2S_RESOURCES I2S1_Resources = {
  RTE_I2S1_TX_PORT,
  RTE_I2S1_TX_AF,
  RTE_I2S1_RX_PORT,
  RTE_I2S1_RX_INPUT,
  RTE_I2S1_CLK_OUTPUT_PORT,
  RTE_I2S1_CLK_AF,
  RTE_I2S1_CLK_INPUT_PORT,
  RTE_I2S1_CLK_INPUT,
  RTE_I2S1_WS_OUTPUT_PORT,
  RTE_I2S1_WS_AF,
  RTE_I2S1_WS_INPUT_PORT,
  RTE_I2S1_WS_INPUT_ID_INPUT,
#if RTE_I2S1_MCLK_OUTPUT_ENABLE
  RTE_I2S1_MCLK_OUTPUT_PORT,
  RTE_I2S1_MCLK_AF,
#else
  NULL, 0, 0,
#endif
  (IRQn_Type)USIC0_1_IRQn,
  RTE_I2S1_TX_FIFO_SIZE_NUM,
  RTE_I2S1_TX_FIFO_SIZE,
  RTE_I2S1_RX_FIFO_SIZE_NUM,
  RTE_I2S1_RX_FIFO_SIZE,
  0,
  &I2S1_INFO,
  XMC_I2S0_CH1
};
#endif

/* I2S2 */
#if (RTE_I2S2 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s2_driver_capabilities = 
{
    1, /* supports asynchronous Transmit/Receive */
    0, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    RTE_I2S2_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
    0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};

static I2S_INFO I2S2_INFO = {0};

I2S_RESOURCES I2S2_Resources = 
{
  RTE_I2S2_TX_PORT,
  RTE_I2S2_TX_AF,
  RTE_I2S2_RX_PORT,
  RTE_I2S2_RX_INPUT,
  RTE_I2S2_CLK_OUTPUT_PORT,
  RTE_I2S2_CLK_AF,
  RTE_I2S2_CLK_INPUT_PORT,
  RTE_I2S2_CLK_INPUT,
  RTE_I2S2_WS_OUTPUT_PORT,
  RTE_I2S2_WS_AF,
  RTE_I2S2_WS_INPUT_PORT,
  RTE_I2S2_WS_INPUT_ID_INPUT,
#if RTE_I2S2_MCLK_OUTPUT_ENABLE
  RTE_I2S2_MCLK_OUTPUT_PORT,
  RTE_I2S2_MCLK_AF,
#else
  NULL, 0, 0,
#endif
  (IRQn_Type)USIC1_0_IRQn,
  RTE_I2S2_TX_FIFO_SIZE_NUM,
  RTE_I2S2_TX_FIFO_SIZE,
  RTE_I2S2_RX_FIFO_SIZE_NUM,
  RTE_I2S2_RX_FIFO_SIZE,
  0,
  &I2S2_INFO,
  XMC_I2S1_CH0
};
#endif

/* I2S3 */
#if (RTE_I2S3 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s3_driver_capabilities = 
{
    1, /* supports asynchronous Transmit/Receive */
    0, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    RTE_I2S3_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
    0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};

static I2S_INFO I2S3_INFO = {0};

I2S_RESOURCES I2S3_Resources = 
{
  RTE_I2S3_TX_PORT,
  RTE_I2S3_TX_AF,
  RTE_I2S3_RX_PORT,
  RTE_I2S3_RX_INPUT,
  RTE_I2S3_CLK_OUTPUT_PORT,
  RTE_I2S3_CLK_AF,
  RTE_I2S3_CLK_INPUT_PORT,
  RTE_I2S3_CLK_INPUT,
  RTE_I2S3_WS_OUTPUT_PORT,
  RTE_I2S3_WS_AF,
  RTE_I2S3_WS_INPUT_PORT,
  RTE_I2S3_WS_INPUT_ID_INPUT,
#if RTE_I2S3_MCLK_OUTPUT_ENABLE
  RTE_I2S3_MCLK_OUTPUT_PORT,
  RTE_I2S3_MCLK_AF,
#else
  NULL, 0, 0,
#endif
  (IRQn_Type)USIC1_1_IRQn,
  RTE_I2S3_TX_FIFO_SIZE_NUM,
  RTE_I2S3_TX_FIFO_SIZE,
  RTE_I2S3_RX_FIFO_SIZE_NUM,
  RTE_I2S3_RX_FIFO_SIZE,
  0,
  &I2S3_INFO,
  XMC_I2S1_CH1
};
#endif

/* I2S4 */
#if (RTE_I2S4 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s4_driver_capabilities = 
{
    1, /* supports asynchronous Transmit/Receive */
    0, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    RTE_I2S4_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
    0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};

static I2S_INFO I2S4_INFO = {0};

I2S_RESOURCES I2S4_Resources = {
  RTE_I2S4_TX_PORT,
  RTE_I2S4_TX_AF,
  RTE_I2S4_RX_PORT,
  RTE_I2S4_RX_INPUT,
  RTE_I2S4_CLK_OUTPUT_PORT,
  RTE_I2S4_CLK_AF,
  RTE_I2S4_CLK_INPUT_PORT,
  RTE_I2S4_CLK_INPUT,
  RTE_I2S4_WS_OUTPUT_PORT,
  RTE_I2S4_WS_AF,
  RTE_I2S4_WS_INPUT_PORT,
  RTE_I2S4_WS_INPUT_ID_INPUT,
#if RTE_I2S4_MCLK_OUTPUT_ENABLE
  RTE_I2S4_MCLK_OUTPUT_PORT,
  RTE_I2S4_MCLK_AF,
#else
  NULL, 0, 0,
#endif
  (IRQn_Type)USIC2_0_IRQn,
  RTE_I2S4_TX_FIFO_SIZE_NUM,
  RTE_I2S4_TX_FIFO_SIZE,
  RTE_I2S4_RX_FIFO_SIZE_NUM,
  RTE_I2S4_RX_FIFO_SIZE,
  0,
  &I2S4_INFO,
  XMC_I2S2_CH0
};
#endif

/* I2S5 */
#if (RTE_I2S5 != 0)

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES i2s5_driver_capabilities = 
{
    1, /* supports asynchronous Transmit/Receive */
    0, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    RTE_I2S5_MCLK_OUTPUT_ENABLE, /* supports MCLK (Master Clock) pin */
    0  /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
};


static I2S_INFO I2S5_INFO = {0};

I2S_RESOURCES I2S5_Resources = {
  RTE_I2S5_TX_PORT,
  RTE_I2S5_TX_AF,
  RTE_I2S5_RX_PORT,
  RTE_I2S5_RX_INPUT,
  RTE_I2S5_CLK_OUTPUT_PORT,
  RTE_I2S5_CLK_AF,
  RTE_I2S5_CLK_INPUT_PORT,
  RTE_I2S5_CLK_INPUT,
  RTE_I2S5_WS_OUTPUT_PORT,
  RTE_I2S5_WS_AF,
  RTE_I2S5_WS_INPUT_PORT,
  RTE_I2S5_WS_INPUT_ID_INPUT,
#if RTE_I2S5_MCLK_OUTPUT_ENABLE
  RTE_I2S5_MCLK_OUTPUT_PORT,
  RTE_I2S5_MCLK_AF,
#else
  NULL, 0, 0,
#endif
  (IRQn_Type)USIC2_1_IRQn,
  RTE_I2S5_TX_FIFO_SIZE_NUM,
  RTE_I2S5_TX_FIFO_SIZE,
  RTE_I2S5_RX_FIFO_SIZE_NUM,
  RTE_I2S5_RX_FIFO_SIZE,
  0,
  &I2S5_INFO,
  XMC_I2S2_CH1
};
#endif

/* I2S Resources */
static I2S_RESOURCES  *i2s[6] = {
#if (RTE_I2S0 != 0)
  &I2S0_Resources,
#else
  NULL,
#endif
#if (RTE_I2S1 != 0)
  &I2S1_Resources,
#else
  NULL,
#endif
#if (RTE_I2S2 != 0)
  &I2S2_Resources,
#else
  NULL,
#endif
#if (RTE_I2S3 != 0)
  &I2S3_Resources,
#else
  NULL,
#endif
#if (RTE_I2S4 != 0)
  &I2S4_Resources,
#else
  NULL,
#endif
#if (RTE_I2S5 != 0)
  &I2S5_Resources,
#else
  NULL,
#endif
};

static void I2S_TX_ISR(I2S_RESOURCES *const i2s);
static void I2S_RX_ISR(I2S_RESOURCES *const i2s);

/***********************************************************************************************************************
  \fn          ARM_DRIVER_VERSION I2SX_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
***********************************************************************************************************************/
static ARM_DRIVER_VERSION I2SX_GetVersion (void)
{
  return i2s_driver_version;
}

/***********************************************************************************************************************
  \fn          ARM_I2S_CAPABILITIES I2S_GetCapabilities (void)
  \brief       Get driver capabilities.
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref ARM_I2S_CAPABILITIES
***********************************************************************************************************************/

#if (RTE_I2S0 != 0)
static ARM_SAI_CAPABILITIES I2S0_GetCapabilities(void) 
{
  return i2s0_driver_capabilities;
}
#endif

#if (RTE_I2S1 != 0)
static ARM_SAI_CAPABILITIES I2S1_GetCapabilities(void) 
{
  return i2s1_driver_capabilities;
}
#endif

#if (RTE_I2S2 != 0)
static ARM_SAI_CAPABILITIES I2S2_GetCapabilities(void) 
{
  return i2s2_driver_capabilities;
}
#endif

#if (RTE_I2S3 != 0)
static ARM_SAI_CAPABILITIES I2S3_GetCapabilities(void) 
{
  return i2s3_driver_capabilities;
}
#endif

#if (RTE_I2S4 != 0)
static ARM_SAI_CAPABILITIES I2S4_GetCapabilities(void) 
{
  return i2s4_driver_capabilities;
}
#endif

#if (RTE_I2S5 != 0)
static ARM_SAI_CAPABILITIES I2S5_GetCapabilities(void) 
{
  return i2s5_driver_capabilities;
}
#endif

/***********************************************************************************************************************
  \fn          int32_t I2S_Initialize (ARM_SAI_SignalEvent_t cb_event)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event   Pointer to \ref ARM_SAI_SignalEvent
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t I2S_Initialize (ARM_SAI_SignalEvent_t cb_event, I2S_RESOURCES *const i2s)
{
  if (((i2s->info->flags) & I2S_INITIALIZED) == 0)
  {
    i2s->info->cb_event                     = cb_event;
    i2s->info->status.frame_error           = 0;
    i2s->info->status.rx_busy               = 0;
    i2s->info->status.rx_overflow           = 0;
    i2s->info->status.tx_busy               = 0;
    i2s->info->status.tx_underflow          = 0;

    i2s->info->flags |= I2S_INITIALIZED;
  }

  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_Initialize(ARM_SAI_SignalEvent_t cb_event) 
{
  return I2S_Initialize(cb_event, i2s[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t I2S_Uninitialize (I2S_RESOURCES *i2s)
  \brief       De-initialize I2S Interface.
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t I2S_Uninitialize(I2S_RESOURCES *const i2s)
{
  i2s->info->flags  = 0;

  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_Uninitialize(void) 
{
  return I2S_Uninitialize(i2s[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t SAI_PowerControl (ARM_POWER_STATE state, I2S_RESOURCES *i2s)
  \brief       Control SAI Interface Power.
  \param[in]   state      Power state
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t I2S_PowerControl(ARM_POWER_STATE state, I2S_RESOURCES *const i2s)
{
  if (state == ARM_POWER_FULL) 
  {
    if ((i2s->info->flags & I2S_INITIALIZED) == 0U) 
    { 
      return ARM_DRIVER_ERROR; 
    }
      
    if ((i2s->info->flags & I2S_POWERED) != 0U) 
    { 
      return ARM_DRIVER_OK; 
    }

    if (i2s->tx_fifo_size_num > i2s->rx_fifo_size_num)
    {
      i2s->info->tx_fifo_pointer = (((uint32_t)(i2s->i2s) & 0x200U) != 0) ? 32 : 0;
      i2s->info->rx_fifo_pointer = i2s->info->tx_fifo_pointer + i2s->tx_fifo_size_num;
    }
    else
    {
      i2s->info->rx_fifo_pointer = (((uint32_t)(i2s->i2s) & 0x200U) != 0) ? 32 : 0;
      i2s->info->tx_fifo_pointer = i2s->info->rx_fifo_pointer + i2s->rx_fifo_size_num;
    }

    XMC_I2S_CH_Init(i2s->i2s, &i2s_default_config);
    i2s->i2s->PCR_IISMode &= ~(uint32_t)USIC_CH_PCR_IISMode_WAGEN_Msk;

    XMC_USIC_CH_TXFIFO_Flush(i2s->i2s);
    XMC_USIC_CH_TXFIFO_Configure(i2s->i2s, i2s->info->tx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2s->tx_fifo_size_reg, 1U);
      
    NVIC_ClearPendingIRQ(i2s->irq_num);

#if(UC_FAMILY == XMC4)
    NVIC_SetPriority(i2s->irq_num, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U, 0U));
#else
    NVIC_SetPriority(i2s->irq_num, 3U);
#endif

#if (UC_SERIES == XMC14)
    XMC_SCU_SetInterruptControl(i2s->irq_num, (XMC_SCU_IRQCTRL_t)(((i2s->irq_num) << 8U) | 3U));
#endif

    NVIC_EnableIRQ(i2s->irq_num);

    i2s->info->flags |= I2S_POWERED;

  } 
  else if (state == ARM_POWER_OFF) 
  {
    NVIC_DisableIRQ(i2s->irq_num);

    // Disable events

    XMC_I2S_CH_DisableEvent(i2s->i2s, XMC_USIC_CH_EVENT_TRANSMIT_BUFFER | XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_TXFIFO_DisableEvent(i2s->i2s, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
    XMC_USIC_CH_RXFIFO_DisableEvent(i2s->i2s, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);

    // Disable peripheral, terminates any pending data transfers
    while (XMC_I2S_CH_Stop(i2s->i2s) == XMC_I2S_CH_STATUS_BUSY)
    {
      ;
    }

    XMC_USIC_CH_TXFIFO_Configure(i2s->i2s, 0, XMC_USIC_CH_FIFO_DISABLED, 0U); 
    XMC_USIC_CH_RXFIFO_Configure(i2s->i2s, 0, XMC_USIC_CH_FIFO_DISABLED, 0U); 

    i2s->info->flags &= ~I2S_POWERED;
  }
  else
  {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_PowerControl(ARM_POWER_STATE state)
{
  return I2S_PowerControl(state, i2s[5]);
}
#endif


/***********************************************************************************************************************
  \fn          int32_t I2S_Send (const void *data, uint32_t num, I2S_RESOURCES *i2s)
  \brief       Start sending data to SAI transmitter.
  \param[in]   data       Pointer to buffer with data to send to SAI transmitter
  \param[in]   num        Number of data items to send
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t I2S_Send(const void *data, uint32_t num, I2S_RESOURCES *const i2s)
{
  uint8_t SRno;

  if ((data == NULL) || (num == 0U))
  { 
    return ARM_DRIVER_ERROR_PARAMETER; 
  }
  
  if ((i2s->info->flags & I2S_CONFIGURED) == 0)
  {
    // I2S is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (i2s->info->status.tx_busy)
  { 
    return ARM_DRIVER_ERROR_BUSY;  
  }

  i2s->info->status.tx_busy = 1U;
  i2s->info->status.tx_underflow = 0U;
  i2s->info->xfer.tx_num = num * (i2s->info->xfer.data_bits >> 3);
  i2s->info->xfer.tx_cnt = 0U;
  i2s->info->xfer.tx_buf = (uint8_t *)data;

  SRno = (((uint32_t)(i2s->i2s) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2s->i2s) & 0x4000U) != 0) ? 1 : 0;
#endif

  XMC_USIC_CH_TXFIFO_Flush(i2s->i2s);
  I2S_TX_ISR(i2s);

  if(i2s->tx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(i2s->i2s, XMC_USIC_CH_EVENT_TRANSMIT_BUFFER);
    XMC_USIC_CH_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_INTERRUPT_NODE_POINTER_TRANSMIT_BUFFER, SRno);
  }
  else
  {   
    XMC_USIC_CH_TXFIFO_EnableEvent(i2s->i2s, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD); 
    XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno);  
  }

  XMC_USIC_CH_EnableEvent(i2s->i2s, XMC_USIC_CH_EVENT_TRANSMIT_SHIFT);
  XMC_USIC_CH_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_INTERRUPT_NODE_POINTER_TRANSMIT_SHIFT, SRno);

  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_Send(const void *data, uint32_t num)
{
  return I2S_Send(data, num, i2s[5]);
}
#endif


/***********************************************************************************************************************
  \fn          int32_t I2S_Receive (void *data, uint32_t num, I2S_RESOURCES *i2s)
  \brief       Start receiving data from SAI receiver.
  \param[out]  data       Pointer to buffer for data to receive from SAI receiver
  \param[in]   num        Number of data items to receive
  \param[in]   i2s        Pointer to I2S resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t I2S_Receive(void *data, uint32_t num, I2S_RESOURCES *const i2s)
{
  uint8_t SRno;

  if ((data == NULL) || (num == 0U))
  { 
    return ARM_DRIVER_ERROR_PARAMETER; 
  }
  
  if ((i2s->info->flags & I2S_CONFIGURED) == 0)
  {
    // I2S is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (i2s->info->status.tx_busy)
  { 
    return ARM_DRIVER_ERROR_BUSY;  
  }

  i2s->info->status.rx_busy = 1U;
  i2s->info->status.rx_overflow = 0U;

  i2s->info->xfer.rx_buf = (uint8_t *)data;
  i2s->info->xfer.rx_num = num * (i2s->info->xfer.data_bits >> 3);;
  i2s->info->xfer.rx_cnt = 0U;

  SRno = (((uint32_t)(i2s->i2s) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2s->i2s) & 0x4000U) != 0) ? 1 : 0;
#endif

  if(i2s->rx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno);
    XMC_USIC_CH_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno);
  }
  else
  {
    XMC_USIC_CH_RXFIFO_Flush(i2s->i2s);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2s->i2s, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno);

    if(num <= i2s->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(i2s->i2s, i2s->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2s->rx_fifo_size_reg, num - 1U);
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(i2s->i2s, i2s->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2s->rx_fifo_size_reg, i2s->rx_fifo_size_num >> 1);
    }
  }
  
  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_Receive(void *data, uint32_t num)
{
  return I2S_Receive(data, num, i2s[5]);
}
#endif


/***********************************************************************************************************************
  \fn          uint32_t I2S_GetTxCount (I2S_RESOURCES *i2s)
  \brief       Get transmitted data count.
  \param[in]   i2s        Pointer to I2S resources
  \return      number of data items transmitted
***********************************************************************************************************************/
static inline uint32_t I2S_GetTxCount(I2S_RESOURCES *const i2s)
{
  return(i2s->info->xfer.tx_cnt / (i2s->info->xfer.data_bits >> 3));
}

#if (RTE_I2S0 != 0)
static uint32_t I2S0_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static uint32_t I2S1_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static uint32_t I2S2_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static uint32_t I2S3_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static uint32_t I2S4_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static uint32_t I2S5_GetTxCount (void) 
{
  return I2S_GetTxCount(i2s[5]);
}
#endif

/***********************************************************************************************************************
  \fn          uint32_t I2S_GetRxCount (I2S_RESOURCES *i2s)
  \brief       Get received data count.
  \param[in]   i2s        Pointer to I2S resources
  \return      number of data items received
***********************************************************************************************************************/
static inline uint32_t I2S_GetRxCount (I2S_RESOURCES *i2s)
{
  return (i2s->info->xfer.rx_cnt / (i2s->info->xfer.data_bits >> 3));
}

#if (RTE_I2S0 != 0)
static uint32_t I2S0_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[0]);
}
#endif
#if (RTE_I2S1 != 0)
static uint32_t I2S1_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[1]);
}
#endif
#if (RTE_I2S2 != 0)
static uint32_t I2S2_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[2]);
}
#endif
#if (RTE_I2S3 != 0)
static uint32_t I2S3_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[3]);
}
#endif
#if (RTE_I2S4 != 0)
static uint32_t I2S4_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[4]);
}
#endif
#if (RTE_I2S5 != 0)
static uint32_t I2S5_GetRxCount (void) 
{
  return I2S_GetRxCount(i2s[5]);
}
#endif


/***********************************************************************************************************************
  \fn          int32_t ARM_SAI_Control (uint32_t control, uint32_t arg1, uint32_t arg2, I2S_RESOURCES *i2s)
  \brief       Control SAI Interface.
  \param[in]   control    Operation
  \param[in]   arg1       Argument 1 of operation (optional)
  \param[in]   arg2       Argument 2 of operation (optional)
  \param[in]   i2s        Pointer to I2S resources
  \return      common \ref execution_status and driver specific \ref sai_execution_status
***********************************************************************************************************************/
static int32_t I2S_Control(uint32_t control, uint32_t arg1, uint32_t arg2, I2S_RESOURCES *i2s)
{
  uint32_t value;
  uint8_t status;
  bool configure_tx = false;
  bool configure_rx = false;

  XMC_GPIO_CONFIG_t gpio_config;

  if ((i2s->info->flags & I2S_POWERED) == 0U) 
  { 
    return ARM_DRIVER_ERROR; 
  }

  /* Operation */
  switch (control & ARM_SAI_CONTROL_Msk)
  {
    case ARM_SAI_CONFIGURE_TX:
      configure_tx = true;
      break;

    case ARM_SAI_CONFIGURE_RX:
      configure_rx = true;
      break;

    case ARM_SAI_CONTROL_TX:
      if ((arg1 & 1U) == 0U) // disable
      {
        i2s->i2s->PCR_IISMode &= (uint32_t)~USIC_CH_PCR_IISMode_WAGEN_Msk;
      }
      else // enable
      {
        if (i2s->info->mode == ARM_SAI_MODE_MASTER)
        {
          i2s->i2s->PCR_IISMode |= (uint32_t)USIC_CH_PCR_IISMode_WAGEN_Msk;
        }
        else
        {
          i2s->i2s->PCR_IISMode &= (uint32_t)~USIC_CH_PCR_IISMode_WAGEN_Msk;
        }

      }

      if ((arg1 & 2U) == 0U) // disable
      {
        i2s->mute_mode_enabled = 0;
      }
      else
      {
        i2s->mute_mode_enabled = 1;
      }
      
      return ARM_DRIVER_OK;


    case ARM_SAI_CONTROL_RX:
      if ((arg1 & 1U) != 0U) // disable
      {
        XMC_I2S_CH_DisableEvent(i2s->i2s, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
        XMC_USIC_CH_RXFIFO_DisableEvent(i2s->i2s, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
      }
      else
      {
        XMC_I2S_CH_EnableEvent(i2s->i2s, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
        XMC_USIC_CH_RXFIFO_EnableEvent(i2s->i2s, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
      }
      
      return ARM_DRIVER_OK;

    case ARM_SAI_MASK_SLOTS_TX:
      return ARM_DRIVER_ERROR;

    case ARM_SAI_MASK_SLOTS_RX:
      return ARM_DRIVER_ERROR;

    case ARM_SAI_ABORT_SEND:
    case ARM_SAI_ABORT_RECEIVE:
      do
      {
        status = XMC_I2S_CH_Stop(i2s->i2s);
      } while (status != XMC_I2S_CH_STATUS_OK);

      value = (control & ARM_SAI_CONTROL_Msk);
      if (value == ARM_SAI_ABORT_SEND)
      {
        i2s->info->status.tx_busy = 0U;
      }
      else
      {
        i2s->info->status.rx_busy = 0U;
      }
      
      return ARM_DRIVER_OK;

    default:
      return ARM_DRIVER_ERROR;
  }

  /* Data Size */
  switch((control & ARM_SAI_DATA_SIZE_Msk) >> ARM_SAI_DATA_SIZE_Pos)
  {
    case 8-1:
      i2s->info->xfer.data_bits = 8;
      break;
    
    case 16-1:
      i2s->info->xfer.data_bits = 16;
      break;
    
    case 32-1:
      i2s->info->xfer.data_bits = 32;
      break;

    default:
      return ARM_SAI_ERROR_DATA_SIZE;
  }

  /* Protocol */
  if ((control & ARM_SAI_PROTOCOL_Msk) != ARM_SAI_PROTOCOL_I2S)
  { 
    return ARM_SAI_ERROR_PROTOCOL; 
  }

  /* Synchronization */
  if ((control & ARM_SAI_SYNCHRONIZATION_Msk))
  { 
    return ARM_SAI_ERROR_SYNCHRONIZATION; 
  }

  /* Companding */
  if ((control & ARM_SAI_COMPANDING_Msk) != ARM_SAI_COMPANDING_NONE)  
  { 
    return ARM_SAI_ERROR_COMPANDING; 
  }
  
  /* Mono Mode */
  if (control & ARM_SAI_MONO_MODE) 
  { 
    i2s->info->xfer.mono_mode = 1;
  }
  else
  {
    i2s->info->xfer.mono_mode = 0;
  }

  /* Clock polarity */
  if (control & ARM_SAI_CLOCK_POLARITY_Msk)
  {
    return ARM_SAI_ERROR_CLOCK_POLARITY;
  }

  /* Master Clock Pin (MCLK) */
  if ((control & ARM_SAI_MCLK_PIN_Msk) == ARM_SAI_MCLK_PIN_INACTIVE)
  {
    XMC_I2S_CH_DisableMasterClock(i2s->i2s);
  }
  else 
  {
    if (i2s->mclk_output_port.port != NULL)
    {
      gpio_config.mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | i2s->ws_output_alternate_function);
#if(UC_FAMILY == XMC4)
      gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
#endif
      XMC_GPIO_Init(i2s->mclk_output_port.port, i2s->mclk_output_port.pin, &gpio_config);

      XMC_I2S_CH_EnableMasterClock(i2s->i2s);
    }
    else
    {
      return ARM_DRIVER_ERROR;
    }
  }    

  if (configure_rx)
  {
    gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(i2s->sd_rx_port.port, i2s->sd_rx_port.pin, &gpio_config);
      
    XMC_I2S_CH_SetInputSource(i2s->i2s, XMC_I2S_CH_INPUT_DIN0, i2s->sd_rx_input);
  }

  switch (control & ARM_SAI_MODE_Msk)
  {
    case ARM_SAI_MODE_MASTER:
      i2s->info->mode = ARM_SAI_MODE_MASTER;
      XMC_I2S_CH_Stop(i2s->i2s);
       
      i2s->info->xfer.baudrate = (arg2 & ARM_SAI_AUDIO_FREQ_Msk) * i2s->info->xfer.data_bits * 2;
      status = XMC_I2S_CH_SetBaudrate(i2s->i2s, i2s->info->xfer.baudrate);
      
      if (status != XMC_I2S_CH_STATUS_OK)
      {
        return ARM_DRIVER_ERROR;
      }
        
      XMC_I2S_CH_SetSystemWordLength(i2s->i2s, i2s->info->xfer.data_bits);
      XMC_I2S_CH_SetFrameLength(i2s->i2s, i2s->info->xfer.data_bits);
      XMC_I2S_CH_SetWordLength(i2s->i2s, 8);

      XMC_I2S_CH_Start(i2s->i2s);
          
      gpio_config.mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | i2s->clk_output_alternate_function);
#if(UC_FAMILY == XMC4)
      if (i2s->info->xfer.baudrate > 20000000)
      {
        gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
      }
      else if (i2s->info->xfer.baudrate > 6000000)
      {
        gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
      }
      else
      {
        gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
      }
#endif
      XMC_GPIO_Init(i2s->clk_output_port.port, i2s->clk_output_port.pin, &gpio_config);


      gpio_config.mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | i2s->ws_output_alternate_function);
#if(UC_FAMILY == XMC4)
      if (i2s->info->xfer.baudrate > 20000000)
      {
        gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
      }
      else
      {
        gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
      }
#endif
      XMC_GPIO_Init(i2s->ws_output_port.port, i2s->ws_output_port.pin, &gpio_config);
        
      break;

    case ARM_SAI_MODE_SLAVE:
      i2s->info->mode = ARM_SAI_MODE_SLAVE;
      
      XMC_I2S_CH_Stop(i2s->i2s);
      XMC_I2S_CH_SetInputSource(i2s->i2s, XMC_I2S_CH_INPUT_SLAVE_SCLKIN, i2s->clk_input_input);
      XMC_I2S_CH_SetInputSource(i2s->i2s, XMC_I2S_CH_INPUT_SLAVE_WA, i2s->ws_input_input);

      gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
        
      XMC_GPIO_Init(i2s->clk_input_port.port, i2s->clk_input_port.pin, &gpio_config);
      XMC_GPIO_Init(i2s->ws_input_port.port, i2s->ws_input_port.pin, &gpio_config);
        
      XMC_I2S_CH_SetSystemWordLength(i2s->i2s, i2s->info->xfer.data_bits);
      XMC_I2S_CH_SetFrameLength(i2s->i2s, i2s->info->xfer.data_bits);
      XMC_I2S_CH_SetWordLength(i2s->i2s, 8U);

      XMC_I2S_CH_Start(i2s->i2s);

      break;
          
    default:
      return ARM_DRIVER_ERROR;
  }

  if (configure_tx)
  {
    gpio_config.mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | i2s->sd_tx_alternate_function);

#if(UC_FAMILY == XMC4)
    if (i2s->info->xfer.baudrate > 20000000)
    {
      gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
    }
    else if (i2s->info->xfer.baudrate > 6000000)
    {
      gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
    }
    else
    {
      gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
    }
#endif

    XMC_GPIO_Init(i2s->sd_tx_port.port, i2s->sd_tx_port.pin, &gpio_config);
  }

  i2s->info->flags |= I2S_CONFIGURED;

  return ARM_DRIVER_OK;
}

#if (RTE_I2S0 != 0)
static int32_t I2S0_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static int32_t I2S1_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static int32_t I2S2_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static int32_t I2S3_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static int32_t I2S4_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static int32_t I2S5_Control(uint32_t control, uint32_t arg1, uint32_t arg2)
{
  return I2S_Control(control, arg1, arg2, i2s[5]);
}
#endif

/***********************************************************************************************************************
  \fn          ARM_SAI_STATUS I2S_GetStatus (I2S_RESOURCES *i2s)
  \brief       Get SAI status.
  \param[in]   i2s        Pointer to I2S resources
  \return      SAI status \ref ARM_SAI_STATUS
***********************************************************************************************************************/
static ARM_SAI_STATUS I2S_GetStatus(I2S_RESOURCES *i2s)
{
  ARM_SAI_STATUS status;

  status.tx_busy      = i2s->info->status.tx_busy;
  status.rx_busy      = i2s->info->status.rx_busy;
  status.tx_underflow = i2s->info->status.tx_underflow;
  status.rx_overflow  = i2s->info->status.rx_overflow;
  status.frame_error  = i2s->info->status.frame_error;

  return status;
}

#if (RTE_I2S0 != 0)
static ARM_SAI_STATUS I2S0_GetStatus(void) 
{
  return I2S_GetStatus(i2s[0]);
}
#endif

#if (RTE_I2S1 != 0)
static ARM_SAI_STATUS I2S1_GetStatus(void) 
{
  return I2S_GetStatus(i2s[1]);
}
#endif

#if (RTE_I2S2 != 0)
static ARM_SAI_STATUS I2S2_GetStatus(void) 
{
  return I2S_GetStatus(i2s[2]);
}
#endif

#if (RTE_I2S3 != 0)
static ARM_SAI_STATUS I2S3_GetStatus(void) 
{
  return I2S_GetStatus(i2s[3]);
}
#endif

#if (RTE_I2S4 != 0)
static ARM_SAI_STATUS I2S4_GetStatus(void) 
{
  return I2S_GetStatus(i2s[4]);
}
#endif

#if (RTE_I2S5 != 0)
static ARM_SAI_STATUS I2S5_GetStatus(void) 
{
  return I2S_GetStatus(i2s[5]);
}
#endif

static void I2S_TX_ISR(I2S_RESOURCES  *i2s)
{   
  XMC_I2S_CH_CHANNEL_t channel; 
  uint8_t data;
  
  if (i2s->tx_fifo_size_reg == NO_FIFO)
  {
    if (i2s->mute_mode_enabled)
    {
      data = 0;
    }
    else
    {
      /* MSB to LSB */
      data = *(uint8_t *)((uint32_t)i2s->info->xfer.tx_buf ^ ((i2s->info->xfer.data_bits >> 3) - 1));
      i2s->info->xfer.tx_buf++;
    }
    
    if (i2s->info->xfer.mono_mode)
    {
      XMC_I2S_CH_Transmit(i2s->i2s, data, XMC_I2S_CH_CHANNEL_1_LEFT);
      XMC_I2S_CH_Transmit(i2s->i2s, data, XMC_I2S_CH_CHANNEL_2_RIGHT);
    }
    else
    {
      if ((i2s->info->xfer.tx_cnt & (i2s->info->xfer.data_bits >> 3)) != 0)
      {
        channel = XMC_I2S_CH_CHANNEL_2_RIGHT;
      }
      else
      {
        channel = XMC_I2S_CH_CHANNEL_1_LEFT;
      }
      XMC_I2S_CH_Transmit(i2s->i2s, data, channel);
    }
    i2s->info->xfer.tx_cnt++;
  }
  else
  {
    while((XMC_USIC_CH_TXFIFO_IsFull(i2s->i2s) == false))
    {
      if (i2s->info->xfer.tx_cnt < i2s->info->xfer.tx_num)
      {
        if (i2s->mute_mode_enabled)
        {
          data = 0;
        }
        else
        {
          /* MSB to LSB */
          data = *(uint8_t *)((uint32_t)i2s->info->xfer.tx_buf ^ ((i2s->info->xfer.data_bits >> 3) - 1));
          i2s->info->xfer.tx_buf++;
        }
        
        if (i2s->info->xfer.mono_mode)
        {
          XMC_I2S_CH_Transmit(i2s->i2s, data, XMC_I2S_CH_CHANNEL_1_LEFT);
          XMC_I2S_CH_Transmit(i2s->i2s, data, XMC_I2S_CH_CHANNEL_2_RIGHT);
        }
        else
        {
          if ((i2s->info->xfer.tx_cnt & (i2s->info->xfer.data_bits >> 3)) != 0)
          {
            channel = XMC_I2S_CH_CHANNEL_2_RIGHT;
          }
          else
          {
            channel = XMC_I2S_CH_CHANNEL_1_LEFT;
          }
          XMC_I2S_CH_Transmit(i2s->i2s, data, channel);
        }
        i2s->info->xfer.tx_cnt++;
      }
      else
      {
        break;
      }
    }
  }  
  
  if (i2s->info->xfer.tx_num == i2s->info->xfer.tx_cnt)
  {
    i2s->info->status.tx_busy = 0;

    if (i2s->tx_fifo_size_reg == NO_FIFO)
    {
      XMC_USIC_CH_DisableEvent(i2s->i2s, XMC_USIC_CH_EVENT_TRANSMIT_BUFFER);
    }
    else
    {
      XMC_USIC_CH_TXFIFO_DisableEvent(i2s->i2s, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
    }

    if (i2s->info->cb_event != NULL)
    {
      i2s->info->cb_event(ARM_SAI_EVENT_SEND_COMPLETE);
    }
  }

}

static void I2S_RX_ISR(I2S_RESOURCES  *i2s) 
{
  if (i2s->rx_fifo_size_reg == NO_FIFO)
  {
    i2s->info->xfer.rx_cnt++;
    /* MSB to LSB */
    *(uint8_t *)((uint32_t)i2s->info->xfer.rx_buf ^ ((i2s->info->xfer.data_bits >> 3) - 1)) = (uint8_t)XMC_I2S_CH_GetReceivedData(i2s->i2s);
    i2s->info->xfer.rx_buf++;
  }
  else
  {
    while(XMC_USIC_CH_RXFIFO_IsEmpty(i2s->i2s) == false)
    {
      if (i2s->info->xfer.rx_num > i2s->info->xfer.rx_cnt)
      {
        i2s->info->xfer.rx_cnt++;
        /* MSB to LSB */
        *(uint8_t *)((uint32_t)i2s->info->xfer.rx_buf ^ ((i2s->info->xfer.data_bits >> 3) - 1)) = (uint8_t)XMC_I2S_CH_GetReceivedData(i2s->i2s);
        i2s->info->xfer.rx_buf++;
      }
      else
      {
        break;
      }
    }
    
    if ((i2s->info->xfer.rx_num - i2s->info->xfer.rx_cnt) <= (i2s->rx_fifo_size_num >> 1))
    {
      XMC_USIC_CH_RXFIFO_SetSizeTriggerLimit(i2s->i2s, (XMC_USIC_CH_FIFO_SIZE_t)i2s->rx_fifo_size_reg, i2s->info->xfer.rx_num - i2s->info->xfer.rx_cnt - 1U);
    }
  }

  if (i2s->info->xfer.rx_num == i2s->info->xfer.rx_cnt)
  {
    if (i2s->tx_fifo_size_reg == NO_FIFO)
    {
      XMC_USIC_CH_DisableEvent(i2s->i2s, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE | XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
    }
    else
    {
      XMC_USIC_CH_RXFIFO_DisableEvent(i2s->i2s, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
    }
    

    i2s->info->status.rx_busy = 0;
    i2s->info->cb_event(ARM_SAI_EVENT_RECEIVE_COMPLETE);
  }
}

#if (RTE_I2S0 != 0)
void I2S0_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[0]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[0]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[0]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[0]);
  }
  if (i2s[0]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[0]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[0]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[0]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[0]);
  }
#else
  if (i2s[0]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[0]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[0]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[0]);
  }
  if (i2s[0]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[0]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[0]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[0]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[0]);
  }
#endif

#if RTE_I2S0_TX_FIFO_SIZE == NO_FIFO
  if (i2s[0]->info->status.tx_busy && ((XMC_UART_CH_GetStatusFlag(i2s[0]->i2s) & XMC_UART_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_UART_CH_ClearStatusFlag(i2s[0]->i2s, XMC_UART_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[0]);
  }
#else
  if (i2s[0]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[0]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[0]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[0]);
  }
#endif
}
#endif

#if (RTE_I2S1 != 0)
void I2S1_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[1]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[1]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[1]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[1]);
  }
  if (i2s[1]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[1]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[1]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[1]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[1]);
  }
#else
  if (i2s[1]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[1]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[1]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[1]);
  }
  if (i2s[1]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[1]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[1]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[1]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[1]);
  }
#endif

#if RTE_I2S1_TX_FIFO_SIZE == NO_FIFO
  if (i2s[1]->info->status.tx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[1]->i2s) & XMC_I2S_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[1]->i2s, XMC_I2S_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[1]);
  }
#else
  if (i2s[1]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[1]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[1]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[1]);
  }
#endif
}
#endif

#if (RTE_I2S2 != 0)
void I2S2_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[2]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[2]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[2]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[2]);
  }
  if (i2s[2]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[2]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[2]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[2]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[2]);
  }
#else
  if (i2s[2]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[2]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[2]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[2]);
  }
  if (i2s[2]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[2]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[2]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[2]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[2]);
  }
#endif

#if RTE_I2S1_TX_FIFO_SIZE == NO_FIFO
  if (i2s[2]->info->status.tx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[2]->i2s) & XMC_I2S_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[2]->i2s, XMC_I2S_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[2]);
  }
#else
  if (i2s[2]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[2]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[2]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[2]);
  }
#endif
}
#endif

#if (RTE_I2S3 != 0)
void I2S3_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[3]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[3]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[3]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[3]);
  }
  if (i2s[3]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[3]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[3]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[3]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[3]);
  }
#else
  if (i2s[3]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[3]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[3]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[3]);
  }
  if (i2s[3]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[3]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[3]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[3]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[3]);
  }
#endif

#if RTE_I2S1_TX_FIFO_SIZE == NO_FIFO
  if (i2s[3]->info->status.tx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[3]->i2s) & XMC_I2S_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[3]->i2s, XMC_I2S_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[3]);
  }
#else
  if (i2s[3]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[3]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[3]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[3]);
  }
#endif
}
#endif

#if (RTE_I2S4 != 0)
void I2S4_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[4]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[4]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[4]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[4]);
  }
  if (i2s[4]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[4]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[4]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[4]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[4]);
  }
#else
  if (i2s[4]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[4]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[4]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[4]);
  }
  if (i2s[4]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[4]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[4]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[4]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[4]);
  }
#endif

#if RTE_I2S1_TX_FIFO_SIZE == NO_FIFO
  if (i2s[4]->info->status.tx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[4]->i2s) & XMC_I2S_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[4]->i2s, XMC_I2S_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[4]);
  }
#else
  if (i2s[4]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[4]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[4]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[4]);
  }
#endif
}
#endif

#if (RTE_I2S5 != 0)
void I2S5_ISR(void)
{
#if RTE_I2S0_RX_FIFO_SIZE == NO_FIFO
  if (i2s[5]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[5]->i2s) & XMC_I2S_CH_EVENT_STANDARD_RECEIVE) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[5]->i2s, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
    I2S_RX_ISR(i2s[5]);
  }
  if (i2s[5]->info->status.rx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[5]->i2s) & XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE) != 0) && !i2s[5]->info->xfer.mono_mode)
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[5]->i2s, XMC_I2S_CH_EVENT_ALTERNATIVE_RECEIVE);
    I2S_RX_ISR(i2s[5]);
  }
#else
  if (i2s[5]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[5]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[5]->i2s, XMC_USIC_CH_RXFIFO_EVENT_STANDARD);
    I2S_RX_ISR(i2s[5]);
  }
  if (i2s[5]->info->status.rx_busy && ((XMC_USIC_CH_RXFIFO_GetEvent(i2s[5]->i2s) & XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE) != 0) && !i2s[5]->info->xfer.mono_mode)
  {
    XMC_USIC_CH_RXFIFO_ClearEvent(i2s[5]->i2s, XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
    I2S_RX_ISR(i2s[5]);
  }
#endif

#if RTE_I2S1_TX_FIFO_SIZE == NO_FIFO
  if (i2s[5]->info->status.tx_busy && ((XMC_I2S_CH_GetStatusFlag(i2s[5]->i2s) & XMC_I2S_CH_EVENT_TRANSMIT_BUFFER) != 0))
  {
    XMC_I2S_CH_ClearStatusFlag(i2s[5]->i2s, XMC_I2S_CH_EVENT_TRANSMIT_BUFFER);
    I2S_TX_ISR(i2s[5]);
  }
#else
  if (i2s[5]->info->status.tx_busy && ((XMC_USIC_CH_TXFIFO_GetEvent(i2s[5]->i2s) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) != 0))
  {
    XMC_USIC_CH_TXFIFO_ClearEvent(i2s[5]->i2s, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
    I2S_TX_ISR(i2s[5]);
  }
#endif
}
#endif

#if (RTE_I2S0 != 0)
ARM_DRIVER_SAI Driver_SAI0 = 
{
    I2SX_GetVersion,
    I2S0_GetCapabilities,
    I2S0_Initialize,
    I2S0_Uninitialize,
    I2S0_PowerControl,
    I2S0_Send,
    I2S0_Receive,
    I2S0_GetTxCount,
    I2S0_GetRxCount,
    I2S0_Control,
    I2S0_GetStatus
};
#endif

#if (RTE_I2S1 != 0)
ARM_DRIVER_SAI Driver_SAI1 = 
{
    I2SX_GetVersion,
    I2S1_GetCapabilities,
    I2S1_Initialize,
    I2S1_Uninitialize,
    I2S1_PowerControl,
    I2S1_Send,
    I2S1_Receive,
    I2S1_GetTxCount,
    I2S1_GetRxCount,
    I2S1_Control,
    I2S1_GetStatus
};
#endif

#if (RTE_I2S2 != 0)
ARM_DRIVER_SAI Driver_SAI2 = 
{
    I2SX_GetVersion,
    I2S2_GetCapabilities,
    I2S2_Initialize,
    I2S2_Uninitialize,
    I2S2_PowerControl,
    I2S2_Send,
    I2S2_Receive,
    I2S2_GetTxCount,
    I2S2_GetRxCount,
    I2S2_Control,
    I2S2_GetStatus
};
#endif

#if (RTE_I2S3 != 0)
ARM_DRIVER_SAI Driver_SAI3 = 
{
    I2SX_GetVersion,
    I2S3_GetCapabilities,
    I2S3_Initialize,
    I2S3_Uninitialize,
    I2S3_PowerControl,
    I2S3_Send,
    I2S3_Receive,
    I2S3_GetTxCount,
    I2S3_GetRxCount,
    I2S3_Control,
    I2S3_GetStatus
};
#endif

#if (RTE_I2S4 != 0)
ARM_DRIVER_SAI Driver_SAI4 = 
{
    I2SX_GetVersion,
    I2S4_GetCapabilities,
    I2S4_Initialize,
    I2S4_Uninitialize,
    I2S4_PowerControl,
    I2S4_Send,
    I2S4_Receive,
    I2S4_GetTxCount,
    I2S4_GetRxCount,
    I2S4_Control,
    I2S4_GetStatus
};
#endif

#if (RTE_I2S5 != 0)
ARM_DRIVER_SAI Driver_SAI5 = 
{
    I2SX_GetVersion,
    I2S5_GetCapabilities,
    I2S5_Initialize,
    I2S5_Uninitialize,
    I2S5_PowerControl,
    I2S5_Send,
    I2S5_Receive,
    I2S5_GetTxCount,
    I2S5_GetRxCount,
    I2S5_Control,
    I2S5_GetStatus
};
#endif
