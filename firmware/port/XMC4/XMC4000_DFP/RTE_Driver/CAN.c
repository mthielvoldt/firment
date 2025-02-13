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
 * @file CAN.c
 * @date 30 July, 2016
 * @version 1.0
 *
 * @brief CAN Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version
 *
 */

#include "CAN.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_CAN_DRV_VERSION            ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)   /* driver version */
#define DEFAULT_ACCEPTANCE_MASK        0x1FFFFFFF

#if (UC_SERIES == XMC14) || (UC_DEVICE == XMC4108)
#define NUM_CAN_MESSAGE_OBJ            32U
#endif
#if (UC_DEVICE == XMC4100) || (UC_SERIES == XMC42) || (UC_SERIES == XMC43) || (UC_SERIES == XMC44) || (UC_SERIES == XMC45)
#define NUM_CAN_MESSAGE_OBJ            64U
#endif
#if (UC_SERIES == XMC47) || (UC_SERIES == XMC48)
#define NUM_CAN_MESSAGE_OBJ            255U
#endif

#if (defined(RTE_Drivers_CAN) && \
     (RTE_CAN0 == 0) && \
     (RTE_CAN1 == 0) && \
     (RTE_CAN2 == 0) && \
     (RTE_CAN3 == 0) && \
     (RTE_CAN4 == 0) && \
     (RTE_CAN5 == 0))
#error "CAN not configured in RTE_Device.h!"
#endif

