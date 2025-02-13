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
 * @file I2C.c
 * @date 30 July, 2016
 * @version 2.6
 *
 * @brief I2C Driver for Infineon XMC devices
 *
 * History
 *
 * Version 2.6 Fixed IRQ handlers names
 *
 * Version 2.5 Fixed IRQ handlers names
 *
 * Version 2.4 Adapted to xmc1400 devices<br>
 *
 * Version 2.3 Added missing events at the end of slave operation. <br>
 *
 * Version 2.2 Added fixed to prevent race conditions 
 *               and Initialize/Uninitialize, Power Control 
 *               guidelines related modifications <br>
 *
 * Version 1.0 Initial version<br>
 */

#include "I2C.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_I2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,6)   /* driver version */

#define I2C_MODE_MASTER 1
#define I2C_MODE_SLAVE  0 

#define I2C_DIR_WRITE   0
#define I2C_DIR_READ    1

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = 
{
  ARM_I2C_API_VERSION,
  ARM_I2C_DRV_VERSION
};

// Default I2C initialization
static const XMC_I2C_CH_CONFIG_t i2c_default_config =
{
  .baudrate = 100000U,
  .address =  0U,
};

#if (defined(RTE_Drivers_I2C) && \
     (RTE_I2C0 == 0) && \
     (RTE_I2C1 == 0) && \
     (RTE_I2C2 == 0) && \
     (RTE_I2C3 == 0) && \
     (RTE_I2C4 == 0) && \
     (RTE_I2C5 == 0))
#error "I2C not configured in RTE_Device.h!"
#endif

static const ARM_I2C_CAPABILITIES DriverCapabilities =
{
  0   //< supports 10-bit addressing
};


/* I2C0 */
#if (RTE_I2C0 != 0)

static I2C_INFO I2C0_Info = {0};
static XMC_GPIO_CONFIG_t I2C0_sda_conf; 
static XMC_GPIO_CONFIG_t I2C0_scl_conf; 

/* I2C0 Resources */
I2C_RESOURCES I2C0_Resources = 
{
  RTE_I2C0_TX_PORT,
  RTE_I2C0_TX_AF,
  RTE_I2C0_RX_PORT,
  &I2C0_sda_conf,
  RTE_I2C0_RX_INPUT,
  RTE_I2C0_CLK_OUTPUT_PORT,
  RTE_I2C0_CLK_AF,
  RTE_I2C0_CLK_INPUT_PORT,
  &I2C0_scl_conf,
  RTE_I2C0_CLK_INPUT,
  XMC_I2C0_CH0,
  (IRQn_Type)USIC0_0_IRQn,
  RTE_I2C0_TX_FIFO_SIZE_NUM,
  RTE_I2C0_TX_FIFO_SIZE,
  RTE_I2C0_RX_FIFO_SIZE_NUM,
  RTE_I2C0_RX_FIFO_SIZE,
  &I2C0_Info
};
#endif

/* I2C1 */
#if (RTE_I2C1 != 0)

static I2C_INFO I2C1_Info = {0};
static XMC_GPIO_CONFIG_t I2C1_sda_conf; 
static XMC_GPIO_CONFIG_t I2C1_scl_conf; 

/* I2C1 Resources */
I2C_RESOURCES I2C1_Resources = 
{
  RTE_I2C1_TX_PORT,
  RTE_I2C1_TX_AF,
  RTE_I2C1_RX_PORT,
  &I2C1_sda_conf,
  RTE_I2C1_RX_INPUT,
  RTE_I2C1_CLK_OUTPUT_PORT,
  RTE_I2C1_CLK_AF,
  RTE_I2C1_CLK_INPUT_PORT,
  &I2C1_scl_conf,
  RTE_I2C1_CLK_INPUT,
  XMC_I2C0_CH1,
  (IRQn_Type)USIC0_1_IRQn,
  RTE_I2C1_TX_FIFO_SIZE_NUM,
  RTE_I2C1_TX_FIFO_SIZE,
  RTE_I2C1_RX_FIFO_SIZE_NUM,
  RTE_I2C1_RX_FIFO_SIZE,
  &I2C1_Info
};
#endif

/* I2C2 */
#if (RTE_I2C2 != 0)

static I2C_INFO I2C2_Info = {0};
static XMC_GPIO_CONFIG_t I2C2_sda_conf; 
static XMC_GPIO_CONFIG_t I2C2_scl_conf; 

/* I2C2 Resources */
I2C_RESOURCES I2C2_Resources = 
{
  RTE_I2C2_TX_PORT,
  RTE_I2C2_TX_AF,
  RTE_I2C2_RX_PORT,
  &I2C2_sda_conf,
  RTE_I2C2_RX_INPUT,
  RTE_I2C2_CLK_OUTPUT_PORT,
  RTE_I2C2_CLK_AF,
  RTE_I2C2_CLK_INPUT_PORT,
  &I2C2_scl_conf,
  RTE_I2C2_CLK_INPUT,
  XMC_I2C1_CH0,
  (IRQn_Type)USIC1_0_IRQn,
  RTE_I2C2_TX_FIFO_SIZE_NUM,
  RTE_I2C2_TX_FIFO_SIZE,
  RTE_I2C2_RX_FIFO_SIZE_NUM,
  RTE_I2C2_RX_FIFO_SIZE,
  &I2C2_Info
};
#endif

/* I2C3 */
#if (RTE_I2C3 != 0)

static I2C_INFO I2C3_Info = {0};
static XMC_GPIO_CONFIG_t I2C3_sda_conf; 
static XMC_GPIO_CONFIG_t I2C3_scl_conf; 

/* I2C3 Resources */
I2C_RESOURCES I2C3_Resources = 
{
  RTE_I2C3_TX_PORT,
  RTE_I2C3_TX_AF,
  RTE_I2C3_RX_PORT,
  &I2C3_sda_conf,
  RTE_I2C3_RX_INPUT,
  RTE_I2C3_CLK_OUTPUT_PORT,
  RTE_I2C3_CLK_AF,
  RTE_I2C3_CLK_INPUT_PORT,
  &I2C3_scl_conf,
  RTE_I2C3_CLK_INPUT,
  XMC_I2C1_CH1,
  (IRQn_Type)USIC1_1_IRQn,
  RTE_I2C3_TX_FIFO_SIZE_NUM,
  RTE_I2C3_TX_FIFO_SIZE,
  RTE_I2C3_RX_FIFO_SIZE_NUM,
  RTE_I2C3_RX_FIFO_SIZE,
  &I2C3_Info
};
#endif


