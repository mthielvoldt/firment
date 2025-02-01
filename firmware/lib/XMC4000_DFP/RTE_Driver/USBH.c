/*
 * Copyright (c) 2016, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of condit
 ions and the following 
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
 * (To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *
 */

/**
 * @file USBH.c
 * @date 02 March, 2016
 * @version 1.0
 *
 * @brief USBH Driver for Infineon XMC4000
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"
#include "USBH.h"
#include "Driver_USBH.h"

/* USBH Driver ***************************************************************** */

#define ARM_USBH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)

/* Driver Version */
static const ARM_DRIVER_VERSION xmc_usbh_driver_version = { ARM_USBH_API_VERSION, ARM_USBH_DRV_VERSION };

/* Driver Capabilities */
static const ARM_USBH_CAPABILITIES xmc_usbh_driver_capabilities = {
  0x0001U,      /* Root HUB available Ports Mask */
  0U,           /* Automatic SPLIT packet handling */
  1U,           /* Signal Connect event */
  1U,           /* Signal Disconnect event */
  0U            /* Signal Overcurrent event */
};

/* Driver state and registers */
static XMC_USBH0_DEVICE_t XMC_USBH0_device __attribute__((section ("RW_IRAM1"))) = { 
   (USB0_GLOBAL_TypeDef *)(USB0_BASE),    /** Global register interface            */
   ((USB0_CH_TypeDef *)(USB0_CH0_BASE)),  /** Host channel interface               */
   0,                                     /** Port event callback; set during init */
   0,                                     /** Pipe event callback; set during init */
   false,                                 /** init status */
   ARM_POWER_OFF,                         /** USB Power status */
   false                                  /** Port reset state */
};
bool is_nack[USBH0_MAX_PIPE_NUM];

#define RX_FIFO_SIZE           (1024U)   /* RxFIFO size */
#define TX_FIFO_SIZE_NON_PERI  (64U)     /* Non-periodic Tx FIFO size */
#define TX_FIFO_SIZE_PERI      (960U)   /* Periodic Tx FIFO size */

static uint32_t *XMC_USBH0_dfifo_ptr[USBH0_MAX_PIPE_NUM];

/* Local functions */
/**
  \fn          bool XMC_lClockGating (uint8_t enable)
  \brief       Enable/disable clock gating depending if feature is supported.
  \param[in]   enable Enable (XMC_USBH0_CLOCK_GATING_ENABLE) or disable(XMC_USBH0_CLOCK_GATING_DISABLE) clock gating
  \return      none
*/
__INLINE static void XMC_lClockGating(uint8_t enable)
{
#if defined(CLOCK_GATING_SUPPORTED)
if (enable == XMC_USBH0_CLOCK_GATING_ENABLE)
    XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USB0);
   if (enable == XMC_USBH0_CLOCK_GATING_DISABLE)
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USB0);
#endif
  return;
}

/**
  \fn          bool XMC_lTriggerHaltChannel (USB0_CH_TypeDef *ptr_ch)
  \brief       Triggers halt of a channel.
  \param[in]   ptr_ch Pointer to Channel
  \return      none
*/
__INLINE static void XMC_lTriggerHaltChannel(USB0_CH_TypeDef *ptr_ch)
{
  ptr_ch->HCINTMSK = USB_CH_HCINT_ChHltd_Msk; /* Enable halt interrupt */
  ptr_ch->HCCHAR  |= USB_CH_HCCHAR_ChEna_Msk | USB_CH_HCCHAR_ChDis_Msk;
  return;
}

