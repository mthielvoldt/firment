/*
 * Copyright (c) 2015, Infineon Technologies AG
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
 * @file CAN.h
 * @date 19 July, 2016
 * @version 1.0
 *
 * @brief CAN Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#include "Driver_CAN.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "xmc_can.h"

// CAN flags
#define CAN_INITIALIZED       (1 << 0)
#define CAN_POWERED           (1 << 1)

#if (UC_SERIES == XMC14)
#define  CAN0_0_IRQn         0U                            // CAN SR0 Interrupt                      
#define  CAN0_1_IRQn         1U                            // CAN SR1 Interrupt                      
#define  CAN0_ISR            IRQ0_Handler
#define  CAN1_ISR            IRQ1_Handler
#else
#define  CAN0_ISR            CAN0_0_IRQHandler
#define  CAN1_ISR            CAN0_1_IRQHandler
#define  CAN2_ISR            CAN0_2_IRQHandler
#define  CAN3_ISR            CAN0_3_IRQHandler
#define  CAN4_ISR            CAN0_4_IRQHandler
#define  CAN5_ISR            CAN0_5_IRQHandler
#endif

// GPIO
typedef struct XMC_GPIO 
{
  XMC_GPIO_PORT_t *const port;
  const uint8_t pin;
} XMC_GPIO_t;

typedef struct CAN_STATUS 
{
  uint32_t unit_state       : 4;                            // Unit bus state
  uint32_t last_error_code  : 4;                            // Last error code
  uint32_t tx_error_count   : 8;                            // Transmitter error count
  uint32_t rx_error_count   : 8;                            // Receiver error count
} CAN_STATUS_t;

// CAN Information (Run-Time)
typedef struct CAN_INFO
{
  ARM_CAN_SignalUnitEvent_t      cb_unit_event;             // Unit event callback
  ARM_CAN_SignalObjectEvent_t    cb_object_event;           // Object event callback
  CAN_STATUS_t                   status;                    // Status flags
  uint8_t                        RTR_response;              // Automatic Data Message response on RTR
  uint8_t                        flags;                     // CAN driver flags (node dependent)  
} CAN_INFO_t;

// CAN Resources definitions
typedef struct CAN_RESOURCES
{
  XMC_GPIO_t                     tx_port;                   // TX pin identifier
  uint32_t                       tx_alternate_function;     // TX pin alternate function
  XMC_GPIO_t                     rx_port;                   // RX pin identifier
  uint32_t                       rx_input;                  // RX pin input configuration
  IRQn_Type                      irq_num;                   // CAN IRQ Number
  const uint8_t                  current_node_index;        // Currently used CAN node index
  uint32_t                       mo_index;                  // MO allocated to the current node
  volatile CAN_INFO_t            *info;                     // Pointer to run-time information
  XMC_CAN_t                      *can;                      // Pointer to CAN peripheral
  XMC_CAN_NODE_t                 *can_node;                 // Pointer to CAN Node
  XMC_CAN_MO_t                   *can_mo;                   // Pointer to Message Object
} CAN_RESOURCES_t;