/* I2C4 */
#if (RTE_I2C4 != 0)

static I2C_INFO I2C4_Info = {0};
static XMC_GPIO_CONFIG_t I2C4_sda_conf; 
static XMC_GPIO_CONFIG_t I2C4_scl_conf; 

/* I2C4 Resources */
I2C_RESOURCES I2C4_Resources = 
{
  RTE_I2C4_TX_PORT,
  RTE_I2C4_TX_AF,
  RTE_I2C4_RX_PORT,
  &I2C4_sda_conf,
  RTE_I2C4_RX_INPUT,
  RTE_I2C4_CLK_OUTPUT_PORT,
  RTE_I2C4_CLK_AF,
  RTE_I2C4_CLK_INPUT_PORT,
  &I2C4_scl_conf,
  RTE_I2C4_CLK_INPUT,
  XMC_I2C2_CH0,
  (IRQn_Type)USIC2_0_IRQn,
  RTE_I2C4_TX_FIFO_SIZE_NUM,
  RTE_I2C4_TX_FIFO_SIZE,
  RTE_I2C4_RX_FIFO_SIZE_NUM,
  RTE_I2C4_RX_FIFO_SIZE,
  &I2C4_Info
};
#endif


/* I2C5 */
#if (RTE_I2C5 != 0)

static I2C_INFO I2C5_Info = {0};
static XMC_GPIO_CONFIG_t I2C5_sda_conf; 
static XMC_GPIO_CONFIG_t I2C5_scl_conf; 

/* I2C4 Resources */
I2C_RESOURCES I2C5_Resources = 
{
  RTE_I2C5_TX_PORT,
  RTE_I2C5_TX_AF,
  RTE_I2C5_RX_PORT,
  &I2C5_sda_conf,
  RTE_I2C5_RX_INPUT,
  RTE_I2C5_CLK_OUTPUT_PORT,
  RTE_I2C5_CLK_AF,
  RTE_I2C5_CLK_INPUT_PORT,
  &I2C5_scl_conf,
  RTE_I2C5_CLK_INPUT,
  XMC_I2C2_CH1,
  (IRQn_Type)USIC2_1_IRQn,
  RTE_I2C5_TX_FIFO_SIZE_NUM,
  RTE_I2C5_TX_FIFO_SIZE,
  RTE_I2C5_RX_FIFO_SIZE_NUM,
  RTE_I2C5_RX_FIFO_SIZE,
  &I2C5_Info
};

#endif


/* I2C Resources */
static I2C_RESOURCES  *i2c[6] = 
{
#if (RTE_I2C0 != 0)
  &I2C0_Resources,
#else
  NULL,
#endif
#if (RTE_I2C1 != 0)
  &I2C1_Resources,
#else
  NULL,
#endif
#if (RTE_I2C2 != 0)
  &I2C2_Resources,
#else
  NULL,
#endif
#if (RTE_I2C3 != 0)
  &I2C3_Resources,
#else
  NULL,
#endif
#if (RTE_I2C4 != 0)
  &I2C4_Resources,
#else
  NULL,
#endif
#if (RTE_I2C5 != 0)
  &I2C5_Resources,
#else
  NULL,
#endif
};

/**
  \fn          ARM_DRV_VERSION I2CX_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION I2CX_GetVersion(void) 
{
  return DriverVersion;
}


/**
  \fn          ARM_I2C_CAPABILITIES I2C_GetCapabilities (I2C_RESOURCES  *i2c)
  \brief       Get driver capabilities.
  \param[in]   i2c    Pointer to I2C resources
  \return      \ref I2C_CAPABILITIES
*/
static inline ARM_I2C_CAPABILITIES I2C_GetCapabilities(I2C_RESOURCES *const i2c) 
{
  (void)i2c;
  return DriverCapabilities;
}

