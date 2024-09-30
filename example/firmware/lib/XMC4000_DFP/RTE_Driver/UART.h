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
 * @file UART.h
 * @date 16 September, 2015
 * @version 2.7
 *
 * @brief UART Driver for Infineon XMC devices
 *
 * History
 *
 * Version 2.7 Fixed IRQ numbers for XMC1400
 *
 * Version 2.6 Adapted to xmc1400 devices<br>
 *
 * Version 2.5 Runtime variables defined as volatile<br>
 *
 * Version 2.4 Added Status Structure for handling the driver status<br>
 *
 * Version 1.0 Initial version<br>
 */

#include "Driver_USART.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "xmc_uart.h"

// UART flags
#define UART_INITIALIZED       (1 << 0)
#define UART_POWERED           (1 << 1)
#define UART_CONFIGURED        (1 << 2)
#define UART_TX_ENABLED        (1 << 3)
#define UART_RX_ENABLED        (1 << 4)

#if (UC_SERIES == XMC14)
#define USIC0_0_IRQn 9U
#define UART0_ISR    IRQ9_Handler

#define USIC0_1_IRQn 10U
#define UART1_ISR    IRQ10_Handler

#define USIC1_0_IRQn 11U
#define UART2_ISR    IRQ11_Handler

#define USIC1_1_IRQn 12U
#define UART3_ISR    IRQ12_Handler

#else

#define UART0_ISR USIC0_0_IRQHandler
#define UART1_ISR USIC0_1_IRQHandler
#define UART2_ISR USIC1_0_IRQHandler
#define UART3_ISR USIC1_1_IRQHandler
#define UART4_ISR USIC2_0_IRQHandler
#define UART5_ISR USIC2_1_IRQHandler

#endif

// UART Transfer Information (Run-Time)
typedef struct UART_TRANSFER_INFO 
{
  volatile uint32_t                rx_num;        // Total number of data to be received
  volatile uint32_t                tx_num;        // Total number of data to be send
  volatile uint8_t                 *rx_buf;       // Pointer to in data buffer
  volatile uint8_t                 *tx_buf;       // Pointer to out data buffer
  volatile uint32_t                rx_cnt;        // Number of data received
  volatile uint32_t                tx_cnt;        // Number of data sent
} UART_TRANSFER_INFO_t ;

typedef struct _UART_STATUS 
{
  volatile uint8_t tx_busy;                      // Transmitter busy flag
  volatile uint8_t rx_busy;                      // Receiver busy flag
  uint8_t tx_underflow;                 // Transmit data underflow detected (cleared on start of next send operation)
  uint8_t rx_overflow;                  // Receive data overflow detected (cleared on start of next receive operation)
  uint8_t rx_break;                     // Break detected on receive (cleared on start of next receive operation)
  uint8_t rx_framing_error;             // Framing error detected on receive (cleared on start of next receive operation)
  uint8_t rx_parity_error;              // Parity error detected on receive (cleared on start of next receive operation)
} UART_STATUS_t;

// UART Information (Run-Time)
typedef struct 
{
  ARM_USART_SignalEvent_t cb_event;           // Event callback
  UART_STATUS_t           status;             // Status flags
  UART_TRANSFER_INFO_t    xfer;               // Transfer information
  uint8_t                 flags;              // UART driver flags
  uint8_t           mode;               // UART driver flags
  volatile uint32_t       rx_fifo_pointer;    // FIFO rx pointer
  volatile uint32_t       tx_fifo_pointer;    // FIFO tx pointer
} UART_INFO;

// GPIO
typedef struct XMC_GPIO 
{
  XMC_GPIO_PORT_t *const port;
  const uint8_t pin;
} XMC_GPIO_t;


// UART Resources definitions
typedef struct 
{
  XMC_GPIO_t             pin_tx;                     // TX  Pin identifier
  XMC_GPIO_CONFIG_t      *pin_tx_config;             // TX  Pin configuration
  uint32_t               pin_tx_alternate_function;  // TX  pin alternate function              
  XMC_GPIO_t             pin_rx;                     // RX  Pin identifier
  XMC_GPIO_CONFIG_t      *pin_rx_config;             // RX  Pin configuration
  uint32_t               input;                      // Input for RX  Pin
  XMC_USIC_CH_t          *uart;                      // Pointer to UART peripheral
  IRQn_Type              irq_num;                    // UART TX IRQ Number
  volatile uint32_t      tx_fifo_size_reg;           // FIFO tx size register
  volatile uint32_t      tx_fifo_size_num;           // FIFO tx size register num
  volatile uint32_t      rx_fifo_size_reg;           // FIFO rx size register 
  volatile uint32_t      rx_fifo_size_num;           // FIFO rx size register num
  UART_INFO              *info;                      // Run-Time Information
} const UART_RESOURCES;