/* Driver Version */    
static const ARM_DRIVER_VERSION can_driver_version = 
{
  ARM_CAN_API_VERSION,
  ARM_CAN_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_CAN_CAPABILITIES can_driver_capabilities = 
{
  NUM_CAN_MESSAGE_OBJ,  /* Number of CAN Message Objects available */
  1U,                   /* Supports reentrant calls to ARM_CAN_MessageSend, ARM_CAN_MessageRead, ARM_CAN_ObjectConfigure
                        and abort message sending used by ARM_CAN_Control. */
  0U,                   /* Does not support CAN with Flexible Data-rate mode (CAN_FD) */
  0U,                   /* Does not support restricted operation mode */
  1U,                   /* Supports bus monitoring mode */
  1U,                   /* Supports internal loop-back mode */
  0U,                   /* Does not support external loop-back mode */
};

/* Object Capabilities */
static const ARM_CAN_OBJ_CAPABILITIES can_object_capabilities = 
{
  1U,   /* Object supports transmission */
  1U,   /* Object supports reception */
  1U,   /* Object does support RTR reception and automatic Data transmission */
  1U,   /* Object does support RTR transmission and automatic Data reception */
  0U,   /* Object does not allows assignment of multiple filters to it */
  1U,   /* Object supports exact identifier filtering */
  0U,   /* Object does not support range identifier filtering */
  1U,   /* Object supports mask identifier filtering */
  1U    /* Object can buffer 1 message */
};

static bool global_can_initialized = false;

/* CAN0 */
#if (RTE_CAN0 != 0)

static CAN_INFO_t CAN0_INFO = {0};
static XMC_CAN_MO_t CAN0_MO;

CAN_RESOURCES_t CAN0_Resources = 
{
  RTE_CAN0_TX_PORT,
  RTE_CAN0_TX_AF,
  RTE_CAN0_RX_PORT,
  RTE_CAN0_RX_INPUT,
  (IRQn_Type)CAN0_0_IRQn,
  0,
  0,
  &CAN0_INFO,
  CAN,
  CAN_NODE0,
  &CAN0_MO
};
#endif

/* CAN1 */
#if (RTE_CAN1 != 0)

static CAN_INFO_t CAN1_INFO = {0};
static XMC_CAN_MO_t CAN1_MO;

CAN_RESOURCES_t CAN1_Resources = 
{
  RTE_CAN1_TX_PORT,
  RTE_CAN1_TX_AF,
  RTE_CAN1_RX_PORT,
  RTE_CAN1_RX_INPUT,
  (IRQn_Type)CAN0_1_IRQn,
  1,
  0,
  &CAN1_INFO,
  CAN,
  CAN_NODE1,
  &CAN1_MO
};
#endif

/* CAN2 */
#if (RTE_CAN2 != 0)

static CAN_INFO_t CAN2_INFO = {0};
static XMC_CAN_MO_t CAN2_MO;

CAN_RESOURCES_t CAN2_Resources = 
{
  RTE_CAN2_TX_PORT,
  RTE_CAN2_TX_AF,
  RTE_CAN2_RX_PORT,
  RTE_CAN2_RX_INPUT,
  (IRQn_Type)CAN0_2_IRQn,
  2,
  0,
  &CAN2_INFO,
  CAN,
  CAN_NODE2,
  &CAN2_MO
};
#endif

/* CAN3 */
#if (RTE_CAN3 != 0)

static CAN_INFO_t CAN3_INFO = {0};
static XMC_CAN_MO_t CAN3_MO;

CAN_RESOURCES_t CAN3_Resources = 
{
  RTE_CAN3_TX_PORT,
  RTE_CAN3_TX_AF,
  RTE_CAN3_RX_PORT,
  RTE_CAN3_RX_INPUT,
  (IRQn_Type)CAN0_3_IRQn,
  3,
  0,
  &CAN3_INFO,
  CAN,
  CAN_NODE3,
  &CAN3_MO
};
#endif

/* CAN4 */
#if (RTE_CAN4 != 0)

static CAN_INFO_t CAN4_INFO = {0};
static XMC_CAN_MO_t CAN4_MO;

CAN_RESOURCES_t CAN4_Resources = 
{
  RTE_CAN4_TX_PORT,
  RTE_CAN4_TX_AF,
  RTE_CAN4_RX_PORT,
  RTE_CAN4_RX_INPUT,
  (IRQn_Type)CAN0_4_IRQn,
  4,
  0,
  &CAN4_INFO,
  CAN,
  CAN_NODE4,
  &CAN4_MO
};
#endif

/* CAN5 */
#if (RTE_CAN5 != 0)

static CAN_INFO_t CAN5_INFO = {0};
static XMC_CAN_MO_t CAN5_MO;

CAN_RESOURCES_t CAN5_Resources = 
{
  RTE_CAN5_TX_PORT,
  RTE_CAN5_TX_AF,
  RTE_CAN5_RX_PORT,
  RTE_CAN5_RX_INPUT,
  (IRQn_Type)CAN0_5_IRQn,
  5,
  0,
  &CAN5_INFO,
  CAN,
  CAN_NODE5,
  &CAN5_MO
};
#endif

/* CAN Resources */
static CAN_RESOURCES_t *can[6] = 
{
#if (RTE_CAN0 != 0)
  &CAN0_Resources,
#else
  NULL,
#endif
#if (RTE_CAN1 != 0)
  &CAN1_Resources,
#else
  NULL,
#endif
#if (RTE_CAN2 != 0)
  &CAN2_Resources,
#else
  NULL,
#endif
#if (RTE_CAN3 != 0)
  &CAN3_Resources,
#else
  NULL,
#endif
#if (RTE_CAN4 != 0)
  &CAN4_Resources,
#else
  NULL,
#endif
#if (RTE_CAN5 != 0)
  &CAN5_Resources,
#else
  NULL,
#endif
};

/***********************************************************************************************************************
  \fn          ARM_DRV_VERSION CANX_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
***********************************************************************************************************************/
static ARM_DRIVER_VERSION CANX_GetVersion(void)
{
  return can_driver_version;
}

/***********************************************************************************************************************
  \fn          ARM_CAN_CAPABILITIES CAN_GetCapabilities (CAN_RESOURCES_t *can)
  \brief       Get driver capabilities.
  \param[in]   can    Pointer to CAN resources
  \return      \ref ARM_CAN_CAPABILITIES
***********************************************************************************************************************/
static ARM_CAN_CAPABILITIES CAN_GetCapabilities(CAN_RESOURCES_t *can)
{
  return can_driver_capabilities;
}

#if (RTE_CAN0 != 0)
static ARM_CAN_CAPABILITIES CAN0_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static ARM_CAN_CAPABILITIES CAN1_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static ARM_CAN_CAPABILITIES CAN2_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static ARM_CAN_CAPABILITIES CAN3_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static ARM_CAN_CAPABILITIES CAN4_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static ARM_CAN_CAPABILITIES CAN5_GetCapabilities(void)
{
  return CAN_GetCapabilities(can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          ARM_CAN_STATUS CAN_Initialize (ARM_CAN_SignalUnitEvent_t cb_unit_event,
                                              ARM_CAN_SignalObjectEvent_t cb_object_event,
                                              CAN_RESOURCES_t *can)
  \brief       Initialize CAN interface and register signal (callback) functions.
  \param[in]   cb_unit_event   Pointer to \ref ARM_CAN_SignalUnitEvent callback function
  \param[in]   cb_object_event Pointer to \ref ARM_CAN_SignalObjectEvent callback function
  \param[in]   can             Pointer to CAN resources
  \return      \ref ARM_CAN_STATUS
***********************************************************************************************************************/
static int32_t CAN_Initialize(ARM_CAN_SignalUnitEvent_t   cb_unit_event,
                              ARM_CAN_SignalObjectEvent_t cb_object_event,
                              CAN_RESOURCES_t *can) 
{
  if (((can->info->flags) & CAN_INITIALIZED) == 0)
  {
    can->info->cb_unit_event          = cb_unit_event;
    can->info->cb_object_event        = cb_object_event;
    can->info->status.unit_state      = 0;
    can->info->status.last_error_code = 0;
    can->info->status.tx_error_count  = 0;
    can->info->status.rx_error_count  = 0;
    can->can_mo->can_priority         = XMC_CAN_ARBITRATION_MODE_IDE_DIR_BASED_PRIO_2;
    
    can->info->flags |= CAN_INITIALIZED;
  }

  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event,
                               ARM_CAN_SignalObjectEvent_t cb_object_event)
{
  return CAN_Initialize(cb_unit_event, cb_object_event, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_Uninitialize (CAN_RESOURCES_t *can)
  \brief       De-initialize CAN Interface.
  \param[in]   can   Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static inline int32_t CAN_Uninitialize(CAN_RESOURCES_t *can)
{
  can->info->flags = 0;

  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_Uninitialize(void)
{
  return CAN_Uninitialize(can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_Uninitialize(void)
{
  return CAN_Uninitialize(can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_Uninitialize(void)
{
  return CAN_Uninitialize(can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_Uninitialize(void)
{
  return CAN_Uninitialize(can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_Uninitialize(void)
{
  return CAN_Uninitialize(can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_Uninitialize(void)
{
  return CAN_Uninitialize(can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_PowerControl (ARM_POWER_STATE state, CAN_RESOURCES_t *can)
  \brief       Control CAN interface power.
  \param[in]   state  Power state
                 - \ref ARM_POWER_OFF :  power off: no operation possible
                 - \ref ARM_POWER_LOW :  low power mode: retain state, detect and signal wake-up events
                 - \ref ARM_POWER_FULL : power on: full operation at maximum performance
  \param[in]   can    Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_PowerControl(ARM_POWER_STATE state, CAN_RESOURCES_t *can)
{
  switch (state)
  {
    case ARM_POWER_OFF:
      XMC_CAN_NODE_Disable(can->can_node);
      NVIC_DisableIRQ(can->irq_num);
      XMC_GPIO_SetMode(can->tx_port.port, can->tx_port.pin, XMC_GPIO_MODE_INPUT_TRISTATE);

      can->info->flags &= ~CAN_POWERED;
      break;

    case ARM_POWER_FULL:
      if ((can->info->flags & CAN_INITIALIZED) == 0U)
      {
        return ARM_DRIVER_ERROR;
      }
      
      if ((can->info->flags & CAN_POWERED) != 0U)
      {
        return ARM_DRIVER_OK;
      }

      if (!global_can_initialized)
      {
#if defined(MULTICAN_PLUS)
  #if UC_FAMILY == XMC4
        XMC_CAN_Init(can->can, XMC_CAN_CANCLKSRC_FPERI, XMC_SCU_CLOCK_GetPeripheralClockFrequency());
  #else
        XMC_CAN_Init(can->can, XMC_CAN_CANCLKSRC_MCLK, XMC_SCU_CLOCK_GetPeripheralClockFrequency());
  #endif
#else
        XMC_CAN_Init(can->can, XMC_SCU_CLOCK_GetPeripheralClockFrequency());
#endif

        global_can_initialized = true;
      }
      
      XMC_GPIO_CONFIG_t pin_config;
      XMC_STRUCT_INIT(pin_config);

      pin_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
      XMC_GPIO_Init(can->rx_port.port, can->rx_port.pin, &pin_config);

      pin_config.mode = (XMC_GPIO_MODE_t)(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | can->tx_alternate_function);
#if(UC_FAMILY == XMC4)
      pin_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;
#endif
      pin_config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;

      XMC_GPIO_Init(can->tx_port.port, can->tx_port.pin, &pin_config);
      
      XMC_CAN_NODE_EnableConfigurationChange(can->can_node); 
      XMC_CAN_NODE_SetReceiveInput(can->can_node, (XMC_CAN_NODE_RECEIVE_INPUT_t)can->rx_input);
      XMC_CAN_NODE_DisableConfigurationChange(can->can_node);
      
      NVIC_ClearPendingIRQ(can->irq_num);

#if(UC_FAMILY == XMC4)
      NVIC_SetPriority(can->irq_num, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U, 0U));
#else
      NVIC_SetPriority(can->irq_num, 3U);
#endif

#if (UC_SERIES == XMC14)
      XMC_SCU_SetInterruptControl(can->irq_num, (XMC_SCU_IRQCTRL_t)(((can->irq_num) << 8U) | 3U));
#endif

      NVIC_EnableIRQ(can->irq_num);

      can->info->flags |= CAN_POWERED;

      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_PowerControl(ARM_POWER_STATE state)
{
  return CAN_PowerControl(state, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          uint32_t CAN_GetClock (CAN_RESOURCES_t *can)
  \brief       Retrieve CAN base clock frequency.
  \param[in]   can    Pointer to CAN resources
  \return      base clock frequency
***********************************************************************************************************************/
static inline uint32_t CAN_GetClock(CAN_RESOURCES_t *can)
{
  (void)can;
  return (XMC_SCU_CLOCK_GetPeripheralClockFrequency());
}

#if (RTE_CAN0 != 0)
static uint32_t CAN0_GetClock(void)
{
  return CAN_GetClock(can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static uint32_t CAN1_GetClock(void)
{
  return CAN_GetClock(can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static uint32_t CAN2_GetClock(void)
{
  return CAN_GetClock(can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static uint32_t CAN3_GetClock(void)
{
  return CAN_GetClock(can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static uint32_t CAN4_GetClock(void)
{
  return CAN_GetClock(can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static uint32_t CAN5_GetClock(void)
{
  return CAN_GetClock(can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_SetBitrate (ARM_CAN_BITRATE_SELECT select,
                                       uint32_t bitrate,
                                       uint32_t bit_segments,
                                       CAN_RESOURCES_t *can)
  \brief       Set bitrate for CAN interface.
  \param[in]   select       Bitrate selection
                 - \ref ARM_CAN_BITRATE_NOMINAL : nominal (flexible data-rate arbitration) bitrate
                 - \ref ARM_CAN_BITRATE_FD_DATA : flexible data-rate data bitrate
  \param[in]   bitrate      Bitrate
  \param[in]   bit_segments Segment time quanta for signal sampling
  \param[in]   can          Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_SetBitrate (ARM_CAN_BITRATE_SELECT select,
                               uint32_t bitrate,
                               uint32_t bit_segments,
                               CAN_RESOURCES_t *can)
{
  uint32_t prop_seg, phase_seg1, phase_seg2, sjw, nominal_bit_time, brp;
  uint32_t div = 0U;  /* Default initialization value of DIV8 bitfield */

  if ((can->info->flags & CAN_POWERED) == 0U)
  { 
    return ARM_DRIVER_ERROR;
  }
  
  if (select != ARM_CAN_BITRATE_NOMINAL)
  { 
    return ARM_CAN_INVALID_BITRATE_SELECT; 
  }

  prop_seg   = (bit_segments & ARM_CAN_BIT_PROP_SEG_Msk  ) >> ARM_CAN_BIT_PROP_SEG_Pos;
  phase_seg1 = (bit_segments & ARM_CAN_BIT_PHASE_SEG1_Msk) >> ARM_CAN_BIT_PHASE_SEG1_Pos;
  phase_seg2 = (bit_segments & ARM_CAN_BIT_PHASE_SEG2_Msk) >> ARM_CAN_BIT_PHASE_SEG2_Pos;
  sjw        = (bit_segments & ARM_CAN_BIT_SJW_Msk       ) >> ARM_CAN_BIT_SJW_Pos;

  if (((prop_seg + phase_seg1) < 3U) || ((prop_seg + phase_seg1) > 16U))
  {
    return ARM_CAN_INVALID_BIT_PROP_SEG;
  }
  
  if (( phase_seg2             < 2U) || ( phase_seg2             >  8U))
  {
    return ARM_CAN_INVALID_BIT_PHASE_SEG2;
  }

  if (( sjw                    < 2U) || ( sjw                    >  4U))
  {
    return ARM_CAN_INVALID_BIT_SJW;
  }
  
  if (( bitrate           < 100000U) || (bitrate            > 1000000U))
  { 
    return ARM_CAN_INVALID_BITRATE;
  }

  nominal_bit_time = 1U + prop_seg + phase_seg1 + phase_seg2;  /* bit time = Sync + Tseq1 + Tseq2 */
  brp = XMC_SCU_CLOCK_GetPeripheralClockFrequency() / (nominal_bit_time * bitrate);
  if (brp > 512U)
  { 
    return ARM_CAN_INVALID_BITRATE;
  }
  
  if (brp > 64)
  {  
    brp = brp >> 3;
    div = 1U;
  }

  XMC_CAN_NODE_EnableConfigurationChange(can->can_node);
  
  can->can_node->NBTR = (((uint32_t)(phase_seg2-1U) << CAN_NODE_NBTR_TSEG2_Pos) & (uint32_t)CAN_NODE_NBTR_TSEG2_Msk) |
                        ((((uint32_t)(sjw-1U) << CAN_NODE_NBTR_SJW_Pos)) & (uint32_t)CAN_NODE_NBTR_SJW_Msk)|
                        (((uint32_t)((prop_seg + phase_seg1)-1U) << CAN_NODE_NBTR_TSEG1_Pos) & (uint32_t)CAN_NODE_NBTR_TSEG1_Msk)|
                        (((uint32_t)(brp-1U) << CAN_NODE_NBTR_BRP_Pos) & (uint32_t)CAN_NODE_NBTR_BRP_Msk)|
                        (((uint32_t)div << CAN_NODE_NBTR_DIV8_Pos) & (uint32_t)CAN_NODE_NBTR_DIV8_Msk);
  
  XMC_CAN_NODE_DisableConfigurationChange(can->can_node);

  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  return CAN_SetBitrate(select, bitrate, bit_segments, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_SetMode (ARM_CAN_MODE mode, CAN_RESOURCES_t *can)
  \brief       Set operating mode for CAN interface.
  \param[in]   mode   Operating mode
                 - \ref ARM_CAN_MODE_INITIALIZATION :    initialization mode
                 - \ref ARM_CAN_MODE_NORMAL :            normal operation mode
                 - \ref ARM_CAN_MODE_RESTRICTED :        restricted operation mode
                 - \ref ARM_CAN_MODE_MONITOR :           bus monitoring mode
                 - \ref ARM_CAN_MODE_LOOPBACK_INTERNAL : loopback internal mode
                 - \ref ARM_CAN_MODE_LOOPBACK_EXTERNAL : loopback external mode
  \param[in]   can    Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_SetMode(ARM_CAN_MODE mode, CAN_RESOURCES_t *can)
{

  if ((can->info->flags & CAN_POWERED) == 0U)
  {
    return ARM_DRIVER_ERROR;
  }

  switch (mode)
  {
    case ARM_CAN_MODE_INITIALIZATION:
      XMC_CAN_NODE_SetInitBit(can->can_node);                   /* terminate the participation of node in CAN traffic */

      can->info->status.unit_state = ARM_CAN_UNIT_STATE_INACTIVE;
      can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_BUS_OFF);     /* node is now initialized and disconnected from bus */
      break;

    case ARM_CAN_MODE_NORMAL:
      XMC_CAN_NODE_SetInitBit(can->can_node);                   /* terminate the participation of node in CAN traffic */
      XMC_CAN_NODE_EnableConfigurationChange(can->can_node);    /* enable configuration changes */
      XMC_CAN_NODE_ReSetAnalyzerMode(can->can_node);            /* disable analyzer mode */
      XMC_CAN_NODE_DisableLoopBack(can->can_node);              /* disable loop-back mode */
      XMC_CAN_NODE_DisableConfigurationChange(can->can_node);   /* disable configuration changes */
      XMC_CAN_NODE_ResetInitBit(can->can_node);                 /* enable the participation of node in CAN traffic */

      can->info->status.unit_state = ARM_CAN_UNIT_STATE_ACTIVE;
      can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_ACTIVE);      /* node is now connected to CAN bus and active */
      break;

    case ARM_CAN_MODE_RESTRICTED:
        return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_CAN_MODE_MONITOR:
      XMC_CAN_NODE_SetInitBit(can->can_node);                   /* terminate the participation of node in CAN traffic */
      XMC_CAN_NODE_EnableConfigurationChange(can->can_node);    /* enable configuration changes */
      XMC_CAN_NODE_DisableLoopBack(can->can_node);              /* disable loop-back mode */
      XMC_CAN_NODE_SetAnalyzerMode(can->can_node);              /* enable analyzer mode */
      XMC_CAN_NODE_DisableConfigurationChange(can->can_node);   /* disable configuration changes */
      XMC_CAN_NODE_ResetInitBit(can->can_node);                 /* enable the participation of node in CAN traffic */

      can->info->status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;
      can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_PASSIVE);     /* node is now in analyzer mode */
      break;

    case ARM_CAN_MODE_LOOPBACK_INTERNAL:
      XMC_CAN_NODE_SetInitBit(can->can_node);                   /* terminate the participation of node in CAN traffic */
      XMC_CAN_NODE_EnableConfigurationChange(can->can_node);    /* enable configuration changes */
      XMC_CAN_NODE_ReSetAnalyzerMode(can->can_node);            /* disable analyzer mode */
      XMC_CAN_NODE_EnableLoopBack(can->can_node);               /* enable loop-back mode */
      XMC_CAN_NODE_DisableConfigurationChange(can->can_node);   /* disable configuration changes */
      XMC_CAN_NODE_ResetInitBit(can->can_node);                 /* enable the participation of node in CAN traffic */

      can->info->status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;
      can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_PASSIVE);     /* node is now in loop-back mode */
      break;

    case ARM_CAN_MODE_LOOPBACK_EXTERNAL:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_SetMode(ARM_CAN_MODE mode)
{
  return CAN_SetMode(mode, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          ARM_CAN_OBJ_CAPABILITIES CAN_ObjectGetCapabilities (uint32_t obj_idx)
  \brief       Retrieve capabilities of an object.
  \param[in]   obj_idx  Object index
  \return      \ref ARM_CAN_OBJ_CAPABILITIES
***********************************************************************************************************************/
ARM_CAN_OBJ_CAPABILITIES CANX_ObjectGetCapabilities(uint32_t obj_idx)
{
  return can_object_capabilities;
}

/***********************************************************************************************************************
  \fn          int32_t CAN_ObjectSetFilter (uint32_t obj_idx,
                                            ARM_CAN_FILTER_OPERATION operation,
                                            uint32_t id,
                                            uint32_t arg,
                                            CAN_RESOURCES_t *can)
  \brief       Add or remove filter for message reception.
  \param[in]   obj_idx      Object index of object that filter should be or is assigned to
  \param[in]   operation    Operation on filter
                 - \ref ARM_CAN_FILTER_ID_EXACT_ADD :       add    exact id filter
                 - \ref ARM_CAN_FILTER_ID_EXACT_REMOVE :    remove exact id filter
                 - \ref ARM_CAN_FILTER_ID_RANGE_ADD :       add    range id filter
                 - \ref ARM_CAN_FILTER_ID_RANGE_REMOVE :    remove range id filter
                 - \ref ARM_CAN_FILTER_ID_MASKABLE_ADD :    add    maskable id filter
                 - \ref ARM_CAN_FILTER_ID_MASKABLE_REMOVE : remove maskable id filter
  \param[in]   id           ID or start of ID range (depending on filter type)
  \param[in]   arg          Mask or end of ID range (depending on filter type)
  \param[in]   can          Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_ObjectSetFilter(uint32_t obj_idx,
                                   ARM_CAN_FILTER_OPERATION operation,
                                   uint32_t id,
                                   uint32_t arg,
                                   CAN_RESOURCES_t *can)
{

  if (obj_idx >= NUM_CAN_MESSAGE_OBJ)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  if ((can->info->flags & CAN_POWERED) == 0U)
  {
    return ARM_DRIVER_ERROR;
  }

  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * obj_idx) + CAN_BASE + 0x1000U);

  if ((id & ARM_CAN_ID_IDE_Msk) != 0U)
  {
    XMC_CAN_MO_SetExtendedID(can->can_mo);
  }
  else
  {
    XMC_CAN_MO_SetStandardID(can->can_mo);
  }
  XMC_CAN_MO_AcceptOnlyMatchingIDE(can->can_mo);

  switch (operation)
  {
    case ARM_CAN_FILTER_ID_EXACT_ADD:
      XMC_CAN_MO_SetIdentifier(can->can_mo, id);
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, (uint32_t)DEFAULT_ACCEPTANCE_MASK);
      break;

    case ARM_CAN_FILTER_ID_MASKABLE_ADD:
      XMC_CAN_MO_SetIdentifier(can->can_mo, id);
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, arg);
      break;

    case ARM_CAN_FILTER_ID_RANGE_ADD:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_CAN_FILTER_ID_EXACT_REMOVE:
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, (uint32_t)0U);
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, (uint32_t)DEFAULT_ACCEPTANCE_MASK);
      break;

    case ARM_CAN_FILTER_ID_MASKABLE_REMOVE:
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, (uint32_t)0U);
      XMC_CAN_MO_SetAcceptanceMask(can->can_mo, (uint32_t)DEFAULT_ACCEPTANCE_MASK);
      break;

    case ARM_CAN_FILTER_ID_RANGE_REMOVE:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  return CAN_ObjectSetFilter(obj_idx, operation, id, arg, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg, CAN_RESOURCES_t *can)
  \brief       Configure object.
  \param[in]   obj_idx  Object index
  \param[in]   obj_cfg  Object configuration state
                 - \ref ARM_CAN_OBJ_INACTIVE :       deactivate object
                 - \ref ARM_CAN_OBJ_RX :             configure object for reception
                 - \ref ARM_CAN_OBJ_TX :             configure object for transmission
                 - \ref ARM_CAN_OBJ_RX_RTR_TX_DATA : configure object that on RTR reception automatically transmits Data Frame
                 - \ref ARM_CAN_OBJ_TX_RTR_RX_DATA : configure object that transmits RTR and automatically receives Data Frame
  \param[in]   can      Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg, CAN_RESOURCES_t *can)
{

  if (obj_idx >= NUM_CAN_MESSAGE_OBJ)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  if ((can->info->flags & CAN_POWERED) == 0U)
  {
    return ARM_DRIVER_ERROR;
  }

  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * obj_idx) + CAN_BASE + 0x1000U);
  
  XMC_CAN_AllocateMOtoNodeList(can->can, can->current_node_index, (uint8_t)obj_idx);
  XMC_CAN_NODE_EnableEvent(can->can_node, XMC_CAN_NODE_EVENT_ALERT);
  XMC_CAN_NODE_SetEventNodePointer(can->can_node, XMC_CAN_NODE_POINTER_EVENT_ALERT, can->current_node_index);
  can->mo_index = obj_idx;

  switch (obj_cfg)
  {
    case ARM_CAN_OBJ_INACTIVE:
      can->can_mo->can_mo_ptr->MOCTR = XMC_CAN_MO_RESET_STATUS_MESSAGE_VALID;
      break;
    
    case ARM_CAN_OBJ_TX:
      can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_TRANSMSGOBJ;
      can->info->RTR_response = 0U;
      XMC_CAN_MO_Config(can->can_mo);
      XMC_CAN_MO_EnableEvent(can->can_mo, XMC_CAN_MO_EVENT_TRANSMIT);
      XMC_CAN_MO_SetEventNodePointer(can->can_mo, XMC_CAN_MO_POINTER_EVENT_TRANSMIT, can->current_node_index); 
      break;

    case ARM_CAN_OBJ_RX:
      can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_RECMSGOBJ;
      can->info->RTR_response = 0U;
      XMC_CAN_MO_Config(can->can_mo);
      XMC_CAN_MO_EnableEvent(can->can_mo, XMC_CAN_MO_EVENT_RECEIVE);
      XMC_CAN_MO_SetEventNodePointer(can->can_mo, XMC_CAN_MO_POINTER_EVENT_RECEIVE, can->current_node_index); 
      break;

    case ARM_CAN_OBJ_RX_RTR_TX_DATA:
      can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_TRANSMSGOBJ;
      can->info->RTR_response = 1U;
      XMC_CAN_MO_Config(can->can_mo);
      XMC_CAN_MO_EnableEvent(can->can_mo, XMC_CAN_MO_EVENT_TRANSMIT);
      XMC_CAN_MO_SetEventNodePointer(can->can_mo, XMC_CAN_MO_POINTER_EVENT_TRANSMIT, can->current_node_index); 
      break;

    case ARM_CAN_OBJ_TX_RTR_RX_DATA:
      can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_RECMSGOBJ;
      can->info->RTR_response = 0U;
      XMC_CAN_MO_Config(can->can_mo);
      XMC_CAN_MO_EnableEvent(can->can_mo, XMC_CAN_MO_EVENT_RECEIVE );
      XMC_CAN_MO_SetEventNodePointer(can->can_mo, XMC_CAN_MO_POINTER_EVENT_RECEIVE, can->current_node_index); 
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{
  return CAN_ObjectConfigure(obj_idx, obj_cfg, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t CAN_MessageSend (uint32_t obj_idx,
                                        ARM_CAN_MSG_INFO *msg_info,
                                        const uint8_t *data,
                                        uint8_t size,
                                        CAN_RESOURCES_t *can)
  \brief       Send message on CAN bus.
  \param[in]   obj_idx  Object index
  \param[in]   msg_info Pointer to CAN message information
  \param[in]   data     Pointer to data buffer
  \param[in]   size     Number of data bytes to send
  \param[in]   can      Pointer to CAN resources
  \return      value >= 0  number of data bytes accepted to send
  \return      value < 0   \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_MessageSend(uint32_t obj_idx,
                               ARM_CAN_MSG_INFO *msg_info,
                               const uint8_t *data,
                               uint8_t size,
                               CAN_RESOURCES_t *can)
{
                  
  XMC_CAN_STATUS_t status = XMC_CAN_STATUS_SUCCESS;

  if (obj_idx >= NUM_CAN_MESSAGE_OBJ)
  { 
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  if ((can->info->flags & CAN_POWERED) == 0U)
  {
    return ARM_DRIVER_ERROR;
  }

  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * obj_idx) + CAN_BASE + 0x1000U);

  XMC_CAN_NODE_ClearStatus(can->can_node, XMC_CAN_NODE_STATUS_LAST_ERROR_CODE);

  can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_TRANSMSGOBJ;

  if ((msg_info->id & ARM_CAN_ID_IDE_Msk) != 0U)
  {
    XMC_CAN_MO_SetExtendedID(can->can_mo);
  }
  else
  {
    XMC_CAN_MO_SetStandardID(can->can_mo);
  }
  XMC_CAN_MO_SetIdentifier(can->can_mo, msg_info->id);

  if (size > 8U)
  { 
    size = 8U;
  }

  if (msg_info->rtr != 0U)
  {
    size = 0U;
    can->can_mo->can_data_length = msg_info->dlc;
    can->can_mo->can_data[0]     = 0U;
    can->can_mo->can_data[1]     = 0U;
    XMC_CAN_MO_SetStatus(can->can_mo, XMC_CAN_MO_RESET_STATUS_MESSAGE_DIRECTION);
  }
  else
  {
    can->can_mo->can_data_length = size;
    can->can_mo->can_data[0]     = *((__packed uint32_t *)(data  ));
    can->can_mo->can_data[1]     = *((__packed uint32_t *)(data+4));
  }

  XMC_CAN_MO_UpdateData(can->can_mo);
  if (!can->info->RTR_response)
  {
    status = XMC_CAN_MO_Transmit(can->can_mo);
  }

  if (status == XMC_CAN_STATUS_BUSY)
  {
    return ARM_DRIVER_ERROR_BUSY;
  }
  else
  {
    return ((int32_t)size);
  }
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  return CAN_MessageSend(obj_idx, msg_info, data, size, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t ARM_CAN_MessageRead (uint32_t obj_idx,
                                            ARM_CAN_MSG_INFO *msg_info,
                                            uint8_t *data,
                                            uint8_t size,
                                            CAN_RESOURCES_t *can)
  \brief       Read message received on CAN bus.
  \param[in]   obj_idx  Object index
  \param[in]   can      Pointer to CAN resources
  \param[out]  msg_info Pointer to read CAN message information
  \param[out]  data     Pointer to data buffer for read data
  \param[in]   size     Maximum number of data bytes to read
  \return      value >= 0  number of data bytes read
  \return      value < 0   \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_MessageRead(uint32_t obj_idx,
                               ARM_CAN_MSG_INFO *msg_info,
                               uint8_t *data,
                               uint8_t size,
                               CAN_RESOURCES_t *can)
{

  if (obj_idx >= NUM_CAN_MESSAGE_OBJ)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  if ((can->info->flags & CAN_POWERED) == 0U)
  {
    return ARM_DRIVER_ERROR;
  }

  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * obj_idx) + CAN_BASE + 0x1000U);
  if ((can->can_mo->can_mo_ptr->MOSTAT & XMC_CAN_MO_STATUS_MESSAGE_DIRECTION) != 0)
  {
    return ARM_DRIVER_ERROR;
  }

  can->can_mo->can_mo_type = XMC_CAN_MO_TYPE_RECMSGOBJ;

  if (size > 8U)
  { 
    size = 8U;
  }

  XMC_CAN_MO_Receive(can->can_mo);

  if (can->can_mo->can_id_mode == XMC_CAN_FRAME_TYPE_EXTENDED_29BITS)
  {
    msg_info->id = (0x1FFFFFFFUL & can->can_mo->can_identifier) | ARM_CAN_ID_IDE_Msk;
  }
  else
  {
    msg_info->id = (    0x07FFUL & can->can_mo->can_identifier);
  }

  if (size > 0U)
  {
    memcpy(data, (uint8_t *)(&can->can_mo->can_data[0]), size);
  }

  return ((int32_t)size);
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  return CAN_MessageRead(obj_idx, msg_info, data, size, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          int32_t ARM_CAN_Control (uint32_t control, uint32_t arg, CAN_RESOURCES_t *can)
  \brief       Control CAN interface.
  \param[in]   control  Operation
                 - \ref ARM_CAN_SET_FD_MODE :            set FD operation mode
                 - \ref ARM_CAN_ABORT_MESSAGE_SEND :     abort sending of CAN message
                 - \ref ARM_CAN_CONTROL_RETRANSMISSION : enable/disable automatic retransmission
                 - \ref ARM_CAN_SET_TRANSCEIVER_DELAY :  set transceiver delay
  \param[in]   arg      Argument of operation
  \param[in]   can      Pointer to CAN resources
  \return      \ref execution_status
***********************************************************************************************************************/
static int32_t CAN_Control(uint32_t control, uint32_t arg, CAN_RESOURCES_t *can)
{

  if ((can->info->flags & CAN_POWERED) == 0U)
  { 
    return ARM_DRIVER_ERROR; 
  }
  
  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * arg) + CAN_BASE + 0x1000U);

  switch (control & ARM_CAN_CONTROL_Msk)
  {
    case ARM_CAN_ABORT_MESSAGE_SEND:
      if (arg >= NUM_CAN_MESSAGE_OBJ)
      { 
        return ARM_DRIVER_ERROR_PARAMETER;
      }

      XMC_CAN_MO_ResetStatus(can->can_mo, XMC_CAN_MO_RESET_STATUS_TX_PENDING);
      break;

    case ARM_CAN_CONTROL_RETRANSMISSION:
      if (arg == 0)
      {
        XMC_CAN_MO_EnableSingleTransmitTrial(can->can_mo);
      }
      break;

    case ARM_CAN_SET_FD_MODE:
    case ARM_CAN_SET_TRANSCEIVER_DELAY:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

  }
  return ARM_DRIVER_OK;
}

#if (RTE_CAN0 != 0)
static int32_t CAN0_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static int32_t CAN1_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static int32_t CAN2_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static int32_t CAN3_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static int32_t CAN4_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static int32_t CAN5_Control(uint32_t control, uint32_t arg)
{
  return CAN_Control(control, arg, can[5]);
}
#endif

/***********************************************************************************************************************
  \fn          ARM_CAN_STATUS CAN_GetStatus (CAN_RESOURCES *can)
  \brief       Get CAN status.
  \param[in]   can     Pointer to CAN resources
  \return      CAN status \ref ARM_CAN_STATUS
***********************************************************************************************************************/
static ARM_CAN_STATUS CAN_GetStatus(CAN_RESOURCES_t *can)
{
  ARM_CAN_STATUS status;

  if (((can->can_node->NCR & XMC_CAN_NODE_CONTROL_NODE_INIT) != 0U) ||
      ((can->can_node->NSR & XMC_CAN_NODE_STATUS_BUS_OFF) != 0))
  {
    can->info->status.unit_state = ARM_CAN_UNIT_STATE_INACTIVE;
  }
  else if (((can->can_node->NPCR & CAN_NODE_NPCR_LBM_Msk) != 0U) ||
           ((can->can_node->NCR & XMC_CAN_NODE_CONTROL_CAN_ANALYZER_NODEDE) != 0U) ||
           ((can->can_node->NSR & XMC_CAN_NODE_STATUS_ERROR_WARNING_STATUS) != 0U))
  {
    can->info->status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;
  }
  else
  {
    can->info->status.unit_state = ARM_CAN_UNIT_STATE_ACTIVE;
  }

  switch (can->can_node->NSR & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
  {
    case 0:
      can->info->status.last_error_code = ARM_CAN_LEC_NO_ERROR;
      break;
    case 1:
      can->info->status.last_error_code = ARM_CAN_LEC_STUFF_ERROR;
      break;
    case 2:
      can->info->status.last_error_code = ARM_CAN_LEC_FORM_ERROR;
      break;
    case 3:
      can->info->status.last_error_code = ARM_CAN_LEC_ACK_ERROR;
      break;
    case 4:
    case 5:
      can->info->status.last_error_code = ARM_CAN_LEC_BIT_ERROR;
      break;
    case 6:
      can->info->status.last_error_code = ARM_CAN_LEC_CRC_ERROR;
      break;
    case 7:
      can->info->status.last_error_code = ARM_CAN_LEC_NO_ERROR;
      break;
  }

  can->info->status.tx_error_count = XMC_CAN_NODE_GetTransmitErrorCounter(can->can_node);
  can->info->status.rx_error_count = XMC_CAN_NODE_GetReceiveErrorCounter(can->can_node);

  status.unit_state       = can->info->status.unit_state;
  status.last_error_code  = can->info->status.last_error_code;
  status.tx_error_count   = can->info->status.tx_error_count;
  status.rx_error_count   = can->info->status.rx_error_count;

  return status;
}

#if (RTE_CAN0 != 0)
static ARM_CAN_STATUS CAN0_GetStatus(void)
{
  return CAN_GetStatus(can[0]);
}
#endif

#if (RTE_CAN1 != 0)
static ARM_CAN_STATUS CAN1_GetStatus(void)
{
  return CAN_GetStatus(can[1]);
}
#endif

#if (RTE_CAN2 != 0)
static ARM_CAN_STATUS CAN2_GetStatus(void)
{
  return CAN_GetStatus(can[2]);
}
#endif

#if (RTE_CAN3 != 0)
static ARM_CAN_STATUS CAN3_GetStatus(void)
{
  return CAN_GetStatus(can[3]);
}
#endif

#if (RTE_CAN4 != 0)
static ARM_CAN_STATUS CAN4_GetStatus(void)
{
  return CAN_GetStatus(can[4]);
}
#endif

#if (RTE_CAN5 != 0)
static ARM_CAN_STATUS CAN5_GetStatus(void)
{
  return CAN_GetStatus(can[5]);
}
#endif


/***********************************************************************************************************************
  \fn          void CAN_IRQHandler (CAN_RESOURCES  *can)
  \brief       CAN Interrupt handler.
  \param[in]   can  Pointer to CAN resources
***********************************************************************************************************************/
static void CAN_IRQHandler(CAN_RESOURCES_t *can)
{

  uint32_t node_status;
  uint32_t object_status;

  can->can_mo->can_mo_ptr = (CAN_MO_TypeDef *)((0x0020U * can->mo_index) + CAN_BASE + 0x1000U);

  node_status = XMC_CAN_NODE_GetStatus(can->can_node);
  object_status = XMC_CAN_MO_GetStatus(can->can_mo);

  if ((node_status & XMC_CAN_NODE_STATUS_BUS_OFF) != 0)
  {
    XMC_CAN_NODE_ClearStatus(can->can_node, XMC_CAN_NODE_STATUS_ALERT_WARNING);
    can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_BUS_OFF);
  }

  if ((node_status & XMC_CAN_NODE_STATUS_ERROR_WARNING_STATUS) != 0)
  {
    XMC_CAN_NODE_ClearStatus(can->can_node, XMC_CAN_NODE_STATUS_ALERT_WARNING);
    can->info->cb_unit_event(ARM_CAN_EVENT_UNIT_WARNING);
  }

  if ((object_status & XMC_CAN_MO_STATUS_RX_PENDING ) != 0)
  {
    XMC_CAN_MO_ResetStatus(can->can_mo, XMC_CAN_MO_RESET_STATUS_RX_PENDING);
    can->info->cb_object_event(can->mo_index, ARM_CAN_EVENT_RECEIVE);
  }

  if ((object_status & XMC_CAN_MO_STATUS_TX_PENDING ) != 0)
  {
    XMC_CAN_MO_ResetStatus(can->can_mo, XMC_CAN_MO_RESET_STATUS_TX_PENDING);
    can->info->cb_object_event(can->mo_index, ARM_CAN_EVENT_SEND_COMPLETE);
  }
}

#if (RTE_CAN0 != 0)
void CAN0_ISR(void)
{
  CAN_IRQHandler(can[0]);
}
#endif

#if (RTE_CAN1 != 0)
void CAN1_ISR(void)
{
  CAN_IRQHandler(can[1]);
}
#endif

#if (RTE_CAN2 != 0)
void CAN2_ISR(void)
{
  CAN_IRQHandler(can[2]);
}
#endif

#if (RTE_CAN3 != 0)
void CAN3_ISR(void)
{
  CAN_IRQHandler(can[3]);
}
#endif

#if (RTE_CAN4 != 0)
void CAN4_ISR(void)
{
  CAN_IRQHandler(can[4]);
}
#endif

#if (RTE_CAN5 != 0)
void CAN5_ISR(void)
{
  CAN_IRQHandler(can[5]);
}
#endif

#if (RTE_CAN0 != 0)
ARM_DRIVER_CAN Driver_CAN0 = 
{
  CANX_GetVersion,
  CAN0_GetCapabilities,
  CAN0_Initialize,
  CAN0_Uninitialize,
  CAN0_PowerControl,
  CAN0_GetClock,
  CAN0_SetBitrate,
  CAN0_SetMode,
  CANX_ObjectGetCapabilities,
  CAN0_ObjectSetFilter,
  CAN0_ObjectConfigure,
  CAN0_MessageSend,
  CAN0_MessageRead,
  CAN0_Control,
  CAN0_GetStatus
};
#endif

#if (RTE_CAN1 != 0)
ARM_DRIVER_CAN Driver_CAN1 = 
{
  CANX_GetVersion,
  CAN1_GetCapabilities,
  CAN1_Initialize,
  CAN1_Uninitialize,
  CAN1_PowerControl,
  CAN1_GetClock,
  CAN1_SetBitrate,
  CAN1_SetMode,
  CANX_ObjectGetCapabilities,
  CAN1_ObjectSetFilter,
  CAN1_ObjectConfigure,
  CAN1_MessageSend,
  CAN1_MessageRead,
  CAN1_Control,
  CAN1_GetStatus
};
#endif

#if (RTE_CAN2 != 0)
ARM_DRIVER_CAN Driver_CAN2 = 
{
  CANX_GetVersion,
  CAN2_GetCapabilities,
  CAN2_Initialize,
  CAN2_Uninitialize,
  CAN2_PowerControl,
  CAN2_GetClock,
  CAN2_SetBitrate,
  CAN2_SetMode,
  CANX_ObjectGetCapabilities,
  CAN2_ObjectSetFilter,
  CAN2_ObjectConfigure,
  CAN2_MessageSend,
  CAN2_MessageRead,
  CAN2_Control,
  CAN2_GetStatus
};
#endif

#if (RTE_CAN3 != 0)
ARM_DRIVER_CAN Driver_CAN3 = 
{
  CANX_GetVersion,
  CAN3_GetCapabilities,
  CAN3_Initialize,
  CAN3_Uninitialize,
  CAN3_PowerControl,
  CAN3_GetClock,
  CAN3_SetBitrate,
  CAN3_SetMode,
  CANX_ObjectGetCapabilities,
  CAN3_ObjectSetFilter,
  CAN3_ObjectConfigure,
  CAN3_MessageSend,
  CAN3_MessageRead,
  CAN3_Control,
  CAN3_GetStatus
};
#endif

#if (RTE_CAN4 != 0)
ARM_DRIVER_CAN Driver_CAN4 = 
{
  CANX_GetVersion,
  CAN4_GetCapabilities,
  CAN4_Initialize,
  CAN4_Uninitialize,
  CAN4_PowerControl,
  CAN4_GetClock,
  CAN4_SetBitrate,
  CAN4_SetMode,
  CANX_ObjectGetCapabilities,
  CAN4_ObjectSetFilter,
  CAN4_ObjectConfigure,
  CAN4_MessageSend,
  CAN4_MessageRead,
  CAN4_Control,
  CAN4_GetStatus
};
#endif

#if (RTE_CAN5 != 0)
ARM_DRIVER_CAN Driver_CAN5 = 
{
  CANX_GetVersion,
  CAN5_GetCapabilities,
  CAN5_Initialize,
  CAN5_Uninitialize,
  CAN5_PowerControl,
  CAN5_GetClock,
  CAN5_SetBitrate,
  CAN5_SetMode,
  CANX_ObjectGetCapabilities,
  CAN5_ObjectSetFilter,
  CAN5_ObjectConfigure,
  CAN5_MessageSend,
  CAN5_MessageRead,
  CAN5_Control,
  CAN5_GetStatus
};
#endif
