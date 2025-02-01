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
 * @file SPI.h
 * @date 16 September, 2015
 * @version 2.5
 *
 * @brief SPI Driver for Infineon XMC devices
 *
 * History
 *
 * Version 2.5 Adapted to xmc1400 devices<br>
 *
 * Version 2.4 Added Status Structure for handling the driver status<br>
 *
 * Version 1.0 Initial version<br>
 */

#include "Driver_SPI.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "xmc_spi.h"

// SPI flags
#define SPI_INITIALIZED       (1 << 0)
#define SPI_POWERED           (1 << 1)

#if (UC_SERIES == XMC14)
/*!< USIC SR0 Interrupt */
#define USIC0_0_IRQn 9U
#define SPI0_ISR IRQ9_Handler

/*!< USIC SR2 Interrupt (TX) */
#define USIC0_1_IRQn 10U
#define SPI1_ISR IRQ10_Handler

/*!< USIC SR0 Interrupt (TX) */
#define USIC1_0_IRQn 11U
#define SPI2_ISR IRQ11_Handler

/*!< USIC SR2 Interrupt (TX) */
#define USIC1_1_IRQn 12U
#define SPI3_ISR IRQ12_Handler

#else

#define SPI0_ISR USIC0_0_IRQHandler
#define SPI1_ISR USIC0_1_IRQHandler
#define SPI2_ISR USIC1_0_IRQHandler
#define SPI3_ISR USIC1_1_IRQHandler
#define SPI4_ISR USIC2_0_IRQHandler
#define SPI5_ISR USIC2_1_IRQHandler

#endif


// GPIO
typedef struct XMC_GPIO 
{
  XMC_GPIO_PORT_t *const port;
  const uint8_t pin;
} XMC_GPIO_t;

// SPI Transfer Information (Run-Time)
typedef struct SPI_TRANSFER_INFO 
{
  volatile uint32_t                num;            // Total number of data
  volatile uint8_t                *rx_buf;        // Pointer to in data buffer
  volatile uint8_t                *tx_buf;        // Pointer to out data buffer
  volatile uint32_t                rx_cnt;         // Number of data received
  volatile uint32_t                tx_cnt;         // Number of data transmitted
  uint8_t                          def_val;        // Default transfer value
  volatile uint32_t                baudrate;       // Baudrate value
  volatile uint8_t                 data_bytes;     // Number of data bytes
  volatile uint8_t                 bit_order;      // Bit order (MSB to LSB or LSB to MSB)
} SPI_TRANSFER_INFO_t ;

typedef struct SPI_STATUS 
{
  uint8_t busy;              // Transmitter/Receiver busy flag
  uint8_t data_lost;         // Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
  uint8_t mode_fault;        // Mode fault detected; optional (cleared on start of transfer operation)
} SPI_STATUS_t;

// SPI Information (Run-Time)
typedef struct 
{
  ARM_SPI_SignalEvent_t          cb_event;                  // Event callback
  SPI_STATUS_t                   status;                    // Status flags
  volatile SPI_TRANSFER_INFO_t   xfer;                      // Transfer information
  uint8_t                        mode;                      // SPI mode
  uint8_t                        flags;                     // SPI driver flags
  volatile uint32_t              rx_fifo_pointer;           // FIFO rx pointer
  volatile uint32_t              tx_fifo_pointer;           // FIFO tx pointer
} SPI_INFO;



// SPI Resources definitions
typedef struct 
{
  XMC_GPIO_t               miso_mosi_port;                            // MISO(master) MOSI(slave)  Pin identifier
  XMC_GPIO_CONFIG_t       *pin_miso_mosi_config;                      // MISO(master) MOSI(slave)  Pin configuration
  uint32_t                 miso_mosi_input;                           // MISO(master) MOSI(slave)  Input configuration
  XMC_GPIO_t               mosi_miso_port;                            // MOSI(master) MISO(slave) Pin identifier
  XMC_GPIO_CONFIG_t       *pin_mosi_miso_config;                      // MOSI(master) MISO(slave)  Pin configuration
  uint32_t                 pin_mosi_miso_alternate_function;          // MOSI(master) MISO(slave) pin alternate function
  XMC_GPIO_t               output_clck_port;                          // Output CLCK  Pin identifier
  XMC_GPIO_CONFIG_t       *pin_output_clck_config;                    // Output CLCK  Pin configuration
  uint32_t                 pin_output_clck_alternate_function;        // Output CLCK pin alternate function
  XMC_GPIO_t               input_clck_port;                           // Input CLCK  Pin identifier
  XMC_GPIO_CONFIG_t       *pin_input_clck_config;                     // Input CLCK  Pin configuration
  uint32_t                 clck_input;                                // Input CLCK  Input configuration
  XMC_GPIO_t               slave_select_output_port;                  // SELO  Pin identifier
  XMC_GPIO_CONFIG_t       *pin_slave_select_output_config;            // SELO  Pin configuration
  uint32_t                 pin_slave_select_output_alternate_function;// Output CLCK pin alternate function
  XMC_GPIO_t               slave_select_input_port;                   // SELI  Pin identifier
  XMC_GPIO_CONFIG_t       *pin_slave_select_input_config;             // SELI  Pin configuration
  uint32_t                 slave_select_input;                        // SELI  Input
  XMC_USIC_CH_t           *spi;                                       // Pointer to SPI peripheral
  IRQn_Type                irq_rx_num;                                // SPI RX IRQ Number
  volatile uint32_t        tx_fifo_size_reg;                          // FIFO tx size register num
  volatile uint32_t        tx_fifo_size_num;                          // FIFO tx size register 
  volatile uint32_t        rx_fifo_size_reg;                          // FIFO rx size register num
  volatile uint32_t        rx_fifo_size_num;                          // FIFO rx size register
  volatile SPI_INFO       *info;                                      // Run-Time Information
  uint32_t                 spi_slave_select_line0;                 // Slave Select Line 0
  uint32_t                 spi_slave_select_line1;                 // Slave Select Line 1
  uint32_t                 spi_slave_select_line2;                 // Slave Select Line 2
  uint32_t                 spi_slave_select_line3;                 // Slave Select Line 3
  uint32_t                 spi_slave_select_line4;                 // Slave Select Line 4
  uint32_t                 spi_slave_select_line5;                 // Slave Select Line 5
  uint32_t                 spi_slave_select_line6;                 // Slave Select Line 6
  uint32_t                 spi_slave_select_line7;                 // Slave Select Line 7
} const SPI_RESOURCES;