#if (RTE_I2C0 != 0)
static ARM_I2C_CAPABILITIES I2C0_GetCapabilities(void)
{
  return I2C_GetCapabilities(i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
static ARM_I2C_CAPABILITIES I2C1_GetCapabilities(void) 
{
  return I2C_GetCapabilities(i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
static ARM_I2C_CAPABILITIES I2C2_GetCapabilities(void) 
{
  return I2C_GetCapabilities(i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
static ARM_I2C_CAPABILITIES I2C3_GetCapabilities(void) 
{
  return I2C_GetCapabilities(i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
static ARM_I2C_CAPABILITIES I2C4_GetCapabilities(void)
{
  return I2C_GetCapabilities(i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
static ARM_I2C_CAPABILITIES I2C5_GetCapabilities(void) 
{
  return I2C_GetCapabilities(i2c[5]);
}
#endif

/**
  \fn          ARM_I2C_STATUS I2C_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES  *i2c)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event    Pointer to \ref ARM_I2C_SignalEvent
  \param[in]   i2c    Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static int32_t I2C_Initialize(ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *const i2c)
{
  if (((i2c->info->flags) & I2C_FLAG_INITIALIZED) == 0)
  {
    // Initialize I2C Run-Time Resources
    i2c->info->cb_event = cb_event;                                        
    
    /* Clear driver status */
    i2c->info->status.arbitration_lost = 0;
    i2c->info->status.busy = 0;
    i2c->info->status.bus_error = 0;
    i2c->info->status.direction = 0;
    i2c->info->pending = false;
    i2c->info->status.general_call=0;
    i2c->info->status.mode = I2C_MODE_MASTER;
    i2c->info->flags |= I2C_FLAG_INITIALIZED;
  }
  
  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
static int32_t I2C1_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
static int32_t I2C2_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
static int32_t I2C3_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
static int32_t I2C4_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
static int32_t I2C5_Initialize(ARM_I2C_SignalEvent_t cb_event) 
{
  return I2C_Initialize(cb_event, i2c[5]);
}
#endif


  /**
  \fn          int32_t I2C_Uninitialize (I2C_RESOURCES *i2c)
  \brief       De-initialize I2C Interface.
  \param[in]   i2c     Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_Uninitialize (I2C_RESOURCES *const i2c) 
{
  // Reset I2C status flags
  i2c->info->flags = 0;
  i2c->info->flags &= ~I2C_FLAG_INITIALIZED;
  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[0]);
}
#endif  
#if (RTE_I2C1 != 0)
static int32_t I2C1_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static int32_t I2C2_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static int32_t I2C3_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static int32_t I2C4_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static int32_t I2C5_Uninitialize(void) 
{
  return I2C_Uninitialize(i2c[5]);
}
#endif
/**
  \fn          ARM_I2C_STATUS I2C_PowerControl (ARM_POWER_STATE  state,
                                                  I2C_RESOURCES   *i2c)
  \brief       Controls I2C Interface Power.
  \param[in]   state    Power state
  \param[in]   i2c Pointer to I2C resources
  \return      \ref I2C_STATUS
*/
static int32_t I2C_PowerControl(ARM_POWER_STATE state, I2C_RESOURCES *const i2c)
{
  if (state == ARM_POWER_FULL) 
  {
    if ((i2c->info->flags & I2C_FLAG_INITIALIZED) == 0U) 
    {
      return ARM_DRIVER_ERROR;
    }
    if ((i2c->info->flags & I2C_FLAG_POWERED)!= 0U) 
    {
      return ARM_DRIVER_OK;
    }
    
    if (i2c->tx_fifo_size_num > i2c->rx_fifo_size_num)
    {
      i2c->info->tx_fifo_pointer = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 32 : 0;
      i2c->info->rx_fifo_pointer = i2c->info->tx_fifo_pointer + i2c->tx_fifo_size_num;
    }
    else
    {
      i2c->info->rx_fifo_pointer = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 32 : 0;
      i2c->info->tx_fifo_pointer = i2c->info->rx_fifo_pointer + i2c->rx_fifo_size_num;
    }
    
    XMC_I2C_CH_Init(i2c->i2c, &i2c_default_config);
    XMC_USIC_CH_TXFIFO_Configure(i2c->i2c, i2c->info->tx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2c->tx_fifo_size_reg, 1U);
    XMC_I2C_CH_SetInputSource(i2c->i2c, (XMC_I2C_CH_INPUT_t)0U, i2c->sda_pin_input);  
    i2c->pin_sda_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN | i2c->sda_pin_af_output);
    XMC_I2C_CH_SetInputSource(i2c->i2c,(XMC_I2C_CH_INPUT_t)1U,i2c->scl_pin_input);  
    i2c->pin_scl_config->mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN | i2c->scl_pin_af_output);
    XMC_I2C_CH_Start(i2c->i2c); 
  
    // Configure pin  
    XMC_GPIO_Init(i2c->sda_tx_port.port,i2c->sda_tx_port.pin, i2c->pin_sda_config);                                           
    XMC_GPIO_Init(i2c->scl_output_port.port,i2c->scl_output_port.pin, i2c->pin_scl_config);                                                       

    NVIC_ClearPendingIRQ(i2c->irq_num);
    
#if(UC_FAMILY == XMC4)
    NVIC_SetPriority(i2c->irq_num, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U,0U)); 
#else
    NVIC_SetPriority(i2c->irq_num,3U); 
#endif

#if (UC_SERIES == XMC14)    
    XMC_SCU_SetInterruptControl(i2c->irq_num,(XMC_SCU_IRQCTRL_t)(((i2c->irq_num) << 8U) | 3U));
#endif

    NVIC_EnableIRQ(i2c->irq_num);

    i2c->info->flags |= I2C_FLAG_POWERED;
  } 
  else if(state == ARM_POWER_OFF) 
  {
    NVIC_DisableIRQ(i2c->irq_num);

    // Disable peripheral, terminates any pending data transfers
    while (XMC_I2C_CH_Stop(i2c->i2c) == XMC_I2C_CH_STATUS_BUSY)
    {
      ;
    }

    XMC_USIC_CH_TXFIFO_Configure(i2c->i2c, 0, XMC_USIC_CH_FIFO_DISABLED, 0U); 
    XMC_USIC_CH_RXFIFO_Configure(i2c->i2c, 0, XMC_USIC_CH_FIFO_DISABLED, 0U); 

    i2c->info->flags &= ~I2C_FLAG_POWERED;
  } 
  else 
  {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[0]);
}
#endif
#if (RTE_I2C1 != 0)
static int32_t I2C1_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static int32_t I2C2_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static int32_t I2C3_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static int32_t I2C4_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static int32_t I2C5_PowerControl(ARM_POWER_STATE state) 
{
  return I2C_PowerControl(state, i2c[5]);
}
#endif
/**
  \fn          int32_t I2C_MasterTransmit (uint32_t       addr,
                                            const uint8_t *data,
                                            uint32_t       num,
                                            bool           xfer_pending,
                                            I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Master.
  \param[in]   addr          Slave address (7-bit)
  \param[in]   data          Pointer to buffer with data to transmit to I2C Slave
  \param[in]   num           Number of data bytes to transmit
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, I2C_RESOURCES *const i2c) 
{
  uint8_t SRno;
  
  if ((data == NULL) || (num == 0)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((i2c->info->flags & I2C_FLAG_POWERED) == 0)
  {
    // I2C is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (i2c->info->status.busy)
  {
    /* Transfer operation in progress, or Slave stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }
 
  XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ERROR |
                                       XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED |
                                       XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST |
                                       XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
  
  /* Set control variables */
  i2c->info->sla = addr;
  i2c->info->xfer.tx_buf            = (uint8_t *)data;
  i2c->info->xfer.tx_num            = num;
  i2c->info->xfer.tx_cnt            = 0;

  /* Update driver status */
  i2c->info->status.busy             = 1;
  i2c->info->status.mode             = I2C_MODE_MASTER;
  i2c->info->status.direction        = I2C_DIR_WRITE;
  i2c->info->status.arbitration_lost = 0;
  i2c->info->status.bus_error        = 0;

  SRno = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2c->i2c) & 0x4000U) != 0) ? 1 : 0;
#endif

  XMC_USIC_CH_TXFIFO_Flush(i2c->i2c); 
  
  XMC_I2C_CH_EnableEvent(i2c->i2c, XMC_I2C_CH_EVENT_ACK |
                                   XMC_I2C_CH_EVENT_NACK |
                                   XMC_I2C_CH_EVENT_ARBITRATION_LOST |
                                   XMC_I2C_CH_EVENT_ERROR);
  
  XMC_I2C_CH_SetInterruptNodePointer(i2c->i2c, SRno);
  
  if(i2c->info->pending == false)
  {  
    /* send slave address and command to write */
    XMC_I2C_CH_MasterStart(i2c->i2c, addr, XMC_I2C_CH_CMD_WRITE);
  }
  else
  {
    XMC_I2C_CH_MasterRepeatedStart(i2c->i2c, addr, XMC_I2C_CH_CMD_WRITE);
  }
  
  i2c->info->pending = xfer_pending;
  
  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
static int32_t I2C1_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
static int32_t I2C2_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
static int32_t I2C3_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
static int32_t I2C4_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
static int32_t I2C5_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
  return I2C_MasterTransmit(addr, data, num, xfer_pending, i2c[5]);
}
#endif


/**
  \fn          int32_t I2C_MasterReceive (uint32_t       addr,
                                           uint8_t       *data,
                                           uint32_t       num,
                                           bool           xfer_pending,
                                           I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[out]  data          Pointer to buffer for data to receive from I2C Slave
  \param[in]   num           Number of data bytes to receive
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, I2C_RESOURCES *const i2c) 
{

  uint8_t SRno;

  if ((data == NULL) || (num == 0))
  { 
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((i2c->info->flags & I2C_FLAG_POWERED) == 0)
  {
    // I2C is not powered 
    return ARM_DRIVER_ERROR;
  }

  // Check if receiver is busy
  if (i2c->info->status.busy == 1)
  {
    return ARM_DRIVER_ERROR_BUSY;
  }

  SRno = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2c->i2c) & 0x4000U) != 0) ? 1 : 0;
#endif

  XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ERROR |
                                       XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED |
                                       XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST |
                                       XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

  /* Set control variables */
  i2c->info->sla  = addr;
  i2c->info->xfer.rx_buf = data;
  i2c->info->xfer.rx_num  = num;
  i2c->info->xfer.rx_cnt  = 0;
  i2c->info->xfer.tx_num  = num;
  i2c->info->xfer.tx_cnt  = 0;
  
  /* Update driver status */
  i2c->info->status.busy             = 1;
  i2c->info->status.mode             = I2C_MODE_MASTER;
  i2c->info->status.direction        = I2C_DIR_READ;
  i2c->info->status.arbitration_lost = 0;
  i2c->info->status.bus_error        = 0;

  if(i2c->rx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(i2c->i2c, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | 
                                      XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
    XMC_USIC_CH_SetInterruptNodePointer(i2c->i2c,XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno);
    XMC_USIC_CH_SetInterruptNodePointer(i2c->i2c,XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno);
  }
  else
  {
    XMC_USIC_CH_TXFIFO_Flush(i2c->i2c);   
    XMC_USIC_CH_RXFIFO_Flush(i2c->i2c);     
    XMC_USIC_CH_RXFIFO_EnableEvent(i2c->i2c, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD |
                                             XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE); 
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2c->i2c,XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2c->i2c,XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno);
  
    if(num <= i2c->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(i2c->i2c,i2c->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg, i2c->info->xfer.rx_num - 1U); 
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(i2c->i2c,i2c->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg, i2c->rx_fifo_size_num - 1U); 
    }
  } 
  
  XMC_I2C_CH_EnableEvent(i2c->i2c, XMC_I2C_CH_EVENT_ACK |
                                   XMC_I2C_CH_EVENT_NACK |
                                   XMC_I2C_CH_EVENT_ARBITRATION_LOST | 
                                   XMC_I2C_CH_EVENT_ERROR);                     

  XMC_I2C_CH_SetInterruptNodePointer(i2c->i2c, SRno);
  
  if(i2c->info->pending == false)
  {  
    /* send slave address and command to write */
    XMC_I2C_CH_MasterStart(i2c->i2c, addr, XMC_I2C_CH_CMD_READ);
  }
  else
  {
    XMC_I2C_CH_MasterRepeatedStart(i2c->i2c, addr, XMC_I2C_CH_CMD_READ);
  }
  
  i2c->info->pending = xfer_pending;

  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
static int32_t I2C1_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
static int32_t I2C2_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
static int32_t I2C3_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
static int32_t I2C4_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
static int32_t I2C5_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
  return I2C_MasterReceive(addr, data, num, xfer_pending, i2c[5]);
}
#endif

  
/**
  \fn          int32_t I2C_SlaveTransmit (const uint8_t *data,
                                            uint32_t       num,
                                            I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Slave.
  \param[in]   data          Pointer to buffer with data to transmit to I2C Slave
  \param[in]   num           Number of data bytes to transmit
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveTransmit(const uint8_t *data, uint32_t num, I2C_RESOURCES *const i2c) 
{

  uint8_t SRno = 0U;
  
  if ((data == NULL) || (num == 0)) 
  {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((i2c->info->flags & I2C_FLAG_POWERED) == 0)
  {
    // I2C is not powered 
    return ARM_DRIVER_ERROR;
  }

  if (i2c->info->status.busy)
  {
    /* Transfer operation in progress, or Slave stalled */
    return ARM_DRIVER_ERROR_BUSY;
  }

  SRno = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 1 : 0;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2c->i2c) & 0x4000U) != 0) ? 1 : 0;
