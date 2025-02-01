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
 * @file SAI.h
 * @date XXXX
 * @version 1.0
 *
 * @brief SAI CMSIS Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#include "Driver_SAI.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "xmc_i2s.h"

// I2S flags
#define I2S_INITIALIZED     (      1U)
#define I2S_POWERED         (1U << 1U)
#define I2S_CONFIGURED      (1U << 2U)

#if (UC_SERIES == XMC14)

/*!< USIC0 SR0 Interrupt */
#define  USIC0_0_IRQn      9U
#define  I2S0_ISR          IRQ9_Handler

/*!< USIC0 SR1 Interrupt */
#define  USIC0_1_IRQn      10U
#define  I2S1_ISR          IRQ10_Handler

/*!< USIC0 SR2 Interrupt */
#define  USIC1_0_IRQn      11U
#define  I2S2_ISR          IRQ11_Handler

/*!< USIC0 SR3 Interrupt */
#define  USIC1_1_IRQn      12U
#define  I2S3_ISR          IRQ12_Handler

#else

#define I2S0_ISR USIC0_0_IRQHandler
#define I2S1_ISR USIC0_1_IRQHandler
#define I2S2_ISR USIC1_0_IRQHandler
#define I2S3_ISR USIC1_1_IRQHandler
#define I2S4_ISR USIC2_0_IRQHandler
#define I2S5_ISR USIC2_1_IRQHandler

#endif

// GPIO
typedef struct XMC_GPIO 
{
  XMC_GPIO_PORT_t *const port;
  const uint8_t pin;
} XMC_GPIO_t;

// I2S transfer related information (Run-Time)
typedef struct _I2S_TRANSFER_INFO 
{
  volatile uint32_t         tx_num;                         // Total number of data to be transmitted
  volatile uint32_t         rx_num;                         // Total number of data to be received
  volatile uint8_t         *tx_buf;                         // Pointer to out data buffer
  volatile uint8_t         *rx_buf;                         // Pointer to in data buffer
  volatile uint32_t         tx_cnt;                         // Number of transmitted data
  volatile uint32_t         rx_cnt;                         // Number of received data
  volatile uint8_t          data_bits;                      // Number of data bits
  volatile uint8_t          master;                         // Master flag
  volatile uint32_t         baudrate;                       // Baudrate value
  volatile uint8_t          mono_mode;                      // Mono mode
} I2S_TRANSFER_INFO;

typedef struct _I2S_STATUS 
{
  uint32_t tx_busy          : 1;        ///< Transmitter busy flag
  uint32_t rx_busy          : 1;        ///< Receiver busy flag
  uint32_t tx_underflow     : 1;        ///< Transmit data underflow detected (cleared on start of next send operation)
  uint32_t rx_overflow      : 1;        ///< Receive data overflow detected (cleared on start of next receive operation)
  uint32_t frame_error      : 1;        ///< Sync Frame error detected (cleared on start of next send/receive operation)
} I2S_STATUS;

// Run-time I2S information
typedef struct _I2S_INFO
{
  ARM_SAI_SignalEvent_t     cb_event;                       // Event callback
  I2S_STATUS                status;                         // Status flags
  uint32_t                  tx_fifo_pointer;                // FIFO TX pointer
  uint32_t                  rx_fifo_pointer;                // FIFO RX pointer
  I2S_TRANSFER_INFO         xfer;                           // Transfer (transmit/receive) information
  uint32_t                  mode;                           // I2S mode
  uint8_t                   flags;                          // I2S driver flags (node dependent)
} I2S_INFO;

// I2S resources definitions
typedef struct 
{
  XMC_GPIO_t                sd_tx_port;                     // SD TX pin identifier
  uint32_t                  sd_tx_alternate_function;       // SD TX pin alternate function
  XMC_GPIO_t                sd_rx_port;                     // SD RX pin identifier
  uint32_t                  sd_rx_input;                    // SD RX pin input configuration
  XMC_GPIO_t                clk_output_port;                // CLK OUTPUT pin identifier
  uint32_t                  clk_output_alternate_function;  // CLK OUTPUT pin alternate function
  XMC_GPIO_t                clk_input_port;                 // CLK INPUT pin identifier
  uint32_t                  clk_input_input;                // CLK INPUT pin input configuration
  XMC_GPIO_t                ws_output_port;                 // Word select OUTPUT pin identifier
  uint32_t                  ws_output_alternate_function;   // Word select OUTPUT pin alternate function
  XMC_GPIO_t                ws_input_port;                  // Word select INPUT pin identifier
  uint32_t                  ws_input_input;                 // Word select INPUT pin input configuration
  XMC_GPIO_t                mclk_output_port;               // MCLK (Master Clock) OUTPUT pin identifier
  uint32_t                  mclk_output_alternate_function; // MCLK (Master Clock) OUTPUT pin alternate function
  IRQn_Type                 irq_num;                        // I2S transmit IRQ Number
  uint32_t                  tx_fifo_size_num;               // FIFO TX size register num
  uint32_t                  tx_fifo_size_reg;               // FIFO TX size register
  uint32_t                  rx_fifo_size_num;               // FIFO RX size register num
  uint32_t                  rx_fifo_size_reg;               // FIFO RX size register
  uint8_t                   mute_mode_enabled;              // Mute mode selected
  volatile I2S_INFO        *info;                           // Pointer to run-time information
  XMC_USIC_CH_t            *i2s;                            // Pointer to I2S peripheral
} I2S_RESOURCES;
