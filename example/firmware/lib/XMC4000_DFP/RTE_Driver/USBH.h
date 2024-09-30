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
 * @file USBH.h
 * @date 02 March, 2016
 * @version 1.0
 *
 * @brief USBH Driver for Infineon XMC4000
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#ifndef USBH_H_
#define USBH_H_
#include <stdint.h>
#include "xmc_common.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "Driver_USBH.h"
#include "RTE_Device.h"

#ifndef USBH0_MAX_PIPE_NUM
#define USBH0_MAX_PIPE_NUM             (14U)
#endif
#if    (USBH0_MAX_PIPE_NUM > 14U)
#error  Too many Pipes, maximum Pipes that this driver supports is 14 !!!
#endif

#define XMC_USBH0_CLOCK_GATING_ENABLE  1
#define XMC_USBH0_CLOCK_GATING_DISABLE 0

#define  USB_CH_HCCHARx_MPS(x)       (((uint32_t)    x   ) & USB_CH_HCCHAR_MPS_Msk)
#define  USB_CH_HCCHARx_EPNUM(x)     (((uint32_t)    x   << USB_CH_HCCHAR_EPNum_Pos) & USB_CH_HCCHAR_EPNum_Msk)
#define  USB_CH_HCCHARx_EPTYPE(x)    (((uint32_t)    x   << USB_CH_HCCHAR_EPType_Pos) & USB_CH_HCCHAR_EPType_Msk)
#define  USB_CH_HCCHARx_MCEC(x)      (((uint32_t)    x   << USB_CH_HCCHAR_MC_EC_Pos) & USB_CH_HCCHAR_MC_EC_Msk)
#define  USB_CH_HCCHARx_DEVADDR(x)   (((uint32_t)    x   << USB_CH_HCCHAR_DevAddr_Pos) & USB_CH_HCCHAR_DevAddr_Msk)
#define  USB_CH_HCCHARx_EPDIR(x)     (((uint32_t)    x   << USB_CH_HCCHAR_EPDir_Pos) & USB_CH_HCCHAR_EPDir_Msk)
#define  USB_CH_HCCHAR_LSDEV_Msk     (((uint32_t)    0x1   << 15) & 0x1)
#define  USB_CH_HCTSIZx_DPID(x)      (((uint32_t)    x   << USB_CH_HCTSIZ_BUFFERMODE_Pid_Pos) & USB_CH_HCTSIZ_BUFFERMODE_Pid_Msk)
#define  USB_CH_HCTSIZx_DPID_DATA0   (USB_CH_HCTSIZx_DPID(0U))
#define  USB_CH_HCTSIZx_DPID_DATA2   (USB_CH_HCTSIZx_DPID(1U))
#define  USB_CH_HCTSIZx_DPID_DATA1   (USB_CH_HCTSIZx_DPID(2U))
#define  USB_CH_HCTSIZx_DPID_MDATA   (USB_CH_HCTSIZx_DPID(3U))
#define  USB_CH_HCTSIZx_DPID_SETUP   (USB_CH_HCTSIZx_DPID(3U))
#define  USB_GRXSTSR_HOSTMODE_PktSts_IN_DATA_PKT 0x2
#define  USB_GRXSTSR_HOSTMODE_PktSts_IN_TRSF_CPL 0x3


#define  USB_CH_HCFG_FSLSSUP(x)      (((uint32_t)    x  <<  USB_HCFG_FSLSSupp_Pos) & USB_HCFG_FSLSSupp_Msk)
#define  USB_CH_HCFG_FSLSPCS(x)      (((uint32_t)    x        ) & USB_HCFG_FSLSPclkSel_Msk)

#define  USB_CH_HCINTx_ALL           (USB_CH_HCINTMSK_XferComplMsk_Msk | \
                                      USB_CH_HCINTMSK_ChHltdMsk_Msk | \
                                      USB_CH_HCINTMSK_StallMsk_Msk | \
                                      USB_CH_HCINTMSK_NakMsk_Msk | \
                                      USB_CH_HCINTMSK_AckMsk_Msk | \
                                      USB_CH_HCINTMSK_XactErrMsk_Msk | \
                                      USB_CH_HCINTMSK_BblErrMsk_Msk | \
                                      USB_CH_HCINTMSK_FrmOvrunMsk_Msk | \
                                      USB_CH_HCINTMSK_DataTglErrMsk_Msk)

#define  USB_CH_HCINTx_ERRORS        (USB_CH_HCINTMSK_XactErrMsk_Msk | \
                                      USB_CH_HCINTMSK_BblErrMsk_Msk | \
                                      USB_CH_HCINTMSK_FrmOvrunMsk_Msk | \
                                      USB_CH_HCINTMSK_DataTglErrMsk_Msk)

typedef struct XMC_USBH0_pipe {
  uint32_t  packet;
  uint8_t  *data;
  uint32_t  num;
  uint32_t  num_transferred_total;
  uint32_t  num_transferring;
  uint16_t  ep_max_packet_size;
  uint16_t  interval_reload;
  uint16_t  interval;
  uint8_t   ep_type;
  uint8_t   in_use;
  uint8_t   transfer_active;
  uint8_t   interrupt_triggered;
  uint8_t   event;
} XMC_USBH0_pipe_t;

static volatile XMC_USBH0_pipe_t pipe[USBH0_MAX_PIPE_NUM];

typedef struct xmc_usb_host_device {
  USB0_GLOBAL_TypeDef *global_register;           /** Global register interface            */
  USB0_CH_TypeDef *host_channel_registers;        /** Host channel interface               */
  ARM_USBH_SignalPortEvent_t SignalPortEvent_cb;  /** Port event callback; set during init */
  ARM_USBH_SignalPipeEvent_t SignalPipeEvent_cb;  /** Pipe event callback; set during init */
  bool init_done;                                 /** init status */
  ARM_POWER_STATE power_state;                    /** USB Power status */
  bool port_reset_active;                         /** Port reset state */
} XMC_USBH0_DEVICE_t;


#endif /* USBH_H_ */