#endif

  XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ERROR |
                                       XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED |
                                       XMC_I2C_CH_STATUS_FLAG_SLAVE_READ_REQUESTED |
                                       XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST |
                                       XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
  
  /* Set control variables */
  i2c->info->xfer.tx_buf    = (uint8_t *)data;
  i2c->info->xfer.tx_num    = num;
  i2c->info->xfer.tx_cnt    = 0;

  /* Update driver status */
  i2c->info->status.busy             = 1;
  i2c->info->status.mode             = I2C_MODE_SLAVE;
  i2c->info->status.direction        = I2C_DIR_WRITE;
  i2c->info->status.arbitration_lost = 0;
  i2c->info->status.bus_error        = 0;
 
  XMC_I2C_CH_EnableEvent(i2c->i2c, XMC_I2C_CH_EVENT_SLAVE_READ_REQUEST | 
                                   XMC_I2C_CH_EVENT_NACK | 
                                   XMC_I2C_CH_EVENT_ARBITRATION_LOST | 
                                   XMC_I2C_CH_EVENT_ERROR);
 
  XMC_I2C_CH_SetInterruptNodePointer(i2c->i2c, SRno);
 
  if(i2c->tx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(i2c->i2c, XMC_USIC_CH_EVENT_TRANSMIT_BUFFER) ;
    XMC_USIC_CH_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_INTERRUPT_NODE_POINTER_TRANSMIT_BUFFER, SRno);
  }
  else
  {
    XMC_USIC_CH_TXFIFO_Flush(i2c->i2c); 
    XMC_USIC_CH_TXFIFO_EnableEvent(i2c->i2c, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD); 
    XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno); 
  }
  
  if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_SLAVE_TRANSMIT);
  
  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