/**
  \fn          bool XMC_lStartTransfer (PIPE_t *ptr_pipe, USB0_CH_TypeDef *ptr_ch)
  \brief       Start transfer on Pipe.
  \param[in]   ptr_pipe Pointer to Pipe
  \param[in]   ptr_ch   Pointer to Channel
  \return      true = success, false = fail
*/
static bool XMC_lStartTransfer (XMC_USBH0_pipe_t *ptr_pipe, USB0_CH_TypeDef *ptr_ch) {
  uint32_t  hcchar;
  uint32_t  hctsiz;
  uint32_t  hcintmsk;
  uint32_t  num_remaining_transfer;
  uint32_t  num_remaining_fifo;
  uint32_t  num_remaining_queue;
  uint32_t  txsts;
  uint32_t  pckt_num;
  uint32_t  max_pckt_size;
  uint8_t   *ptr_src;
  uint32_t  *ptr_dest;
  uint16_t  cnt;

  if (!(XMC_USBH0_device.global_register->HPRT & USB_HPRT_PrtConnSts_Msk)){return false;}

  /* Save channel characteristic register to local variable */
  hcchar   = ptr_ch->HCCHAR;
  /* Save transfer size register to local variable */
  hctsiz   = ptr_ch->HCTSIZ_BUFFERMODE; 
  hcintmsk = 0U;
  cnt      = 0U;

  /* Prepare transfer */
  /* Reset EPDir (transfer direction = output) and enable channel */
  hcchar &= ~(USB_CH_HCCHAR_EPDir_Msk | USB_CH_HCCHAR_ChDis_Msk);
  hcchar |= USB_CH_HCCHAR_ChEna_Msk;
  
  /* Enable default interrupts needed for all transfers */
  hcintmsk  = (USB_CH_HCINTMSK_XactErrMsk_Msk  |
               USB_CH_HCINTMSK_XferComplMsk_Msk |
               USB_CH_HCINTMSK_NakMsk_Msk    |
               USB_CH_HCINTMSK_StallMsk_Msk)  ;
  /* Keep PID */
  hctsiz &=  USB_CH_HCTSIZ_BUFFERMODE_Pid_Msk;
  
  /* Packet specific setup */
  switch (ptr_pipe->packet & ARM_USBH_PACKET_TOKEN_Msk) {
    case ARM_USBH_PACKET_IN:
      /* set transfer direction to input */
      hcchar   |=  USB_CH_HCCHAR_EPDir_Msk; 
      /* Enable IN transfer specific interrupts */
      hcintmsk  |= ( USB_CH_HCINTMSK_DataTglErrMsk_Msk  |
                     USB_CH_HCINTMSK_BblErrMsk_Msk |
                     USB_CH_HCINTMSK_AckMsk_Msk    |
                     USB_CH_HCINTMSK_NakMsk_Msk )  ;
      break;
    case ARM_USBH_PACKET_OUT:
      break;
    case ARM_USBH_PACKET_SETUP:
      hctsiz   &= ~USB_CH_HCTSIZ_BUFFERMODE_Pid_Msk  ;
      hctsiz   |=  USB_CH_HCTSIZx_DPID_SETUP;
      break;
    default:
      break;
  }
  /* Prepare PID */
  switch (ptr_pipe->packet & ARM_USBH_PACKET_DATA_Msk) {
    case ARM_USBH_PACKET_DATA0:
      hctsiz   &= ~USB_CH_HCTSIZ_BUFFERMODE_Pid_Msk;
      hctsiz   |=  USB_CH_HCTSIZx_DPID_DATA0;
      break;
    case ARM_USBH_PACKET_DATA1:
      hctsiz   &= ~USB_CH_HCTSIZ_BUFFERMODE_Pid_Msk;
      hctsiz   |=  USB_CH_HCTSIZx_DPID_DATA1;
      break;
    default:
      break;
  }
  
  /* Prepare odd/even frame */
  if ((XMC_USBH0_device.global_register->HFNUM & 1U) != 0U) {
    hcchar &= ~USB_CH_HCCHAR_OddFrm_Msk;
  } else {
    hcchar |=  USB_CH_HCCHAR_OddFrm_Msk;
  }

  /* Get transfer type specific status */
  switch (ptr_pipe->ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      if (!(hcchar & USB_CH_HCCHAR_EPDir_Msk)) {
        txsts = XMC_USBH0_device.global_register->GNPTXSTS;
      }
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      if (!(hcchar & USB_CH_HCCHAR_EPDir_Msk)) {
        txsts = XMC_USBH0_device.global_register->HPTXSTS;
      }
      break;
  }

  /* Calculate remaining transfer size */
  num_remaining_transfer = ptr_pipe->num - ptr_pipe->num_transferred_total;
  /* Limit transfer to available space inside fifo/queue if OUT transaction */
  if (!(hcchar & USB_CH_HCCHAR_EPDir_Msk) != 0U) {
    max_pckt_size =  ptr_pipe->ep_max_packet_size;
    num_remaining_fifo = (txsts & 0x0000FFFFU) <<  2;
    num_remaining_queue  = (txsts & 0x00FF0000U) >> 16;
    if (num_remaining_transfer > num_remaining_fifo) {
      num_remaining_transfer = num_remaining_fifo;
    }
    pckt_num = (num_remaining_transfer + max_pckt_size - 1) / max_pckt_size;
    if (pckt_num > num_remaining_queue) {
      pckt_num = num_remaining_queue;
    }
    if (num_remaining_transfer > (pckt_num * max_pckt_size)) {
      num_remaining_transfer = pckt_num * max_pckt_size;
    }
    cnt = (num_remaining_transfer + 3U) / 4U;
    ptr_src  = ptr_pipe->data + ptr_pipe->num_transferred_total;
    ptr_dest = (uint32_t *)XMC_USBH0_dfifo_ptr[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))];
    /* For OUT/SETUP transfer num_transferring represents num of bytes to be sent */
    ptr_pipe->num_transferring = num_remaining_transfer;
  }
  else {
    /* For IN transfer num_transferring is zero */
    ptr_pipe->num_transferring = 0U;
  }
  /* Set packet count and transfer size */
  if (num_remaining_transfer != 0U) {
    hctsiz |= ((num_remaining_transfer + ptr_pipe->ep_max_packet_size - 1U) / ptr_pipe->ep_max_packet_size) << 19;
    hctsiz |=   num_remaining_transfer;                
  } else {                                                 /* Zero length packet */
    hctsiz |=   1U << USB_CH_HCTSIZ_BUFFERMODE_PktCnt_Pos; /* Packet count = 1 */
    hctsiz |=   0U;                                        /* Transfer size = 0 */
  }
  NVIC_DisableIRQ (USB0_0_IRQn);
  ptr_ch->HCINTMSK = hcintmsk;                  /* Enable channel interrupts */
  ptr_ch->HCTSIZ_BUFFERMODE = hctsiz;           /* Write ch transfer size */
  ptr_ch->HCCHAR = hcchar;                      /* Write ch characteristics */
  while (cnt != 0U) {                           /* Load data */
    *ptr_dest = *((__packed uint32_t *)ptr_src);
    ptr_src  += 4U;
    cnt--;
  }
  NVIC_EnableIRQ  (USB0_0_IRQn);                /* Enable OTG interrupt */
  return true;
}

/* USB driver API functions */
/**
  \fn          ARM_DRIVER_VERSION XMC_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION XMC_GetVersion (void) { return xmc_usbh_driver_version; }

/**
  \fn          ARM_USBH_CAPABILITIES XMC_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBH_CAPABILITIES
*/
static ARM_USBH_CAPABILITIES XMC_GetCapabilities (void) { return xmc_usbh_driver_capabilities; }