static int32_t I2C1_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
static int32_t I2C2_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
static int32_t I2C3_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
static int32_t I2C4_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
static int32_t I2C5_SlaveTransmit(const uint8_t *data, uint32_t num) 
{
  return I2C_SlaveTransmit(data, num, i2c[5]);
}
#endif  
  
  
/**
  \fn          int32_t I2C_SlaveReceive (uint8_t       *data,
                                          uint32_t       num,
                                          I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Slave.
  \param[out]  data  Pointer to buffer for data to receive from I2C Master
  \param[in]   num   Number of data bytes to receive
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveReceive(uint8_t *data, uint32_t num, I2C_RESOURCES *const i2c) 
{

  uint8_t SRno;

  if ((data == NULL) || (num == 0))
  { 
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((i2c->info->flags & I2C_FLAG_POWERED) == 0)
  {
    // I2C is not powered 
    return ARM_DRIVER_ERROR;
  }

  // Check if receiver is busy
  if (i2c->info->status.busy == 1)
  {
    return ARM_DRIVER_ERROR_BUSY;
  }

  SRno = (((uint32_t)(i2c->i2c) & 0x200U) != 0) ? 1 : 1;
#if (UC_SERIES == XMC14)
  SRno += (((uint32_t)(i2c->i2c) & 0x4000U) != 0) ? 1 : 0;
#endif

  XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ERROR |
                                       XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED |
                                       XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST |
                                       XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
  
  XMC_I2C_CH_EnableEvent(i2c->i2c, XMC_I2C_CH_EVENT_NACK | 
                                   XMC_I2C_CH_EVENT_ARBITRATION_LOST |
                                   XMC_I2C_CH_EVENT_ERROR);
 
  /* Set control variables */
  i2c->info->xfer.rx_buf    = data;
  i2c->info->xfer.rx_num     = num;
  i2c->info->xfer.rx_cnt     = 0;

  /* Update driver status */
  i2c->info->status.busy             = 1;
  i2c->info->status.mode             = I2C_MODE_SLAVE;
  i2c->info->status.direction        = I2C_DIR_READ;
  i2c->info->status.arbitration_lost = 0;
  i2c->info->status.bus_error        = 0;
  
  if(i2c->rx_fifo_size_reg == NO_FIFO)
  {
    XMC_USIC_CH_EnableEvent(i2c->i2c, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE) ;
    XMC_USIC_CH_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, SRno);
    XMC_USIC_CH_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, SRno);
  }
  else
  {
    XMC_USIC_CH_RXFIFO_Flush(i2c->i2c);     
    XMC_USIC_CH_RXFIFO_EnableEvent(i2c->i2c, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | 
                                             XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, SRno);
    XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(i2c->i2c, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, SRno);
  
    if(num <= i2c->rx_fifo_size_num)
    {
      XMC_USIC_CH_RXFIFO_Configure(i2c->i2c, i2c->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg, i2c->info->xfer.rx_num - 1U); 
    }
    else
    {
      XMC_USIC_CH_RXFIFO_Configure(i2c->i2c, i2c->info->rx_fifo_pointer, (XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg, i2c->rx_fifo_size_num - 1U); 
    }   
  } 
  
  if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_SLAVE_RECEIVE); 
  
  return ARM_DRIVER_OK; 
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[0]);
}
#endif
#if (RTE_I2C1 != 0)
static int32_t I2C1_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static int32_t I2C2_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static int32_t I2C3_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static int32_t I2C4_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static int32_t I2C5_SlaveReceive(uint8_t *data, uint32_t num) 
{
  return I2C_SlaveReceive(data, num, i2c[5]);
}
#endif

/**
  \fn          int32_t I2C_GetDataCount (I2C_RESOURCES *i2c)
  \brief       Get received data count.
  \param[in]   i2c     Pointer to I2C resources
  \return      number of data items 
*/
static inline int32_t I2C_GetDataCount(I2C_RESOURCES *const i2c)
{
  return i2c->info->xfer.rx_cnt;;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[0]);
}
#endif
#if (RTE_I2C1 != 0)
static int32_t I2C1_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static int32_t I2C2_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static int32_t I2C3_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static int32_t I2C4_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static int32_t I2C5_GetDataCount(void) 
{
  return I2C_GetDataCount(i2c[5]);
}
#endif

/**
  \fn          ARM_I2C_STATUS I2C_GetStatus (I2C_RESOURCES *i2c)
  \brief       Get I2C status.
  \param[in]   i2c     Pointer to I2C resources
  \return      I2C status \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_GetStatus(I2C_RESOURCES *const i2c) 
{
  ARM_I2C_STATUS status;
  
  status.busy             = i2c->info->status.busy;             
  status.mode             = i2c->info->status.mode; 
  status.direction        = i2c->info->status.direction;
  status.general_call     = i2c->info->status.general_call;
  status.arbitration_lost = i2c->info->status.arbitration_lost;
  
  return status;
}

#if (RTE_I2C0 != 0)
static ARM_I2C_STATUS I2C0_GetStatus(void) 
{
  return I2C_GetStatus(i2c[0]);
}
#endif
#if (RTE_I2C1 != 0)
static ARM_I2C_STATUS I2C1_GetStatus(void) 
{
  return I2C_GetStatus(i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static ARM_I2C_STATUS I2C2_GetStatus(void) 
{
  return I2C_GetStatus(i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static ARM_I2C_STATUS I2C3_GetStatus(void) 
{
  return I2C_GetStatus(i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static ARM_I2C_STATUS I2C4_GetStatus(void) 
{
  return I2C_GetStatus(i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static ARM_I2C_STATUS I2C5_GetStatus(void) 
{
  return I2C_GetStatus(i2c[5]);
}
#endif
  

/**
  \fn          int32_t I2C_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c)
  \brief       Control I2C Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   i2c    Pointer to I2C resources
  \return      common \ref execution_status and driver specific \ref i2c_execution_status
*/
static int32_t I2C_Control(uint32_t control, uint32_t arg, I2C_RESOURCES *const i2c) 
{
  if ((i2c->info->flags & I2C_FLAG_POWERED) == 0U) 
  {
    return ARM_DRIVER_ERROR;
  }

  switch (control)
  {
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  
    case ARM_I2C_OWN_ADDRESS:
      // Set Own Slave Address; arg = slave address 
      if (arg != 0) 
      {
        XMC_I2C_CH_SetSlaveAddress(i2c->i2c, arg);
      }
      break;

    case ARM_I2C_BUS_CLEAR:
      // Clear the bus by sending nine clock pulses
      for(int i = 0; i < 9; ++i)
      {
        if(i2c->info->status.mode == I2C_MODE_MASTER)
        {
          XMC_I2C_CH_MasterTransmit(i2c->i2c, (uint8_t)0);
        }
        else
        {
          XMC_I2C_CH_SlaveTransmit(i2c->i2c, (uint8_t)0);
        }
      }
      break;
  
    case ARM_I2C_ABORT_TRANSFER:
      // Aborts the data transfer between Master and Slave for Transmit or Receive
      XMC_I2C_CH_Stop(i2c->i2c);
      i2c->info->status.busy = 0; 
      XMC_I2C_CH_Start(i2c->i2c);
      break;

    case ARM_I2C_BUS_SPEED:
      // Set Bus Speed in bps; arg = value      
      switch (arg) 
      {
        case ARM_I2C_BUS_SPEED_STANDARD:
          XMC_I2C_CH_Stop(i2c->i2c);
          XMC_I2C_CH_SetBaudrate(i2c->i2c, 100000);
          XMC_I2C_CH_Start(i2c->i2c);
          break;
        case ARM_I2C_BUS_SPEED_FAST:
          XMC_I2C_CH_Stop(i2c->i2c);
          XMC_I2C_CH_SetBaudrate(i2c->i2c, 400000);
          XMC_I2C_CH_Start(i2c->i2c);
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }     
      break;  
  } 
  return ARM_DRIVER_OK;
}

#if (RTE_I2C0 != 0)
static int32_t I2C0_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[0]);
}
#endif
#if (RTE_I2C1 != 0)
static int32_t I2C1_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[1]);
}
#endif
#if (RTE_I2C2 != 0)
static int32_t I2C2_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[2]);
}
#endif
#if (RTE_I2C3 != 0)
static int32_t I2C3_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[3]);
}
#endif
#if (RTE_I2C4 != 0)
static int32_t I2C4_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[4]);
}
#endif
#if (RTE_I2C5 != 0)
static int32_t I2C5_Control(uint32_t control, uint32_t arg) 
{
  return I2C_Control(control, arg, i2c[5]);
}
#endif
  