/**
  \fn          int32_t XMC_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event,
                                        ARM_USBH_SignalPipeEvent_t cb_pipe_event)
  \brief       Initialize USB Host Interface.
  \param[in]   cb_port_event  Pointer to \ref ARM_USBH_SignalPortEvent
  \param[in]   cb_pipe_event  Pointer to \ref ARM_USBH_SignalPipeEvent
  \return      \ref execution_status
*/
static int32_t XMC_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event,
                                ARM_USBH_SignalPipeEvent_t cb_pipe_event) {

if (XMC_USBH0_device.init_done == true) { return ARM_DRIVER_OK; }
  /* assign callbacks */
  XMC_USBH0_device.SignalPortEvent_cb = cb_port_event;
  XMC_USBH0_device.SignalPipeEvent_cb = cb_pipe_event;
  
  /* assign fifo start addresses */
  for (uint8_t channel = 0; channel < USBH0_MAX_PIPE_NUM; channel++) {
    XMC_USBH0_dfifo_ptr[channel] = (uint32_t *)(USB0_BASE + (channel+1) * 0x01000U);
  }

  XMC_GPIO_SetMode(XMC_USB_DRIVE_PORT, XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1);

  XMC_USBH0_device.init_done = true;
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_Uninitialize (void)
  \brief       De-initialize USB Host Interface.
  \return      \ref execution_status
*/
static int32_t XMC_Uninitialize (void) {
  XMC_USBH0_device.init_done = false;
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Host Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t XMC_PowerControl (ARM_POWER_STATE state) {
  switch (state) {
    case ARM_POWER_LOW:  
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_POWER_OFF:
      NVIC_DisableIRQ  (USB0_0_IRQn);
      NVIC_ClearPendingIRQ (USB0_0_IRQn); /* Clear pending interrupt */
      XMC_USBH0_device.power_state =  state; /* Clear powered flag */
      XMC_USBH0_device.global_register->GAHBCFG  &= ~USB_GAHBCFG_GlblIntrMsk_Msk; /* Disable USB interrupts */
      XMC_lClockGating (XMC_USBH0_CLOCK_GATING_ENABLE); /* Enable Clock Gating */
      XMC_USBH0_device.global_register->PCGCCTL  |=  USB_PCGCCTL_StopPclk_Msk; /* Stop PHY clock */
      XMC_SCU_POWER_DisableUsb(); /* Disable Power USB */
      XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0); /* reset USB */
      XMC_USBH0_device.port_reset_active =  false; /* Reset variables */
      memset((void *)(pipe), 0, sizeof(pipe));
      break;
    case ARM_POWER_FULL:
      if (XMC_USBH0_device.init_done == false) { return ARM_DRIVER_ERROR; } /* not initialized */
      if (XMC_USBH0_device.power_state == ARM_POWER_FULL) { return ARM_DRIVER_OK; } /* already powered */
      XMC_lClockGating (XMC_USBH0_CLOCK_GATING_DISABLE); /* disable clock gating */
      osDelay(2U);
      XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0); /* deassert reset USB */
      osDelay(2U);
      osDelay(100U);
      XMC_SCU_POWER_EnableUsb(); /* Enable Power USB */

      /* On-chip Full-speed PHY */
      XMC_USBH0_device.global_register->PCGCCTL  &=  ~USB_PCGCCTL_StopPclk_Msk;  /* Start PHY clock */
      XMC_USBH0_device.global_register->GUSBCFG  |=  USB_GUSBCFG_PHYSel_Msk;    /* Full-speed transceiver */

      while ((XMC_USBH0_device.global_register->GRSTCTL & USB_GRSTCTL_AHBIdle_Msk) == 0U); /* wait until AHB master state machine is idle */

      XMC_USBH0_device.global_register->GRSTCTL |=  USB_GRSTCTL_CSftRst_Msk; /* Core soft reset */
      while ((XMC_USBH0_device.global_register->GRSTCTL & USB_GRSTCTL_CSftRst_Msk)  != 0U); /* wait soft reset confirmation */
      osDelay(100U);

      XMC_USBH0_device.port_reset_active =  false; /* Reset variables */
      memset((void *)(pipe), 0, sizeof(pipe));

      if (((XMC_USBH0_device.global_register->GUSBCFG & USB_GUSBCFG_ForceHstMode_Msk) == 0U) || ((XMC_USBH0_device.global_register->GUSBCFG & USB_GUSBCFG_ForceDevMode_Msk) != 0U)) {
        XMC_USBH0_device.global_register->GUSBCFG &= ~USB_GUSBCFG_ForceDevMode_Msk;          /* Clear force device mode */
        XMC_USBH0_device.global_register->GUSBCFG |=  USB_GUSBCFG_ForceHstMode_Msk;          /* Force host mode */
        osDelay(100U);
      }

      /* FS only, even if HS is supported */
      XMC_USBH0_device.global_register->HCFG     |=  USB_CH_HCFG_FSLSSUP(1);

      /* Rx FIFO setting */
      XMC_USBH0_device.global_register->GRXFSIZ   = (RX_FIFO_SIZE/4U);
      /* Non-periodic Tx FIFO setting */
      XMC_USBH0_device.global_register->GNPTXFSIZ_HOSTMODE = (((TX_FIFO_SIZE_NON_PERI/4U) << 16) |  (RX_FIFO_SIZE / 4U));
      /* Periodic Tx FIFO setting */
      XMC_USBH0_device.global_register->HPTXFSIZ  = ((TX_FIFO_SIZE_PERI / 4U) << 16) | ((RX_FIFO_SIZE + TX_FIFO_SIZE_NON_PERI) / 4U);
      /* Enable channel interrupts */
      XMC_USBH0_device.global_register->HAINTMSK  = (1U << USBH0_MAX_PIPE_NUM) - 1U;
      /* Unmask interrupts */
      XMC_USBH0_device.global_register->GINTMSK_HOSTMODE   = (
                        USB_GINTSTS_HOSTMODE_DisconnInt_Msk |
                        USB_GINTMSK_HOSTMODE_HChIntMsk_Msk    |
                        USB_GINTMSK_HOSTMODE_PrtIntMsk_Msk   |
                        USB_GINTMSK_HOSTMODE_RxFLvlMsk_Msk |
                        USB_GINTMSK_HOSTMODE_SofMsk_Msk 
                       )   ;
      /* Set powered state */
      XMC_USBH0_device.power_state = state;
      /* Enable interrupts */
      XMC_USBH0_device.global_register->GAHBCFG  |=  USB_GAHBCFG_GlblIntrMsk_Msk;
      /* Set highest interrupt priority */
      NVIC_SetPriority (USB0_0_IRQn, 0);
      NVIC_EnableIRQ   (USB0_0_IRQn);
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PortVbusOnOff (uint8_t port, bool vbus)
  \brief       Root HUB Port VBUS on/off.
  \param[in]   port  Root HUB Port Number
  \param[in]   vbus
                - \b false VBUS off
                - \b true  VBUS on
  \return      \ref execution_status
*/
static int32_t XMC_PortVbusOnOff (uint8_t port, bool vbus) {
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR; }
  if (port != 0U) { return ARM_DRIVER_ERROR_PARAMETER; }
  if (vbus != 0U) {
    /* Port power on */
    XMC_USBH0_device.global_register->HPRT |=  USB_HPRT_PrtPwr_Msk;
    XMC_GPIO_SetMode(XMC_USB_DRIVE_PORT, XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1);
  } else {                                              
    /* Port power off */
    XMC_USBH0_device.global_register->HPRT &= ~USB_HPRT_PrtPwr_Msk;
    XMC_GPIO_SetMode(XMC_USB_DRIVE_PORT, XMC_GPIO_MODE_INPUT_TRISTATE);
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PortReset (uint8_t port)
  \brief       Do Root HUB Port Reset.
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t XMC_PortReset (uint8_t port) {
  uint32_t hprt;

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR; }
  if (port != 0U) { return ARM_DRIVER_ERROR_PARAMETER; }

  XMC_USBH0_device.port_reset_active = true;
  hprt  =  XMC_USBH0_device.global_register->HPRT;
  hprt &= ~USB_HPRT_PrtEna_Msk;                            /* Disable port */
  hprt |=  USB_HPRT_PrtRst_Msk;                            /* Port reset */
  XMC_USBH0_device.global_register->HPRT = hprt;
  osDelay(50U);                                            /* wait at least 50ms */
  hprt &= ~USB_HPRT_PrtRst_Msk;                            /* Clear port reset */
  XMC_USBH0_device.global_register->HPRT = hprt;
  osDelay(50U);                                            /* wait for ISR */
  if (XMC_USBH0_device.port_reset_active == true) {
    XMC_USBH0_device.port_reset_active = false;
    return ARM_DRIVER_ERROR;                               /* reset not confirmed inside ISR */
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PortSuspend (uint8_t port)
  \brief       Suspend Root HUB Port (stop generating SOFs).
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t XMC_PortSuspend (uint8_t port) {

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }

  XMC_USBH0_device.global_register->HPRT |= USB_HPRT_PrtSusp_Msk;
  
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PortResume (uint8_t port)
  \brief       Resume Root HUB Port (start generating SOFs).
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t XMC_PortResume (uint8_t port) {
  
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }
  
  XMC_USBH0_device.global_register->HPRT |= USB_HPRT_PrtRes_Msk;
  
  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USBH_PORT_STATE XMC_PortGetState (uint8_t port)
  \brief       Get current Root HUB Port State.
  \param[in]   port  Root HUB Port Number
  \return      Port State \ref ARM_USBH_PORT_STATE
*/
static ARM_USBH_PORT_STATE XMC_PortGetState (uint8_t port) {
  ARM_USBH_PORT_STATE port_state = { 0U, 0U, 0U };
  uint32_t hprt;
  
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return port_state;}
  if (port != 0U)          { return port_state;}

  hprt = XMC_USBH0_device.global_register->HPRT;
  port_state.connected   = (hprt & USB_HPRT_PrtConnSts_Msk) != 0U;
  port_state.overcurrent = 0U;

  switch ((hprt & USB_HPRT_PrtSpd_Msk) >> USB_HPRT_PrtSpd_Pos) {
    case 1: /* Full speed */
     port_state.speed = ARM_USB_SPEED_FULL;
     break;
    default:
     break;
  }
  
  return port_state;
}

/**
  \fn          ARM_USBH_PIPE_HANDLE XMC_PipeCreate (uint8_t  dev_addr,
                                                    uint8_t  dev_speed,
                                                    uint8_t  hub_addr,
                                                    uint8_t  hub_port,
                                                    uint8_t  ep_addr,
                                                    uint8_t  ep_type,
                                                    uint16_t ep_max_packet_size,
                                                    uint8_t  ep_interval)
  \brief       Create Pipe in System.
  \param[in]   dev_addr   Device Address
  \param[in]   dev_speed  Device Speed
  \param[in]   hub_addr   Hub Address
  \param[in]   hub_port   Hub Port
  \param[in]   ep_addr    Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type    Endpoint Type (ARM_USB_ENDPOINT_xxx)
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \param[in]   ep_interval        Endpoint Polling Interval
  \return      Pipe Handle \ref ARM_USBH_PIPE_HANDLE
*/
static ARM_USBH_PIPE_HANDLE XMC_PipeCreate (uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t  ep_interval) {
  XMC_USBH0_pipe_t    *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;
  uint32_t         i;
  
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return (ARM_USBH_PIPE_HANDLE)NULL; }
  
 /* get first free pipe available */
  ptr_ch = (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers);

  for (i = 0U; i < USBH0_MAX_PIPE_NUM; i++) {
    if ((ptr_ch->HCCHAR & 0x3FFFFFFFU) == 0U) 
    { 
      break;
    }
    ptr_ch++;
  }

  /* free pipe found? */
  if (i == USBH0_MAX_PIPE_NUM) { return (ARM_USBH_PIPE_HANDLE)NULL; } 
  ptr_pipe = (XMC_USBH0_pipe_t *)(&pipe[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);

  memset((void *)ptr_pipe, 0, sizeof(XMC_USBH0_pipe_t));  /* Initialize pipe structure */
  
  /* Fill in all fields of Endpoint Descriptor */
  ptr_ch->HCCHAR = (USB_CH_HCCHARx_MPS (ep_max_packet_size) ) |
                   (USB_CH_HCCHARx_EPNUM (ep_addr) ) |
                   (USB_CH_HCCHAR_EPDir_Msk * (((ep_addr >> 7) & 0x0001U) == 0U)) |
                   (USB_CH_HCCHARx_EPTYPE (ep_type) ) |
                   (USB_CH_HCCHARx_DEVADDR (dev_addr) ) ;
  /* Store Pipe settings */
  ptr_pipe->ep_max_packet_size = ep_max_packet_size;
  ptr_pipe->ep_type            = ep_type;
  switch (ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      if (ep_interval > 0U) {
        ptr_pipe->interval_reload = ep_interval;
      }
      ptr_pipe->interval = ptr_pipe->interval_reload;
      ptr_ch->HCCHAR |= USB_CH_HCCHARx_MCEC((((ep_max_packet_size >> 11) + 1U) & 3U));
      break;
  }
  
  return ((ARM_USBH_EP_HANDLE)ptr_ch);
}

/**
  \fn          int32_t XMC_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl,
                                        uint8_t              dev_addr,
                                        uint8_t              dev_speed,
                                        uint8_t              hub_addr,
                                        uint8_t              hub_port,
                                        uint16_t             ep_max_packet_size)
  \brief       Modify Pipe in System.
  \param[in]   pipe_hndl  Pipe Handle
  \param[in]   dev_addr   Device Address
  \param[in]   dev_speed  Device Speed
  \param[in]   hub_addr   Hub Address
  \param[in]   hub_port   Hub Port
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \return      \ref execution_status
*/
static int32_t XMC_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint16_t ep_max_packet_size) {
  XMC_USBH0_pipe_t    *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;
  uint32_t   hcchar;

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR; }
  if (pipe_hndl  == 0U) { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (USB0_CH_TypeDef *)(pipe_hndl);
 ptr_pipe = (XMC_USBH0_pipe_t *)(&pipe[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);
  if (ptr_pipe->in_use != 0U) { return ARM_DRIVER_ERROR_BUSY;      }

  /* Fill in all fields of channel */
  hcchar  =   ptr_ch->HCCHAR;
  /* Clear fields */
  hcchar &= ~(USB_CH_HCCHAR_MPS_Msk |
              USB_CH_HCCHAR_DevAddr_Msk)  ;
  /* Set fields */
  hcchar |=   USB_CH_HCCHARx_MPS   (ep_max_packet_size)              |
              (USB_CH_HCCHARx_DEVADDR     (dev_addr))                       ;
  ptr_ch->HCCHAR = hcchar;                           

  ptr_pipe->ep_max_packet_size = ep_max_packet_size;
  
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Delete Pipe from System.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t XMC_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  XMC_USBH0_pipe_t    *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF)    { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)       { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (USB0_CH_TypeDef *)(pipe_hndl);
 ptr_pipe = (XMC_USBH0_pipe_t    *)(&pipe[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);
  if (ptr_pipe->in_use != 0U) { return ARM_DRIVER_ERROR_BUSY; }

  ptr_ch->HCCHAR            = 0U;
  ptr_ch->HCINT             = 0U;
  ptr_ch->HCINTMSK          = 0U;
  ptr_ch->HCTSIZ_BUFFERMODE = 0U;

  memset((void *)ptr_pipe, 0, sizeof(XMC_USBH0_pipe_t));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Reset Pipe.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t XMC_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  XMC_USBH0_pipe_t    *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF)    { return ARM_DRIVER_ERROR; }
  if (pipe_hndl  == 0U)       { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (USB0_CH_TypeDef *)(pipe_hndl);
 ptr_pipe = (XMC_USBH0_pipe_t    *)(&pipe[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);
  if (ptr_pipe->in_use != 0U) { return ARM_DRIVER_ERROR_BUSY; }

  ptr_ch->HCINT    = 0U;
  ptr_ch->HCINTMSK = 0U;
  ptr_ch->HCTSIZ_BUFFERMODE   = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl,
                                          uint32_t             packet,
                                          uint8_t             *data,
                                          uint32_t             num)
  \brief       Transfer packets through USB Pipe.
  \param[in]   pipe_hndl  Pipe Handle
  \param[in]   packet     Packet information
  \param[in]   data       Pointer to buffer with data to send or for data to receive
  \param[in]   num        Number of data bytes to transfer
  \return      \ref execution_status
*/
static int32_t XMC_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet, uint8_t *data, uint32_t num) {
  XMC_USBH0_pipe_t *ptr_pipe;

  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR; }
  
    if (( (packet& ARM_USBH_PACKET_TOKEN_Msk)!=ARM_USBH_PACKET_OUT ) &&
      ( (packet& ARM_USBH_PACKET_TOKEN_Msk)!=ARM_USBH_PACKET_IN ) &&
      ( (packet& ARM_USBH_PACKET_TOKEN_Msk)!=ARM_USBH_PACKET_SETUP )) 
  { 
    return ARM_DRIVER_ERROR_PARAMETER;; 
  }
  if (pipe_hndl  == 0U) { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((XMC_USBH0_device.global_register->HPRT & USB_HPRT_PrtConnSts_Msk) == 0U) { return ARM_DRIVER_ERROR; }

 ptr_pipe = (XMC_USBH0_pipe_t *)(&pipe[((USB0_CH_TypeDef *)pipe_hndl - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);
  if (ptr_pipe->in_use != 0U) { return ARM_DRIVER_ERROR_BUSY; }

  /* Prepare transfer information */
  ptr_pipe->packet                = packet;
  ptr_pipe->data                  = data;
  ptr_pipe->num                   = num;
  ptr_pipe->num_transferred_total = 0U;
  ptr_pipe->num_transferring      = 0U;
  ptr_pipe->in_use                = 0U;
  ptr_pipe->transfer_active       = 0U;
  ptr_pipe->interrupt_triggered   = 0U;
  ptr_pipe->event                 = 0U;
  
  if ((ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) && (ptr_pipe->interval != 0U)) {
    ptr_pipe->in_use              = 1U; /* transfer will be started inside interrupt (SOF) */
  } else {
    ptr_pipe->transfer_active     = 1U;
    ptr_pipe->in_use              = 1U;
    XMC_lStartTransfer (ptr_pipe, (USB0_CH_TypeDef *)pipe_hndl);
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t XMC_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Get result of USB Pipe transfer.
  \param[in]   pipe_hndl  Pipe Handle
  \return      number of successfully transferred data bytes
*/
static uint32_t XMC_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  if (pipe_hndl == 0U) { return 0U; }
  return (pipe[((USB0_CH_TypeDef *)pipe_hndl - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))].num_transferred_total);
}

/**
  \fn          int32_t XMC_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Abort current USB Pipe transfer.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t XMC_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  XMC_USBH0_pipe_t *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;
  uint32_t timeout;
  
  ptr_ch = (USB0_CH_TypeDef *) pipe_hndl;
  
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U) { return ARM_DRIVER_ERROR_PARAMETER; }

 ptr_pipe = (XMC_USBH0_pipe_t *)(&pipe[(ptr_ch - (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers))]);

  if (ptr_pipe->in_use != 0U) {
    ptr_pipe->in_use = 0U;
    /* Disable channel if not yet halted */
    if ((ptr_ch->HCINT & USB_CH_HCINT_ChHltd_Msk) == 0 ) {
      if (ptr_ch->HCCHAR & USB_CH_HCCHAR_ChEna_Msk) {
        ptr_ch->HCINTMSK = 0U;
        osDelay(1U);
        if (ptr_ch->HCINT & USB_CH_HCINT_NAK_Msk) {
          ptr_ch->HCINT  =  USB_CH_HCINTx_ALL;    /* Clear all interrupts */
          return true;
        }
        ptr_ch->HCINT  =  USB_CH_HCINTx_ALL;      /* Clear all interrupts */
        ptr_ch->HCCHAR =  ptr_ch->HCCHAR | USB_CH_HCCHAR_ChEna_Msk | USB_CH_HCCHAR_ChDis_Msk;
    
        /* wait until channel is halted */
        for (timeout = 0U; timeout < 5000U; timeout++) {
          if (ptr_ch->HCINT & USB_CH_HCINT_ChHltd_Msk) {
            ptr_ch->HCINT = USB_CH_HCINTx_ALL;
            return ARM_DRIVER_OK;
          }
        }
        return ARM_DRIVER_ERROR;
      }
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint16_t XMC_GetFrameNumber (void)
  \brief       Get current USB Frame Number.
  \return      Frame Number
*/
static uint16_t XMC_GetFrameNumber (void) {
  
  if (XMC_USBH0_device.power_state == ARM_POWER_OFF) { return 0U; }
  
  return ((XMC_USBH0_device.global_register->HFNUM) & 0xFFFU);
}

/**
  \fn          void XMC_HandleIrq (uint32_t gintsts)
  \brief       USB Host Interrupt Routine (IRQ).
*/
void XMC_HandleIrq (uint32_t gintsts) {
  XMC_USBH0_pipe_t    *ptr_pipe;
  USB0_CH_TypeDef *ptr_ch;
  uint32_t   hprt, haint, hcint, pktcnt, mpsiz;
  uint32_t   ch;
  uint8_t   *ptr_data;
  uint32_t  *dfifo;
  uint32_t   grxsts, bcnt, dat, len, len_rest;
  
  /* Host port interrupt */
  if ((gintsts & USB_GINTSTS_HOSTMODE_PrtInt_Msk) != 0U) {
    hprt = XMC_USBH0_device.global_register->HPRT;
    /* Clear port enable */
    XMC_USBH0_device.global_register->HPRT = hprt & (~USB_HPRT_PrtEna_Msk);
    if ((hprt & USB_HPRT_PrtConnDet_Msk) != 0U) {
      XMC_USBH0_device.global_register->HCFG =  USB_CH_HCFG_FSLSPCS(1) |
                                                    USB_CH_HCFG_FSLSSUP(1);
      /* Ignore connect under reset */
      if (XMC_USBH0_device.port_reset_active == false) {
        XMC_USBH0_device.SignalPortEvent_cb(0, ARM_USBH_EVENT_CONNECT);
      }
    }
    if ((hprt & USB_HPRT_PrtEnChng_Msk) != 0U) { /* If port enable changed */
      if ((hprt & USB_HPRT_PrtEna_Msk) != 0U) {  /* If device connected */
        if (XMC_USBH0_device.port_reset_active == true) {
          XMC_USBH0_device.port_reset_active = false;
          XMC_USBH0_device.SignalPortEvent_cb(0, ARM_USBH_EVENT_RESET);
        }
      }
    }
  }
  
  /* Disconnect interrupt */
  if ((gintsts & USB_GINTSTS_HOSTMODE_DisconnInt_Msk) != 0U) {
    XMC_USBH0_device.global_register->GINTSTS_HOSTMODE = USB_GINTSTS_HOSTMODE_DisconnInt_Msk; /* Clear disconnect interrupt */
    /* Ignore disconnect under reset */
    if ( XMC_USBH0_device.port_reset_active == false) {
      ptr_ch   = (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers);
      ptr_pipe = (XMC_USBH0_pipe_t    *)(pipe);
      for (ch = 0U; ch < USBH0_MAX_PIPE_NUM; ch++) {
        if (ptr_pipe->in_use != 0U) {
          ptr_pipe->in_use = 0U;
          ptr_ch->HCINT    = USB_CH_HCINTx_ALL;                                            /* Clear all interrupts */
          ptr_ch->HCINTMSK = USB_CH_HCINT_ChHltd_Msk;                           /* Enable halt interrupt */
          ptr_ch->HCCHAR  |= USB_CH_HCCHAR_ChEna_Msk | USB_CH_HCCHAR_ChDis_Msk; /* Activate Halt */
          XMC_USBH0_device.SignalPipeEvent_cb((ARM_USBH_EP_HANDLE)ptr_ch, ARM_USBH_EVENT_BUS_ERROR);
        }
        ptr_ch++;
        ptr_pipe++;
      }
      XMC_USBH0_device.SignalPortEvent_cb(0, ARM_USBH_EVENT_DISCONNECT);
    }
  }
  /* Handle receive fifo not-empty interrupt */
  if ((gintsts & USB_GINTSTS_HOSTMODE_RxFLvl_Msk) != 0U) {
    XMC_USBH0_device.global_register->GINTMSK_HOSTMODE &= ~USB_GINTMSK_HOSTMODE_RxFLvlMsk_Msk;
    grxsts     = (XMC_USBH0_device.global_register->GRXSTSP_HOSTMODE);    
    /* IN Data Packet received ? */
    if (((grxsts >> 17) & 0x0FU) == USB_GRXSTSR_HOSTMODE_PktSts_IN_DATA_PKT) {
      ch         = (grxsts     ) & USB_GRXSTSR_DEVICEMODE_EPNum_Msk;
      bcnt       = ((grxsts & USB_GRXSTSR_DEVICEMODE_BCnt_Msk) >> USB_GRXSTSR_DEVICEMODE_BCnt_Pos);
      dfifo      = (uint32_t *)XMC_USBH0_dfifo_ptr[ch];
      ptr_data   =  pipe[ch].data + pipe[ch].num_transferred_total;
      len        =  bcnt / 4U; /* Received number of 32-bit data */
      len_rest   =  bcnt & 3U; /* Number of bytes left */
      /* Read data from fifo */
      /* Read 32 bit sized  data */
      while (len != 0U) {
        *((__packed uint32_t *)ptr_data) = *dfifo;
        ptr_data += 4U;
        len--;
      }
      /* Read 8 bit sized data */
      if (len_rest != 0U) {
        dat = *((__packed uint32_t *)dfifo);
        while (len_rest != 0U) {
          *ptr_data++ = dat;
          dat >>= 8;
          len_rest--;
        }
      }
      pipe[ch].num_transferring      += bcnt;
      pipe[ch].num_transferred_total += bcnt;
    }
    XMC_USBH0_device.global_register->GINTMSK_HOSTMODE |= USB_GINTMSK_HOSTMODE_RxFLvlMsk_Msk;
  }

  /* Handle sof interrupt */
  if ((gintsts & USB_GINTSTS_HOSTMODE_Sof_Msk) != 0U) { /* If start of frame interrupt */
    XMC_USBH0_device.global_register->GINTSTS_HOSTMODE =  USB_GINTSTS_HOSTMODE_Sof_Msk; /* Clear SOF interrupt */
    ptr_pipe = (XMC_USBH0_pipe_t *)(pipe);
    for (ch = 0U; ch < USBH0_MAX_PIPE_NUM; ch++) {
      /* If interrupt transfer is active handle period (interval) */
      if ((ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) && (ptr_pipe->in_use == 1U)) {
        if (ptr_pipe->interval != 0U)
        {
          ptr_pipe->interval--;
          if (ptr_pipe->interval==0)
          {
            ptr_pipe->interval = ptr_pipe->interval_reload;
            ptr_pipe->interrupt_triggered = 1U;
          }
        }
      }
      ptr_pipe++;
    }
  }
  
  /* Handle host ctrl interrupt */
  if ((gintsts & USB_GINTSTS_HOSTMODE_HChInt_Msk) != 0U) {
    haint = XMC_USBH0_device.global_register->HAINT;
    for (ch = 0U; ch < USBH0_MAX_PIPE_NUM; ch++) {
      /* Check for interrupt of all channels */
      if ((haint & (1U << ch)) != 0U) {
        haint     &= ~(1U << ch);
        ptr_ch     =  (USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers) + ch;
        ptr_pipe   =  (XMC_USBH0_pipe_t    *)(&pipe[ch]);
        hcint      =   ptr_ch->HCINT & ptr_ch->HCINTMSK;
        if ((hcint & USB_CH_HCINT_ChHltd_Msk) != 0U) {                                  /* channel halted ? */
          ptr_ch->HCINTMSK = 0U;                                                        /*  disable all channel interrupts */
          ptr_ch->HCINT    = USB_CH_HCINTx_ALL;                                                    /*  clear all interrupts */
          ptr_pipe->transfer_active = 0U;                                               /*  set status transfer not active */
          hcint = 0U;
        }
        if ((hcint & USB_CH_HCINT_XferCompl_Msk) != 0U) {                               /* data transfer finished ? */
          ptr_ch->HCINT   = USB_CH_HCINTx_ALL;                                                     /*  clear all interrupts */
          if ((ptr_ch->HCCHAR & USB_CH_HCCHAR_EPDir_Msk) == 0U) {                       /*  endpoint OUT ? */
            ptr_ch->HCINTMSK = 0U;                                                      /*   disable all channel interrupts */
            ptr_pipe->transfer_active = 0U;                                             /*   transfer not in progress */
            ptr_pipe->num_transferred_total += ptr_pipe->num_transferring;              /*   admin OUT transfer status */
            ptr_pipe->num_transferring       = 0U;                                      /*   admin OUT transfer status */
            if (ptr_pipe->num_transferred_total == ptr_pipe->num) {                     /*   all bytes transferred ? */
              ptr_pipe->in_use = 0U;                                                    /*    release pipe */
              ptr_pipe->event = ARM_USBH_EVENT_TRANSFER_COMPLETE;                       /*    prepare event notification */
            }                                                                           
            hcint = 0U;                                                                 
          }                                                                             
          if ((ptr_ch->HCCHAR & USB_CH_HCCHAR_EPDir_Msk) != 0U) {                       /*  endpoint IN ? */
            ptr_pipe->in_use = 0U;                                                      /*   release pipe */
            ptr_pipe->event = ARM_USBH_EVENT_TRANSFER_COMPLETE;                         /*   prepare event notification */
            XMC_lTriggerHaltChannel(ptr_ch);                                            /*   trigger channel halt */
          }
        }
        if ((hcint & USB_CH_HCINTMSK_AckMsk_Msk) != 0U) {                                /* ACK received ? */
          ptr_ch->HCINT = USB_CH_HCINTMSK_AckMsk_Msk;                                    /*  clear ACK interrupt */
          if ((ptr_ch->HCCHAR & USB_CH_HCCHAR_EPDir_Msk) != 0U) {                        /*  endpoint IN ? */
            if ((ptr_pipe->num != ptr_pipe->num_transferred_total) &&                    /*   if all data was not transferred */
                (ptr_pipe->num_transferring != 0U)                 &&                    /*   if zero-length packet was not received */
               ((ptr_pipe->num_transferred_total%ptr_pipe->ep_max_packet_size) == 0U)){  /*   if short packet was not received */
               ptr_ch->HCCHAR |= USB_CH_HCCHAR_ChEna_Msk;                                /*    trigger next transfer */
            }
          } else {                                                                        /* endpoint OUT */
            XMC_lTriggerHaltChannel(ptr_ch);                                              /*  trigger channel halt */
          } 
          hcint = 0U;
        }
        if (((hcint & (USB_CH_HCINTMSK_StallMsk_Msk |                                    /* STALL */
                      USB_CH_HCINTMSK_NakMsk_Msk   |                                     /* or NAK */
                      USB_CH_HCINTx_ERRORS )) != 0U) &&                                  /* or transaction error */
            ((ptr_ch->HCCHAR & USB_CH_HCCHAR_EPDir_Msk) == 0U)) {                        /* and endpoint OUT */
            pktcnt = ((ptr_ch->HCTSIZ_BUFFERMODE & USB_CH_HCTSIZ_BUFFERMODE_PktCnt_Msk)  /*  administrate OUT transfer status */ 
                      >> USB_CH_HCTSIZ_BUFFERMODE_PktCnt_Pos);   
            mpsiz  = (ptr_ch->HCCHAR      ) & 0x000007FFU;
            if ((ptr_pipe->num_transferring >= mpsiz) && (pktcnt > 0U)) {
              ptr_pipe->num_transferred_total += ptr_pipe->num_transferring - mpsiz * pktcnt;
            }
            ptr_pipe->num_transferring = 0U;
        }

        if ((hcint & USB_CH_HCINTMSK_NakMsk_Msk)!=0U) {                                /* if NAK */
            is_nack[ch] = true;
            ptr_pipe->event |= ARM_USBH_EVENT_HANDSHAKE_NAK;
            ptr_ch->HCINT = USB_CH_HCINTMSK_NakMsk_Msk;                                /*    clear NAK interrupt */
            if ((ptr_ch->HCCHAR & USB_CH_HCCHAR_EPDir_Msk) != 0U) {                    /*    endpoint IN ? */
              if (ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) {                   /*     is endpoint of type interrupt ? */
                XMC_lTriggerHaltChannel(ptr_ch);                                       /*      trigger channel halt (after halted will be restarted in next sof) */
              } else {                                                                 /*     is endpoint not of type interrupt ?*/
                ptr_ch->HCCHAR |= USB_CH_HCCHAR_ChEna_Msk;                             /*      trigger next transfer */
              }
            } else { /* If endpoint OUT */                                             /*    endpoint OUT ? */
              XMC_lTriggerHaltChannel(ptr_ch);                                         /*     trigger channel halt */
            }
            hcint = 0U;
        }

        if ((hcint&USB_CH_HCINTMSK_StallMsk_Msk)!=0U) {                                /* if STALL */
            /*Reset the packet data toggle*/
            ptr_ch->HCINT   = USB_CH_HCINTMSK_StallMsk_Msk;                              /*  clear STALL interrupt */
            ptr_pipe->in_use = 0U;                                                       /*  release pipe */
            ptr_pipe->packet &= (~ARM_USBH_PACKET_DATA_Msk);
            ptr_pipe->packet   |=  ARM_USBH_PACKET_DATA0;
            ptr_pipe->event = ARM_USBH_EVENT_HANDSHAKE_STALL;                            /*  prepare event notification */
            XMC_lTriggerHaltChannel(ptr_ch);                                             /*  trigger channel halt */
            hcint = 0U;
        } 
        if ((hcint&USB_CH_HCINTx_ERRORS)!=0U) {                                        /* if transaction error */
            ptr_ch->HCINT = USB_CH_HCINTx_ERRORS;                                        /*  clear all error interrupt */
            ptr_pipe->in_use = 0U;                                                       /*  release pipe */
            ptr_pipe->event = ARM_USBH_EVENT_BUS_ERROR;                                  /*  prepare event notification */
            XMC_lTriggerHaltChannel(ptr_ch);                                             /*  trigger channel halt */
            hcint = 0U;
        }
        if ((ptr_pipe->transfer_active == 0U) && (ptr_pipe->in_use == 0U) && (ptr_pipe->event != 0U)) {
          XMC_USBH0_device.SignalPipeEvent_cb((ARM_USBH_EP_HANDLE)ptr_ch, ptr_pipe->event);
          ptr_pipe->event  = 0U;
        }
      }
    }
  }

  /* Handle restarts of unfinished transfers (due to NAK or ACK) */
  ptr_pipe = (XMC_USBH0_pipe_t *)(pipe);
  for (ch = 0U; ch < USBH0_MAX_PIPE_NUM; ch++) {
    if ((ptr_pipe->in_use == 1U) && (ptr_pipe->transfer_active == 0U)) {
      /* Restart periodic transfer if not in progress and interval expired */
      if (((ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT)&&(ptr_pipe->interrupt_triggered == 1U))||
          (ptr_pipe->ep_type != ARM_USB_ENDPOINT_INTERRUPT)) {
            ptr_pipe->interrupt_triggered = 0U;
            ptr_pipe->transfer_active = 1;
            /*Data toggle if NACK not received*/
            if(!is_nack[ch])
            {
              switch (ptr_pipe->packet & ARM_USBH_PACKET_DATA_Msk) {
                case ARM_USBH_PACKET_DATA0:
                  ptr_pipe->packet   &= ~ARM_USBH_PACKET_DATA_Msk;
                  ptr_pipe->packet   |=  ARM_USBH_PACKET_DATA1;
                  break;
                case ARM_USBH_PACKET_DATA1:
                  ptr_pipe->packet   &= ~ARM_USBH_PACKET_DATA_Msk;
                  ptr_pipe->packet   |=  ARM_USBH_PACKET_DATA0;
                  break;
                default:
                  break;
            }
            }
            else
            {
              is_nack[ch] = false;
            }
            XMC_lStartTransfer (ptr_pipe, (((USB0_CH_TypeDef *)(XMC_USBH0_device.host_channel_registers)) + ch));
      }
    }
    ptr_pipe++;
  }
}

/**
 * \brief Entry point for exception vector table
 *
 * This function gets called, if an usb exception (interrupt) was thrown and
 * dispatches it to \ref XMC_HandleIrq.
 */
void USB0_0_IRQHandler() {
  XMC_HandleIrq(XMC_USBH0_device.global_register->GINTSTS_HOSTMODE);
}

ARM_DRIVER_USBH Driver_USBH0 = {
  XMC_GetVersion,
  XMC_GetCapabilities,
  XMC_Initialize,
  XMC_Uninitialize,
  XMC_PowerControl,
  XMC_PortVbusOnOff,
  XMC_PortReset,
  XMC_PortSuspend,
  XMC_PortResume,
  XMC_PortGetState,
  XMC_PipeCreate,
  XMC_PipeModify,
  XMC_PipeDelete,
  XMC_PipeReset,
  XMC_PipeTransfer,
  XMC_PipeTransferGetResult,
  XMC_PipeTransferAbort,
  XMC_GetFrameNumber
};