/**
  \fn          void I2C_IRQHandler (I2C_RESOURCES  *i2c)
  \brief       I2C Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_IRQHandler(I2C_RESOURCES *const i2c)
{
  if (i2c->info->status.mode == I2C_MODE_MASTER) // If MASTER (read transaction ACK received)
  {
    if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & (XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED | XMC_I2C_CH_STATUS_FLAG_ERROR)) != 0)
    {
      i2c->info->status.bus_error = 1;
    }

    if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST) != 0)
    {
      i2c->info->status.arbitration_lost = 1; 
    }

    // Transmitter (ACK event)
    if (i2c->info->status.direction == I2C_DIR_WRITE)
    {
      if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) != 0)
      {
        XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
        if (i2c->info->xfer.tx_cnt < i2c->info->xfer.tx_num) 
        {         
          if (i2c->tx_fifo_size_reg == NO_FIFO)
          {
            i2c->info->xfer.tx_cnt++;          
            XMC_I2C_CH_MasterTransmit(i2c->i2c, *(i2c->info->xfer.tx_buf++)); 
          }
          else
          {
            XMC_I2C_CH_DisableDataTransmission(i2c->i2c);

            /* Write to FIFO till Fifo is full */
            while (XMC_USIC_CH_TXFIFO_IsFull(i2c->i2c) == false)
            { 
              if (i2c->info->xfer.tx_num > i2c->info->xfer.tx_cnt) 
              { 
                i2c->info->xfer.tx_cnt++;         
                XMC_USIC_CH_TXFIFO_PutData(i2c->i2c, *(i2c->info->xfer.tx_buf++));  
              }
              else
              {         
                break;
              }
            }

            XMC_I2C_CH_EnableDataTransmission(i2c->i2c);
          }
        }
    
        if (i2c->info->xfer.tx_cnt == i2c->info->xfer.tx_num)
        { 
          /* Disable standard transmit and error event interrupt */
          XMC_I2C_CH_DisableEvent(i2c->i2c, XMC_I2C_CH_EVENT_ACK);
          NVIC_ClearPendingIRQ(i2c->irq_num);
      
          if(i2c->info->pending == false)
          {
            XMC_I2C_CH_MasterStop(i2c->i2c); 
          }
          while(XMC_USIC_CH_GetTransmitBufferStatus(i2c->i2c) & (uint32_t)XMC_USIC_CH_TBUF_STATUS_BUSY);
      
          if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_TRANSFER_DONE);
      
          // Clear TX busy flag
          i2c->info->status.busy = 0;
        }
      }
    }
    // Receiver (standard/alternate receive event)
    else
    {
      if(i2c->rx_fifo_size_reg == NO_FIFO)
      {
        if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & (XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) != 0)
        {
          XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);

          /* Read the data from FIFO buffer */
          i2c->info->xfer.rx_cnt++;   
          *(i2c->info->xfer.rx_buf++) = XMC_I2C_CH_GetReceivedData(i2c->i2c);  
          if (i2c->info->xfer.rx_cnt < i2c->info->xfer.rx_num)
          {
            XMC_I2C_CH_MasterReceiveAck(i2c->i2c);
          }
          else
          {
            XMC_I2C_CH_MasterReceiveNack(i2c->i2c);

            if(i2c->info->pending == false)
            {
              XMC_I2C_CH_MasterStop(i2c->i2c); 
            } 
        
            // Clear RX busy flag and set receive transfer complete event
            i2c->info->status.busy = 0; 
        
            XMC_I2C_CH_DisableEvent(i2c->i2c, XMC_USIC_CH_EVENT_STANDARD_RECEIVE | XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
        
            if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_TRANSFER_DONE);          
          }   
        }
      }
      else // Using FIFO
      {
        if ((XMC_USIC_CH_RXFIFO_GetEvent(i2c->i2c) & (XMC_USIC_CH_RXFIFO_EVENT_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE)) != 0)
        {
          XMC_USIC_CH_RXFIFO_ClearEvent(i2c->i2c, XMC_USIC_CH_RXFIFO_EVENT_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);

          while ((XMC_USIC_CH_RXFIFO_IsEmpty(i2c->i2c) == false))
          { 
            if (i2c->info->xfer.rx_cnt < i2c->info->xfer.rx_num)
            { 
              /* Read the data from FIFO buffer */
              i2c->info->xfer.rx_cnt++;
              *(i2c->info->xfer.rx_buf++) = XMC_I2C_CH_GetReceivedData(i2c->i2c);
            }
            else
            {       
              break;
            } 
          }
          
          if ((i2c->info->xfer.rx_num - i2c->info->xfer.rx_cnt) < i2c->rx_fifo_size_num)
          {
            XMC_USIC_CH_RXFIFO_SetSizeTriggerLimit(i2c->i2c, (XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg, i2c->info->xfer.rx_num - i2c->info->xfer.rx_cnt - 1U); 
          }

          if (i2c->info->xfer.tx_cnt < i2c->info->xfer.tx_num)
          {           
            /* Write to FIFO till Fifo is full */
            while (XMC_USIC_CH_TXFIFO_IsFull(i2c->i2c) == false)
            {         
              if (i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt == 1)
              {
                XMC_I2C_CH_MasterReceiveNack(i2c->i2c);
              }
              else
              {
                XMC_I2C_CH_MasterReceiveAck(i2c->i2c);
              } 
              i2c->info->xfer.tx_cnt++;   
            
              if ((i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt) == 0)
              {       
                break;
              }
            }
          }
          else
          { 
            if(i2c->info->pending == false)
            {
              XMC_I2C_CH_MasterStop(i2c->i2c); 
            } 
        
            XMC_USIC_CH_RXFIFO_DisableEvent(i2c->i2c,XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
        
            // Clear RX busy flag and set receive transfer complete event
            i2c->info->status.busy = 0;
        
            if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_TRANSFER_DONE);  
         } 
        }
      } // (XMC_I2C_CH_GetStatusFlag(i2c->i2c) & (XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) != 0

      if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) != 0)
      {
        XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
        if(i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt != 0)
        {
          if(i2c->tx_fifo_size_reg==NO_FIFO)
          {
            if(i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt == 1)
            {
              XMC_I2C_CH_MasterReceiveNack(i2c->i2c);
            }
            else
            {
              XMC_I2C_CH_MasterReceiveAck(i2c->i2c);
            } 
            i2c->info->xfer.tx_cnt++;         
          }
          else
          {
            /* Write to FIFO till Fifo is full */
            while((XMC_USIC_CH_TXFIFO_IsFull(i2c->i2c) == false))
            {         
              if(i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt == 1)
              {
                XMC_I2C_CH_MasterReceiveNack(i2c->i2c);
              }
              else
              {
                XMC_I2C_CH_MasterReceiveAck(i2c->i2c);
              } 
              i2c->info->xfer.tx_cnt++;   
      
              if(i2c->info->xfer.tx_num - i2c->info->xfer.tx_cnt == 0)
              {
                break;
              }
            }
          }     
        }
      }
    }
  }
  else // if SLAVE
  {
    if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED) != 0U)
    {
      i2c->info->status.busy=0;
    }
    
    if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_ERROR) != 0U)
    {
      /* I2C Bus error */
      i2c->info->status.busy=0;
      i2c->info->status.bus_error=1;      
    }
  
    if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_ARBITRATION_LOST) != 0U)
    {
      i2c->info->status.busy=0; 
      i2c->info->status.arbitration_lost=1;    
    }
  
    if(i2c->info->status.direction == I2C_DIR_READ) // receiver
    {
      if ((XMC_USIC_CH_RXFIFO_GetEvent(i2c->i2c) & (XMC_USIC_CH_RXFIFO_EVENT_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE)) != 0)
      {
        XMC_USIC_CH_RXFIFO_ClearEvent(i2c->i2c, XMC_USIC_CH_RXFIFO_EVENT_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_ALTERNATE);
      
        while((XMC_USIC_CH_RXFIFO_IsEmpty(i2c->i2c) == false))
        { 
          /* Read the data from FIFO buffer */
          i2c->info->xfer.rx_buf[i2c->info->xfer.rx_cnt++]= XMC_I2C_CH_GetReceivedData(i2c->i2c);
          if(i2c->info->xfer.rx_cnt ==  i2c->info->xfer.rx_num)
          {   
            XMC_USIC_CH_RXFIFO_DisableEvent(i2c->i2c,XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
            // Clear RX busy flag and set receive transfer complete event
            i2c->info->status.busy = 0;
            if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_TRANSFER_DONE);
            break;
          }
        }
      
        if((i2c->info->xfer.rx_num - i2c->info->xfer.rx_cnt) < i2c->rx_fifo_size_num)
        {
          XMC_USIC_CH_RXFIFO_Configure(i2c->i2c,i2c->info->rx_fifo_pointer,(XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg,(i2c->info->xfer.rx_num - i2c->info->xfer.rx_cnt)- 1U); 
        }
        else
        {       
          XMC_USIC_CH_RXFIFO_Configure(i2c->i2c,i2c->info->rx_fifo_pointer,(XMC_USIC_CH_FIFO_SIZE_t)i2c->rx_fifo_size_reg,i2c->rx_fifo_size_num - 1U); 
        }
      }
    }
    else // transmitter
    {
      if ((XMC_I2C_CH_GetStatusFlag(i2c->i2c) & XMC_I2C_CH_STATUS_FLAG_SLAVE_READ_REQUESTED) != 0U)
      {
        XMC_I2C_CH_ClearStatusFlag(i2c->i2c, XMC_I2C_CH_STATUS_FLAG_SLAVE_READ_REQUESTED);

        if(i2c->info->xfer.tx_num < i2c->info->xfer.tx_cnt) 
        { 
          /* Write to FIFO till Fifo is full */
          while((XMC_USIC_CH_TXFIFO_IsFull(i2c->i2c) == false))
          {   
            if(i2c->info->xfer.tx_num > i2c->info->xfer.tx_cnt) 
            { 
              XMC_I2C_CH_SlaveTransmit(i2c->i2c, i2c->info->xfer.tx_buf[i2c->info->xfer.tx_cnt]);
              i2c->info->xfer.tx_cnt++;
            }
            else
            {
              break;
            }
          }        
        }
        else  
        {     
          /* Disable standard transmit and error event interrupt */
          XMC_USIC_CH_TXFIFO_DisableEvent(i2c->i2c,XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD); 
          // Clear TX busy flag
          i2c->info->status.busy=0; 
          if (i2c->info->cb_event) i2c->info->cb_event(ARM_I2C_EVENT_TRANSFER_DONE);  
        }        
      }
    }
  }
}


#if (RTE_I2C0 != 0)
void I2C0_ISR(void) 
{
  I2C_IRQHandler(i2c[0]);
}
#endif

#if (RTE_I2C1 != 0)
void I2C1_ISR(void) 
{
  I2C_IRQHandler(i2c[1]);
}
#endif

#if (RTE_I2C2 != 0)
void I2C2_ISR(void) 
{
  I2C_IRQHandler(i2c[2]);
}
#endif

#if (RTE_I2C3 != 0)
void I2C3_ISR(void) 
{
  I2C_IRQHandler(i2c[3]);
}
#endif

#if (RTE_I2C4 != 0)
void I2C4_ISR(void) 
{
  I2C_IRQHandler(i2c[4]);
}
#endif

#if (RTE_I2C5 != 0)
void I2C5_ISR(void) 
{
  I2C_IRQHandler(i2c[5]);
}
#endif

#if (RTE_I2C0 != 0)
ARM_DRIVER_I2C Driver_I2C0 = 
{
  I2CX_GetVersion,
  I2C0_GetCapabilities,
  I2C0_Initialize,
  I2C0_Uninitialize,
  I2C0_PowerControl,
  I2C0_MasterTransmit,
  I2C0_MasterReceive, 
  I2C0_SlaveTransmit,
  I2C0_SlaveReceive,
  I2C0_GetDataCount,
  I2C0_Control,
  I2C0_GetStatus
};
#endif

#if (RTE_I2C1 != 0)
ARM_DRIVER_I2C Driver_I2C1 = 
{
  I2CX_GetVersion,
  I2C1_GetCapabilities,
  I2C1_Initialize,
  I2C1_Uninitialize,
  I2C1_PowerControl,
  I2C1_MasterTransmit,
  I2C1_MasterReceive, 
  I2C1_SlaveTransmit,
  I2C1_SlaveReceive,
  I2C1_GetDataCount,
  I2C1_Control,
  I2C1_GetStatus
};
#endif

#if (RTE_I2C2 != 0)
ARM_DRIVER_I2C Driver_I2C2 = 
{
  I2CX_GetVersion,
  I2C2_GetCapabilities,
  I2C2_Initialize,
  I2C2_Uninitialize,
  I2C2_PowerControl,
  I2C2_MasterTransmit,
  I2C2_MasterReceive, 
  I2C2_SlaveTransmit,
  I2C2_SlaveReceive,
  I2C2_GetDataCount,
  I2C2_Control,
  I2C2_GetStatus
};
#endif

#if (RTE_I2C3 != 0)
ARM_DRIVER_I2C Driver_I2C3 = 
{
  I2CX_GetVersion,
  I2C3_GetCapabilities,
  I2C3_Initialize,
  I2C3_Uninitialize,
  I2C3_PowerControl,
  I2C3_MasterTransmit,
  I2C3_MasterReceive, 
  I2C3_SlaveTransmit,
  I2C3_SlaveReceive,
  I2C3_GetDataCount,
  I2C3_Control,
  I2C3_GetStatus
};
#endif

#if (RTE_I2C4 != 0)
ARM_DRIVER_I2C Driver_I2C4 = 
{
  I2CX_GetVersion,
  I2C4_GetCapabilities,
  I2C4_Initialize,
  I2C4_Uninitialize,
  I2C4_PowerControl,
  I2C4_MasterTransmit,
  I2C4_MasterReceive, 
  I2C4_SlaveTransmit,
  I2C4_SlaveReceive,
  I2C4_GetDataCount,
  I2C4_Control,
  I2C4_GetStatus
};
#endif

#if (RTE_I2C5 != 0)
ARM_DRIVER_I2C Driver_I2C5 = 
{
  I2CX_GetVersion,
  I2C5_GetCapabilities,
  I2C5_Initialize,
  I2C5_Uninitialize,
  I2C5_PowerControl,
  I2C5_MasterTransmit,
  I2C5_MasterReceive, 
  I2C5_SlaveTransmit,
  I2C5_SlaveReceive,
  I2C5_GetDataCount,
  I2C5_Control,
  I2C5_GetStatus
};
#endif
