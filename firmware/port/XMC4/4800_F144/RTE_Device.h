/**
 * @file fmt_RTE_Device.h
 * @date 12 Nov, 2024
 * @version 1.0
 *
 * @brief Run-Time Environment interconnections for Infineon XMC48/47_F144
 * This file contains just the fixed-in-silicon interconnection information and
 * excludes all project-specific config.  It can therefore be used, unmodified,
 * by any project building for XMC48/47_F144.
 * 
 * Usage Requirements
 * ------------------
 * This file is meant to be used in concert with a collection of files providing
 * pcb and project details.  See "example/firmware/config/spi_pcbDetails.h".
 * Find template pcb detail files in ./template_pcbDetails/
 * 
 * This File's Origins
 * -------------------
 * The RTE_Device.h file provided by manufacturers in CMSIS Device Family Packs
 * contains two types of information:
 * 1. Fixed-in-silicon peripheral interconnections that are available for use.
 *    eg: CAN module 0 may use pins 0.0, 1.4, 2.0, and 3.10 for Tx.
 * 2. Project-specific configuration which selects the inteconnections to use.
 *    eg: make CAN module 0 available and use pin 1.4 for Tx with this module. 
 *    These lines of code are generally edited by BSP software, not by hand.
 * 
 * As mentioned above, this file contains only type 1. information.
 * This version extracted information from Version 1.1.0 (30 June, 2016) of
 * XMC4000_DFP/RTE_Driver/Config/XMC4800_F144/RTE_Device.h
 */

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

#pragma once

#include "RTE_Components.h"
#include "xmc_device.h"
#include "xmc_gpio.h"
#include "xmc4_gpio_map.h"
#include "xmc4_usic_map.h"

#define NO_FIFO 0
#define FIFO_SIZE_2 1
#define FIFO_SIZE_4 2
#define FIFO_SIZE_8 3
#define FIFO_SIZE_16 4
#define FIFO_SIZE_32 5

#ifdef RTE_Drivers_CAN
// <e> CAN0 (Controller area network) [Driver_CAN0]
// <i> Configuration settings for Driver_CAN0 in component ::Drivers:CAN
//   <o> CAN0 TX Pin <0=>P0_0 <1=>P1_4 <2=>P2_0 <3=>P3_2 <4=>P3_10
#if    (RTE_CAN0_TX_ID == 0)
#define RTE_CAN0_TX_PORT              P0_0
#define RTE_CAN0_TX_AF                P0_0_AF_CAN_N0_TXD
#elif  (RTE_CAN0_TX_ID == 1)
#define RTE_CAN0_TX_PORT              P1_4
#define RTE_CAN0_TX_AF                P1_4_AF_CAN_N0_TXD
#elif  (RTE_CAN0_TX_ID == 2)
#define RTE_CAN0_TX_PORT              P2_0
#define RTE_CAN0_TX_AF                P2_0_AF_CAN_N0_TXD
#elif  (RTE_CAN0_TX_ID == 3)
#define RTE_CAN0_TX_PORT              P3_2
#define RTE_CAN0_TX_AF                P3_2_AF_CAN_N0_TXD
#elif  (RTE_CAN0_TX_ID == 4)
#define RTE_CAN0_TX_PORT              P3_10
#define RTE_CAN0_TX_AF                P3_10_AF_CAN_N0_TXD
#else
#error "Invalid CAN0_TX Pin Configuration!"
#endif

//   <o> CAN0 RX Pin <0=>P1_5 <1=>P3_12 <2=>P14_3
#if    (RTE_CAN0_RX_ID == 0)
#define RTE_CAN0_RX_PORT              P1_5
#define RTE_CAN0_RX_INPUT             CAN_NODE0_RXD_P1_5
#elif  (RTE_CAN0_RX_ID == 1)
#define RTE_CAN0_RX_PORT              P3_12
#define RTE_CAN0_RX_INPUT             CAN_NODE0_RXD_P3_12
#elif  (RTE_CAN0_RX_ID == 2)
#define RTE_CAN0_RX_PORT              P14_3
#define RTE_CAN0_RX_INPUT             CAN_NODE0_RXD_P14_3
#else
#error "Invalid CAN0_RX Pin Configuration!"
#endif
// </e>

// <e> CAN1 (Controller area network) [Driver_CAN1]
// <i> Configuration settings for Driver_CAN1 in component ::Drivers:CAN
//   <o> CAN1 TX Pin <0=>P1_5 <1=>P1_12 <2=>P2_7 <3=>P3_9
#if    (RTE_CAN1_TX_ID == 0)
#define RTE_CAN1_TX_PORT              P1_5
#define RTE_CAN1_TX_AF                P1_5_AF_CAN_N1_TXD
#elif  (RTE_CAN1_TX_ID == 1)
#define RTE_CAN1_TX_PORT              P1_12
#define RTE_CAN1_TX_AF                P1_12_AF_CAN_N1_TXD
#elif  (RTE_CAN1_TX_ID == 2)
#define RTE_CAN1_TX_PORT              P2_7
#define RTE_CAN1_TX_AF                P2_7_AF_CAN_N1_TXD
#elif  (RTE_CAN1_TX_ID == 3)
#define RTE_CAN1_TX_PORT              P3_9
#define RTE_CAN1_TX_AF                P3_9_AF_CAN_N1_TXD
#else
#error "Invalid CAN1_TX Pin Configuration!"
#endif

//   <o> CAN1 RX Pin <0=>P1_4 <1=>P1_13 <2=>P2_6 <3=>P3_11
#if    (RTE_CAN1_RX_ID == 0)
#define RTE_CAN1_RX_PORT              P1_4
#define RTE_CAN1_RX_INPUT             CAN_NODE1_RXD_P1_4
#elif  (RTE_CAN1_RX_ID == 1)
#define RTE_CAN1_RX_PORT              P1_13
#define RTE_CAN1_RX_INPUT             CAN_NODE1_RXD_P1_13
#elif  (RTE_CAN1_RX_ID == 2)
#define RTE_CAN1_RX_PORT              P2_6
#define RTE_CAN1_RX_INPUT             CAN_NODE1_RXD_P2_6
#elif  (RTE_CAN1_RX_ID == 3)
#define RTE_CAN1_RX_PORT              P3_11
#define RTE_CAN1_RX_INPUT             CAN_NODE1_RXD_P3_11
#else
#error "Invalid CAN1_RX Pin Configuration!"
#endif
// </e>

// <e> CAN2 (Controller area network) [Driver_CAN2]
// <i> Configuration settings for Driver_CAN2 in component ::Drivers:CAN
//   <o> CAN2 TX Pin <0=>P1_9 <1=>P3_7 <2=>P4_7
#if    (RTE_CAN2_TX_ID == 0)
#define RTE_CAN2_TX_PORT              P1_9
#define RTE_CAN2_TX_AF                P1_9_AF_CAN_N2_TXD
#elif  (RTE_CAN2_TX_ID == 1)
#define RTE_CAN2_TX_PORT              P3_7
#define RTE_CAN2_TX_AF                P3_7_AF_CAN_N2_TXD
#elif  (RTE_CAN2_TX_ID == 2)
#define RTE_CAN2_TX_PORT              P4_7
#define RTE_CAN2_TX_AF                P4_7_AF_CAN_N2_TXD
#else
#error "Invalid CAN2_TX Pin Configuration!"
#endif

//   <o> CAN2 RX Pin <0=>P1_8 <1=>P3_8 <2=>P4_6
#if    (RTE_CAN2_RX_ID == 0)
#define RTE_CAN2_RX_PORT              P1_8
#define RTE_CAN2_RX_INPUT             CAN_NODE2_RXD_P1_8
#elif  (RTE_CAN2_RX_ID == 1)
#define RTE_CAN2_RX_PORT              P3_8
#define RTE_CAN2_RX_INPUT             CAN_NODE2_RXD_P3_8
#elif  (RTE_CAN2_RX_ID == 2)
#define RTE_CAN2_RX_PORT              P4_6
#define RTE_CAN2_RX_INPUT             CAN_NODE2_RXD_P4_6
#else
#error "Invalid CAN2_RX Pin Configuration!"
#endif
// </e>

// <e> CAN3 (Controller area network) [Driver_CAN3]
// <i> Configuration settings for Driver_CAN3 in component ::Drivers:CAN
//   <o> CAN3 TX Pin <0=>P4_0 <1=>P6_5
#if    (RTE_CAN3_TX_ID == 0)
#define RTE_CAN3_TX_PORT              P4_0
#define RTE_CAN3_TX_AF                P4_0_AF_CAN_N3_TXD
#elif  (RTE_CAN3_TX_ID == 1)
#define RTE_CAN3_TX_PORT              P6_5
#define RTE_CAN3_TX_AF                P6_5_AF_CAN_N3_TXD
#else
#error "Invalid CAN3_TX Pin Configuration!"
#endif

//   <o> CAN3 RX Pin <0=>P0_8 <1=>P6_6
#if    (RTE_CAN3_RX_ID == 0)
#define RTE_CAN3_RX_PORT              P0_8
#define RTE_CAN3_RX_INPUT             CAN_NODE3_RXD_P0_8
#elif  (RTE_CAN3_RX_ID == 1)
#define RTE_CAN3_RX_PORT              P6_6
#define RTE_CAN3_RX_INPUT             CAN_NODE3_RXD_P6_6
#else
#error "Invalid CAN3_RX Pin Configuration!"
#endif
// </e>

// <e> CAN4 (Controller area network) [Driver_CAN4]
// <i> Configuration settings for Driver_CAN4 in component ::Drivers:CAN
//   <o> CAN4 TX Pin <0=>P2_14 <1=>P5_8
#if    (RTE_CAN4_TX_ID == 0)
#define RTE_CAN4_TX_PORT              P2_14
#define RTE_CAN4_TX_AF                P2_14_AF_CAN_N4_TXD
#elif  (RTE_CAN4_TX_ID == 1)
#define RTE_CAN4_TX_PORT              P5_8
#define RTE_CAN4_TX_AF                P5_8_AF_CAN_N4_TXD
#else
#error "Invalid CAN4_TX Pin Configuration!"
#endif

//   <o> CAN4 RX Pin <0=>P2_15 <2=>P14_4
#if    (RTE_CAN4_RX_ID == 0)
#define RTE_CAN4_RX_PORT              P2_15
#define RTE_CAN4_RX_INPUT             CAN_NODE4_RXD_P2_15
#elif  (RTE_CAN4_RX_ID == 2)
#define RTE_CAN4_RX_PORT              P14_4
#define RTE_CAN4_RX_INPUT             CAN_NODE4_RXD_P14_4
#else
#error "Invalid CAN4_RX Pin Configuration!"
#endif
// </e>

// <e> CAN5 (Controller area network) [Driver_CAN5]
// <i> Configuration settings for Driver_CAN5 in component ::Drivers:CAN
//   <o> CAN5 TX Pin <0=>P2_1 <1=>P5_11
#if    (RTE_CAN5_TX_ID == 0)
#define RTE_CAN5_TX_PORT              P2_1
#define RTE_CAN5_TX_AF                P2_1_AF_CAN_N5_TXD
#elif  (RTE_CAN5_TX_ID == 1)
#define RTE_CAN5_TX_PORT              P5_11
#define RTE_CAN5_TX_AF                P5_11_AF_CAN_N5_TXD
#else
#error "Invalid CAN5_TX Pin Configuration!"
#endif

//   <o> CAN5 RX Pin <0=>P2_6 <1=>P5_10
#if    (RTE_CAN5_RX_ID == 0)
#define RTE_CAN5_RX_PORT              P2_6
#define RTE_CAN5_RX_INPUT             CAN_NODE5_RXD_P2_6
#elif  (RTE_CAN5_RX_ID == 1)
#define RTE_CAN5_RX_PORT              P5_10
#define RTE_CAN5_RX_INPUT             CAN_NODE5_RXD_P5_10
#else
#error "Invalid CAN5_RX Pin Configuration!"
#endif
// </e>
#endif // RTE_Drivers_CAN

#ifdef RTE_Drivers_UART
// <e> UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]
// <i> Configuration settings for Driver_USART0 in component ::Drivers:UART
//   <o> UART0_TX Pin <0=>P1_5 <1=>P1_7 <2=>P5_1
#if    (RTE_UART0_TX_ID == 0)
#define RTE_UART0_TX_PORT              P1_5
#define RTE_UART0_TX_AF                P1_5_AF_U0C0_DOUT0
#elif  (RTE_UART0_TX_ID == 1)
#define RTE_UART0_TX_PORT              P1_7
#define RTE_UART0_TX_AF                P1_7_AF_U0C0_DOUT0
#elif  (RTE_UART0_TX_ID == 2)
#define RTE_UART0_TX_PORT              P5_1
#define RTE_UART0_TX_AF                P5_1_AF_U0C0_DOUT0
#else
#error "Invalid UART0_TX Pin Configuration!"
#endif

//   <o> UART0_RX Pin <0=>P1_4 <1=>P1_5 <2=>P4_7 <3=>P5_0
#if    (RTE_UART0_RX_ID == 0)
#define RTE_UART0_RX_PORT              P1_4
#define RTE_UART0_RX_INPUT             USIC0_C0_DX0_P1_4
#elif  (RTE_UART0_RX_ID == 1)
#define RTE_UART0_RX_PORT              P1_5
#define RTE_UART0_RX_INPUT             USIC0_C0_DX0_P1_5
#elif  (RTE_UART0_RX_ID == 2)
#define RTE_UART0_RX_PORT              P4_7
#define RTE_UART0_RX_INPUT             USIC0_C0_DX0_P4_7
#elif  (RTE_UART0_RX_ID == 3)
#define RTE_UART0_RX_PORT              P5_0
#define RTE_UART0_RX_INPUT             USIC0_C0_DX0_P5_0
#else
#error "Invalid UART0_RX Pin Configuration!"
#endif

//   <o> UART0_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART0_RX_FIFO_SIZE_ID == 0)
#define RTE_UART0_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART0_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART0_RX_FIFO_SIZE_ID == 1)
#define RTE_UART0_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART0_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART0_RX_FIFO_SIZE_ID == 2)
#define RTE_UART0_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART0_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART0_RX_FIFO_SIZE_ID == 3)
#define RTE_UART0_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART0_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART0_RX_FIFO_SIZE_ID == 4)
#define RTE_UART0_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART0_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART0_RX_FIFO_SIZE_ID == 5)
#define RTE_UART0_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART0_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART0_RX FIFO SIZE Configuration!"
#endif

//   <o> UART0_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32 
#if    (RTE_UART0_TX_FIFO_SIZE_ID == 0)
#define RTE_UART0_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART0_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART0_TX_FIFO_SIZE_ID == 1)
#define RTE_UART0_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART0_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART0_TX_FIFO_SIZE_ID == 2)
#define RTE_UART0_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART0_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART0_TX_FIFO_SIZE_ID == 3)
#define RTE_UART0_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART0_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART0_TX_FIFO_SIZE_ID == 4)
#define RTE_UART0_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART0_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART0_TX_FIFO_SIZE_ID == 5)
#define RTE_UART0_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART0_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART0_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]
// <i> Configuration settings for Driver_USART1 in component ::Drivers:UART
//   <o> UART1_TX Pin <0=>P2_5 <1=>P3_5 <2=>P3_13 <3=>P6_4
#if    (RTE_UART1_TX_ID == 0)
#define RTE_UART1_TX_PORT              P2_5
#define RTE_UART1_TX_AF                P2_5_AF_U0C1_DOUT0
#elif  (RTE_UART1_TX_ID == 1)
#define RTE_UART1_TX_PORT              P3_5
#define RTE_UART1_TX_AF                P3_5_AF_U0C1_DOUT0
#elif  (RTE_UART1_TX_ID == 2)
#define RTE_UART1_TX_PORT              P3_13
#define RTE_UART1_TX_AF                P3_13_AF_U0C1_DOUT0
#elif  (RTE_UART1_TX_ID == 3)
#define RTE_UART1_TX_PORT              P6_4
#define RTE_UART1_TX_AF                P6_4_AF_U0C1_DOUT0
#else
#error "Invalid UART1_TX Pin Configuration!"
#endif

//   <o> UART1_RX Pin <0=>P2_2 <1=>P2_5 <2=>P3_13 <3=>P4_0 <4=>P6_3
#if    (RTE_UART1_RX_ID == 0)
#define RTE_UART1_RX_PORT              P2_2
#define RTE_UART1_RX_INPUT             USIC0_C1_DX0_P2_2
#elif  (RTE_UART1_RX_ID == 1)
#define RTE_UART1_RX_PORT              P2_5
#define RTE_UART1_RX_INPUT             USIC0_C1_DX0_P2_5
#elif  (RTE_UART1_RX_ID == 2)
#define RTE_UART1_RX_PORT              P3_13
#define RTE_UART1_RX_INPUT             USIC0_C1_DX0_P3_13
#elif  (RTE_UART1_RX_ID == 3)
#define RTE_UART1_RX_PORT              P4_0
#define RTE_UART1_RX_INPUT             USIC0_C1_DX0_P4_0
#elif  (RTE_UART1_RX_ID == 4)
#define RTE_UART1_RX_PORT              P6_3
#define RTE_UART1_RX_INPUT             USIC0_C1_DX0_P6_3
#else
#error "Invalid UART1_RX Pin Configuration!"
#endif

//   <o> UART1_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART1_RX_FIFO_SIZE_ID == 0)
#define RTE_UART1_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART1_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART1_RX_FIFO_SIZE_ID == 1)
#define RTE_UART1_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART1_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART1_RX_FIFO_SIZE_ID == 2)
#define RTE_UART1_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART1_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART1_RX_FIFO_SIZE_ID == 3)
#define RTE_UART1_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART1_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART1_RX_FIFO_SIZE_ID == 4)
#define RTE_UART1_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART1_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART1_RX_FIFO_SIZE_ID == 5)
#define RTE_UART1_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART1_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART1_RX FIFO SIZE Configuration!"
#endif

//   <o> UART1_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32 
#if    (RTE_UART1_TX_FIFO_SIZE_ID == 0)
#define RTE_UART1_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART1_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART1_TX_FIFO_SIZE_ID == 1)
#define RTE_UART1_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART1_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART1_TX_FIFO_SIZE_ID == 2)
#define RTE_UART1_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART1_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART1_TX_FIFO_SIZE_ID == 3)
#define RTE_UART1_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART1_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART1_TX_FIFO_SIZE_ID == 4)
#define RTE_UART1_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART1_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART1_TX_FIFO_SIZE_ID == 5)
#define RTE_UART1_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART1_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART1_TX FIFO SIZE Configuration!"
#endif
//</e>
// <e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]
// <i> Configuration settings for Driver_USART2 in component ::Drivers:UART
//   <o> UART2_TX Pin <0=>P0_5 <1=>P1_15 <2=>P2_14
#if    (RTE_UART2_TX_ID == 0)
#define RTE_UART2_TX_PORT              P0_5
#define RTE_UART2_TX_AF                P0_5_AF_U1C0_DOUT0
#elif  (RTE_UART2_TX_ID == 1)
#define RTE_UART2_TX_PORT              P1_15
#define RTE_UART2_TX_AF                P1_15_AF_U1C0_DOUT0
#elif  (RTE_UART2_TX_ID == 2)
#define RTE_UART2_TX_PORT              P2_14
#define RTE_UART2_TX_AF                P2_14_AF_U1C0_DOUT0
#else
#error "Invalid UART2_TX Pin Configuration!"
#endif

//   <o> UART2_RX Pin <0=>P0_4 <1=>P0_5 <2=>P1_14 <3=>P2_14 <4=>P2_15
#if    (RTE_UART2_RX_ID == 0)
#define RTE_UART2_RX_PORT              P0_4
#define RTE_UART2_RX_INPUT             USIC1_C0_DX0_P0_4
#elif  (RTE_UART2_RX_ID == 1)
#define RTE_UART2_RX_PORT              P0_5
#define RTE_UART2_RX_INPUT             USIC1_C0_DX0_P0_5
#elif  (RTE_UART2_RX_ID == 2)
#define RTE_UART2_RX_PORT              P1_14
#define RTE_UART2_RX_INPUT             USIC1_C0_DX0_P1_14
#elif  (RTE_UART2_RX_ID == 3)
#define RTE_UART2_RX_PORT              P2_14
#define RTE_UART2_RX_INPUT             USIC1_C0_DX0_P2_14
#elif  (RTE_UART2_RX_ID == 4)
#define RTE_UART2_RX_PORT              P2_15
#define RTE_UART2_RX_INPUT             USIC1_C0_DX0_P2_15
#else
#error "Invalid UART2_RX Pin Configuration!"
#endif

//   <o> UART2_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART2_RX_FIFO_SIZE_ID == 0)
#define RTE_UART2_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART2_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART2_RX_FIFO_SIZE_ID == 1)
#define RTE_UART2_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART2_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART2_RX_FIFO_SIZE_ID == 2)
#define RTE_UART2_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART2_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART2_RX_FIFO_SIZE_ID == 3)
#define RTE_UART2_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART2_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART2_RX_FIFO_SIZE_ID == 4)
#define RTE_UART2_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART2_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART2_RX_FIFO_SIZE_ID == 5)
#define RTE_UART2_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART2_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART2_RX FIFO SIZE Configuration!"
#endif

//   <o> UART2_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32 
#if    (RTE_UART2_TX_FIFO_SIZE_ID == 0)
#define RTE_UART2_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART2_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART2_TX_FIFO_SIZE_ID == 1)
#define RTE_UART2_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART2_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART2_TX_FIFO_SIZE_ID == 2)
#define RTE_UART2_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART2_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART2_TX_FIFO_SIZE_ID == 3)
#define RTE_UART2_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART2_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART2_TX_FIFO_SIZE_ID == 4)
#define RTE_UART2_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART2_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART2_TX_FIFO_SIZE_ID == 5)
#define RTE_UART2_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART2_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART2_TX FIFO SIZE Configuration!"
#endif
//</e>
// <e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::Drivers:UART
//   <o> UART3_TX Pin <0=>P0_1 <1=>P1_9 <2=>P3_15 <3=>P4_2
#if    (RTE_UART3_TX_ID == 0)
#define RTE_UART3_TX_PORT              P0_1
#define RTE_UART3_TX_AF                P0_1_AF_U1C1_DOUT0
#elif  (RTE_UART3_TX_ID == 1)
#define RTE_UART3_TX_PORT              P1_9
#define RTE_UART3_TX_AF                P1_9_AF_U1C1_DOUT0
#elif  (RTE_UART3_TX_ID == 2)
#define RTE_UART3_TX_PORT              P3_15
#define RTE_UART3_TX_AF                P3_15_AF_U1C1_DOUT0
#elif  (RTE_UART3_TX_ID == 3)
#define RTE_UART3_TX_PORT              P4_2
#define RTE_UART3_TX_AF                P4_2_AF_U1C1_DOUT0
#else
#error "Invalid UART3_TX Pin Configuration!"
#endif

//   <o> UART3_RX Pin  <0=>P0_0 <1=>P3_14 <2=>P3_15 <3=>P4_2
#if    (RTE_UART3_RX_ID == 0)
#define RTE_UART3_RX_PORT              P0_0
#define RTE_UART3_RX_INPUT             USIC1_C1_DX0_P0_0
#elif  (RTE_UART3_RX_ID == 1)
#define RTE_UART3_RX_PORT              P3_14
#define RTE_UART3_RX_INPUT             USIC1_C1_DX0_P3_14
#elif  (RTE_UART3_RX_ID == 2)
#define RTE_UART3_RX_PORT              P3_15
#define RTE_UART3_RX_INPUT             USIC1_C1_DX0_P3_15
#elif  (RTE_UART3_RX_ID == 3)
#define RTE_UART3_RX_PORT              P4_2
#define RTE_UART3_RX_INPUT             USIC1_C1_DX0_P4_2
#else
#error "Invalid UART3_RX Pin Configuration!"
#endif

//   <o> UART3_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART3_RX_FIFO_SIZE_ID == 0)
#define RTE_UART3_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART3_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART3_RX_FIFO_SIZE_ID == 1)
#define RTE_UART3_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART3_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART3_RX_FIFO_SIZE_ID == 2)
#define RTE_UART3_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART3_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART3_RX_FIFO_SIZE_ID == 3)
#define RTE_UART3_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART3_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART3_RX_FIFO_SIZE_ID == 4)
#define RTE_UART3_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART3_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART3_RX_FIFO_SIZE_ID == 5)
#define RTE_UART3_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART3_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART3_RX FIFO SIZE Configuration!"
#endif

//   <o> UART3_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART3_TX_FIFO_SIZE_ID == 0)
#define RTE_UART3_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART3_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART3_TX_FIFO_SIZE_ID == 1)
#define RTE_UART3_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART3_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART3_TX_FIFO_SIZE_ID == 2)
#define RTE_UART3_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART3_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART3_TX_FIFO_SIZE_ID == 3)
#define RTE_UART3_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART3_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART3_TX_FIFO_SIZE_ID == 4)
#define RTE_UART3_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART3_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART3_TX_FIFO_SIZE_ID == 5)
#define RTE_UART3_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART3_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART3_TX FIFO SIZE Configuration!"
#endif

//</e>
// <e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]
// <i> Configuration settings for Driver_USART4 in component ::Drivers:UART
//   <o> UART4_TX Pin <0=>P3_8 <1=>P5_0 <2=>P6_6
#if    (RTE_UART4_TX_ID == 0)
#define RTE_UART4_TX_PORT              P3_8
#define RTE_UART4_TX_AF                P3_8_AF_U2C0_DOUT0
#elif  (RTE_UART4_TX_ID == 1)
#define RTE_UART4_TX_PORT              P5_0
#define RTE_UART4_TX_AF                P5_0_AF_U2C0_DOUT0
#elif  (RTE_UART4_TX_ID == 2)
#define RTE_UART4_TX_PORT              P6_6
#define RTE_UART4_TX_AF                P6_6_AF_U2C0_DOUT0
#else
#error "Invalid UART4_TX Pin Configuration!"
#endif

//   <o> UART4_RX Pin <0=>P3_7 <1=>P5_0 <2=>P5_1 <3=>P6_5
#if    (RTE_UART4_RX_ID == 0)
#define RTE_UART4_RX_PORT              P3_7
#define RTE_UART4_RX_INPUT             USIC2_C0_DX0_P3_7
#elif  (RTE_UART4_RX_ID == 1)
#define RTE_UART4_RX_PORT              P5_0
#define RTE_UART4_RX_INPUT             USIC2_C0_DX0_P5_0
#elif  (RTE_UART4_RX_ID == 2)
#define RTE_UART4_RX_PORT              P5_1
#define RTE_UART4_RX_INPUT             USIC2_C0_DX0_P5_1
#elif  (RTE_UART4_RX_ID == 3)
#define RTE_UART4_RX_PORT              P6_5
#define RTE_UART4_RX_INPUT             USIC2_C0_DX0_P6_5
#else
#error "Invalid UART4_RX Pin Configuration!"
#endif

//   <o> UART4_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART4_RX_FIFO_SIZE_ID == 0)
#define RTE_UART4_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART4_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART4_RX_FIFO_SIZE_ID == 1)
#define RTE_UART4_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART4_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART4_RX_FIFO_SIZE_ID == 2)
#define RTE_UART4_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART4_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART4_RX_FIFO_SIZE_ID == 3)
#define RTE_UART4_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART4_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART4_RX_FIFO_SIZE_ID == 4)
#define RTE_UART4_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART4_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART4_RX_FIFO_SIZE_ID == 5)
#define RTE_UART4_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART4_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART4_RX FIFO SIZE Configuration!"
#endif

//   <o> UART4_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART4_TX_FIFO_SIZE_ID == 0)
#define RTE_UART4_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART4_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART4_TX_FIFO_SIZE_ID == 1)
#define RTE_UART4_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART4_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART4_TX_FIFO_SIZE_ID == 2)
#define RTE_UART4_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART4_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART4_TX_FIFO_SIZE_ID == 3)
#define RTE_UART4_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART4_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART4_TX_FIFO_SIZE_ID == 4)
#define RTE_UART4_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART4_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART4_TX_FIFO_SIZE_ID == 5)
#define RTE_UART4_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART4_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART4_TX FIFO SIZE Configuration!"
#endif
//</e>
// <e> UART5 (Universal asynchronous receiver transmitter) [Driver_USART5]
// <i> Configuration settings for Driver_USART5 in component ::Drivers:UART
//   <o> UART5_TX Pin <0=>P3_5 <1=>P3_11 <2=>P4_7
#if    (RTE_UART5_TX_ID == 0)
#define RTE_UART5_TX_PORT              P3_5
#define RTE_UART5_TX_AF                P3_5_AF_U2C1_DOUT0
#elif  (RTE_UART5_TX_ID == 1)
#define RTE_UART5_TX_PORT              P3_11
#define RTE_UART5_TX_AF                P3_11_AF_U2C1_DOUT0
#elif  (RTE_UART5_TX_ID == 2)
#define RTE_UART5_TX_PORT              P4_7
#define RTE_UART5_TX_AF                P4_7_AF_U2C1_DOUT0
#else
#error "Invalid UART5_TX Pin Configuration!"
#endif

//   <o> UART5_RX Pin <0=>P3_4 <1=>P3_5 <2=>P3_12 <3=>P4_0 <4=>P4_6
#if    (RTE_UART5_RX_ID == 0)
#define RTE_UART5_RX_PORT              P3_4
#define RTE_UART5_RX_INPUT             USIC2_C1_DX0_P3_4
#elif  (RTE_UART5_RX_ID == 1)
#define RTE_UART5_RX_PORT              P3_5
#define RTE_UART5_RX_INPUT             USIC2_C1_DX0_P3_5
#elif  (RTE_UART5_RX_ID == 2)
#define RTE_UART5_RX_PORT              P3_12
#define RTE_UART5_RX_INPUT             USIC2_C1_DX0_P3_12
#elif  (RTE_UART5_RX_ID == 3)
#define RTE_UART5_RX_PORT              P4_0
#define RTE_UART5_RX_INPUT             USIC2_C1_DX0_P4_0
#elif  (RTE_UART5_RX_ID == 4)
#define RTE_UART5_RX_PORT              P4_6
#define RTE_UART5_RX_INPUT             USIC2_C1_DX0_P4_6
#else
#error "Invalid UART5_RX Pin Configuration!"
#endif

//   <o> UART5_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART5_RX_FIFO_SIZE_ID == 0)
#define RTE_UART5_RX_FIFO_SIZE         NO_FIFO
#define RTE_UART5_RX_FIFO_SIZE_NUM     0
#elif  (RTE_UART5_RX_FIFO_SIZE_ID == 1)
#define RTE_UART5_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART5_RX_FIFO_SIZE_NUM     2
#elif  (RTE_UART5_RX_FIFO_SIZE_ID == 2)
#define RTE_UART5_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART5_RX_FIFO_SIZE_NUM     4
#elif  (RTE_UART5_RX_FIFO_SIZE_ID == 3)
#define RTE_UART5_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART5_RX_FIFO_SIZE_NUM     8
#elif  (RTE_UART5_RX_FIFO_SIZE_ID == 4)
#define RTE_UART5_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART5_RX_FIFO_SIZE_NUM     16
#elif  (RTE_UART5_RX_FIFO_SIZE_ID == 5)
#define RTE_UART5_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART5_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART5_RX FIFO SIZE Configuration!"
#endif

//   <o> UART5_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_UART5_TX_FIFO_SIZE_ID == 0)
#define RTE_UART5_TX_FIFO_SIZE         NO_FIFO
#define RTE_UART5_TX_FIFO_SIZE_NUM     0
#elif  (RTE_UART5_TX_FIFO_SIZE_ID == 1)
#define RTE_UART5_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_UART5_TX_FIFO_SIZE_NUM     2
#elif  (RTE_UART5_TX_FIFO_SIZE_ID == 2)
#define RTE_UART5_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_UART5_TX_FIFO_SIZE_NUM     4
#elif  (RTE_UART5_TX_FIFO_SIZE_ID == 3)
#define RTE_UART5_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_UART5_TX_FIFO_SIZE_NUM     8
#elif  (RTE_UART5_TX_FIFO_SIZE_ID == 4)
#define RTE_UART5_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_UART5_TX_FIFO_SIZE_NUM     16
#elif  (RTE_UART5_TX_FIFO_SIZE_ID == 5)
#define RTE_UART5_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_UART5_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid UART5_TX FIFO SIZE Configuration!"
#endif
// </e>
#endif // RTE_Drivers_UART

#ifdef RTE_Drivers_SPI
// <e> SPI0 (Serial peripheral interface) [Driver_SPI0]
// <i> Configuration settings for Driver_SPI0 in component ::Drivers:SPI
//   <o> SPI0 TX: MOSI(master) MISO(slave) Pin <0=>P1_5 <1=>P1_7 <2=>P5_1
#if    (RTE_SPI0_TX_ID == 0)
#define RTE_SPI0_TX_PORT              P1_5
#define RTE_SPI0_TX_AF                P1_5_AF_U0C0_DOUT0
#elif  (RTE_SPI0_TX_ID == 1)
#define RTE_SPI0_TX_PORT              P1_7
#define RTE_SPI0_TX_AF                P1_5_AF_U0C0_DOUT0
#elif  (RTE_SPI0_TX_ID == 2)
#define RTE_SPI0_TX_PORT              P5_1
#define RTE_SPI0_TX_AF                P5_1_AF_U0C0_DOUT0
#else
#error "Invalid SPI0_TX Pin Configuration!"
#endif

//   <o> SPI0 RX MISO(master) MOSI(slave) Pin <0=>P1_4 <1=>P1_5 <2=>P4_7 <3=>P5_0
#if    (RTE_SPI0_RX_ID == 0)
#define RTE_SPI0_RX_PORT              P1_4
#define RTE_SPI0_RX_INPUT             USIC0_C0_DX0_P1_4
#elif  (RTE_SPI0_RX_ID == 1)
#define RTE_SPI0_RX_PORT              P1_5
#define RTE_SPI0_RX_INPUT             USIC0_C0_DX0_P1_5
#elif  (RTE_SPI0_RX_ID == 2)
#define RTE_SPI0_RX_PORT              P4_7
#define RTE_SPI0_RX_INPUT             USIC0_C0_DX0_P4_7
#elif  (RTE_SPI0_RX_ID == 3)
#define RTE_SPI0_RX_PORT              P5_0
#define RTE_SPI0_RX_INPUT             USIC0_C0_DX0_P5_0
#else
#error "Invalid SPI0_RX Pin Configuration!"
#endif

//   <o> SPI0_CLK OUTPUT Pin <0=>P0_8 <1=>P1_1 <2=>P1_6 <3=>P1_9 <4=>P1_10
#if    (RTE_SPI0_CLK_OUTPUT_ID == 0)
#define RTE_SPI0_CLK_OUTPUT_PORT              P0_8
#define RTE_SPI0_CLK_AF                       P0_8_AF_U0C0_SCLKOUT 
#elif  (RTE_SPI0_CLK_OUTPUT_ID == 1)
#define RTE_SPI0_CLK_OUTPUT_PORT              P1_1
#define RTE_SPI0_CLK_AF                       P1_1_AF_U0C0_SCLKOUT
#elif  (RTE_SPI0_CLK_OUTPUT_ID == 2)
#define RTE_SPI0_CLK_OUTPUT_PORT              P1_6
#define RTE_SPI0_CLK_AF                       P1_6_AF_U0C0_SCLKOUT
#elif  (RTE_SPI0_CLK_OUTPUT_ID == 3)
#define RTE_SPI0_CLK_OUTPUT_PORT              P1_9
#define RTE_SPI0_CLK_AF                       P1_9_AF_U0C0_SCLKOUT
#elif  (RTE_SPI0_CLK_OUTPUT_ID == 4)
#define RTE_SPI0_CLK_OUTPUT_PORT              P1_10
#define RTE_SPI0_CLK_AF                       P1_10_AF_U0C0_SCLKOUT
#else
#error "Invalid SPI0 CLOCK OUTPUT Pin Configuration!"
#endif

// <h> SPI0_SLAVE SELECT Pins 
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI0_SLAVE SELECT LINE 0 Pin <0=>P0_7 <1=>P1_0 <2=>P1_11
#if    (RTE_SPI0_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_0_PORT              P0_7
#define RTE_SPI0_SLAVE_SELECT_LINE_0_AF                P0_7_AF_U0C0_SELO0
#elif  (RTE_SPI0_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI0_SLAVE_SELECT_LINE_0_PORT              P1_0
#define RTE_SPI0_SLAVE_SELECT_LINE_0_AF                P1_0_AF_U0C0_SELO0
#elif  (RTE_SPI0_SLAVE_SELECT_LINE_0_ID == 2)
#define RTE_SPI0_SLAVE_SELECT_LINE_0_PORT              P1_11
#define RTE_SPI0_SLAVE_SELECT_LINE_0_AF                P1_11_AF_U0C0_SELO0
#else
#error "Invalid SPI0 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI0_SLAVE SELECT LINE 1 Pin <0=>P1_8
#if    (RTE_SPI0_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_1_PORT              P1_8
#define RTE_SPI0_SLAVE_SELECT_LINE_1_AF                P1_8_AF_U0C0_SELO1
#else
#error "Invalid SPI0 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
//</e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI0_SLAVE SELECT LINE 2 Pin <0=>P4_6
#if    (RTE_SPI0_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_2_PORT              P4_6
#define RTE_SPI0_SLAVE_SELECT_LINE_2_AF                P4_6_AF_U0C0_SELO2
#else
#error "Invalid SPI0 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 3
// <i> Enable slave select line 3
//   <o> SPI0_SLAVE SELECT LINE 3 Pin <0=>P4_5
#if    (RTE_SPI0_SLAVE_SELECT_LINE_3_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_3_PORT              P4_5
#define RTE_SPI0_SLAVE_SELECT_LINE_3_AF                P4_5_AF_U0C0_SELO3
#else
#error "Invalid SPI0 SLAVE SELECT LINE 3 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 4
// <i> Enable slave select line 4
//   <o> SPI0_SLAVE SELECT LINE 4 Pin <0=>P4_4
#if    (RTE_SPI0_SLAVE_SELECT_LINE_4_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_4_PORT              P4_4
#define RTE_SPI0_SLAVE_SELECT_LINE_4_AF                P4_4_AF_U0C0_SELO4
#else
#error "Invalid SPI0 SLAVE SELECT LINE 4 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 5
// <i> Enable slave select line 5
// <o> SPI0_SLAVE SELECT LINE 5 Pin <0=>P4_3
#if    (RTE_SPI0_SLAVE_SELECT_LINE_5_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_LINE_5_PORT              P4_3
#define RTE_SPI0_SLAVE_SELECT_LINE_5_AF                P4_3_AF_U0C0_SELO5
#else
#error "Invalid SPI0 SLAVE SELECT LINE 5 Pin Configuration!"
#endif
// </e>
// </h>

//   <o> SPI0_CLK INPUT Pin <0=>P0_8 <1=>P1_1
#if    (RTE_SPI0_CLK_INPUT_ID == 0)
#define RTE_SPI0_CLK_INPUT_PORT              P0_8
#define RTE_SPI0_CLK_INPUT                   USIC0_C0_DX1_P0_8
#elif  (RTE_SPI0_CLK_INPUT_ID == 1)
#define RTE_SPI0_CLK_INPUT_PORT              P1_1
#define RTE_SPI0_CLK_INPUT                   USIC0_C0_DX1_P1_1
#else
#error "Invalid SPI0 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI0_SLAVE_SELECT INPUT Pin <0=>P0_7 <1=>P1_0
#if    (RTE_SPI0_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI0_SLAVE_SELECT_INPUT_PORT              P0_7
#define RTE_SPI0_SLAVE_SELECT_INPUT                   USIC0_C0_DX2_P0_7
#elif  (RTE_SPI0_SLAVE_SELECT_INPUT_ID == 1)
#define RTE_SPI0_SLAVE_SELECT_INPUT_PORT              P1_0
#define RTE_SPI0_SLAVE_SELECT_INPUT                   USIC0_C0_DX2_P1_0
#else
#error "Invalid SPI0 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI0_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI0_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI0_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI0_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI0_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI0_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI0_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI0_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI0_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI0_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI0_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI0_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI0_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI0_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI0_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI0_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI0_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI0_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI0_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI0_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI0_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI0_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI0_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI0_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI0_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI0_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI0_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI0_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI0_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI0_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI0_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI0_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI0_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI0_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI0_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI0_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI0_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI0_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI0_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI0_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> SPI1 (Serial peripheral interface) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::Drivers:SPI
//   <o> SPI1 TX MOSI(master) MISO(slave) Pin <0=>P2_5 <1=>P3_5 <2=>P3_13 <3=>P6_4
#if    (RTE_SPI1_TX_ID == 0)
#define RTE_SPI1_TX_PORT              P2_5
#define RTE_SPI1_TX_AF                P2_5_AF_U0C1_DOUT0
#elif  (RTE_SPI1_TX_ID == 1)
#define RTE_SPI1_TX_PORT              P3_5
#define RTE_SPI1_TX_AF                P3_5_AF_U0C1_DOUT0
#elif  (RTE_SPI1_TX_ID == 2)
#define RTE_SPI1_TX_PORT              P3_13
#define RTE_SPI1_TX_AF                P3_13_AF_U0C1_DOUT0
#elif  (RTE_SPI1_TX_ID == 3)
#define RTE_SPI1_TX_PORT              P6_4
#define RTE_SPI1_TX_AF                P6_4_AF_U0C1_DOUT0
#else
#error "Invalid SPI1_TX Pin Configuration!"
#endif

//   <o> SPI1 RX MISO(master) MOSI(slave) Pin <0=>P2_2 <1=>P2_5 <2=>P3_13 <3=>P4_0 <4=>P6_3
#if    (RTE_SPI1_RX_ID == 0)
#define RTE_SPI1_RX_PORT              P2_2
#define RTE_SPI1_RX_INPUT             USIC0_C1_DX0_P2_2
#elif  (RTE_SPI1_RX_ID == 1)
#define RTE_SPI1_RX_PORT              P2_5
#define RTE_SPI1_RX_INPUT             USIC0_C1_DX0_P2_5
#elif  (RTE_SPI1_RX_ID == 2)
#define RTE_SPI1_RX_PORT              P3_13
#define RTE_SPI1_RX_INPUT             USIC0_C1_DX0_P3_13
#elif  (RTE_SPI1_RX_ID == 3)
#define RTE_SPI1_RX_PORT              P4_0
#define RTE_SPI1_RX_INPUT             USIC0_C1_DX0_P4_0
#elif  (RTE_SPI1_RX_ID == 4)
#define RTE_SPI1_RX_PORT              P6_3
#define RTE_SPI1_RX_INPUT             USIC0_C1_DX0_P6_3
#else
#error "Invalid SPI1_RX Pin Configuration!"
#endif


//   <o> SPI1_CLK OUTPUT Pin <0=>P2_4 <1=>P3_0 <2=>P3_6 <3=>P6_2
#if    (RTE_SPI1_CLK_OUTPUT_ID == 0)
#define RTE_SPI1_CLK_OUTPUT_PORT              P2_4
#define RTE_SPI1_CLK_AF                       P2_4_AF_U0C1_SCLKOUT
#elif  (RTE_SPI1_CLK_OUTPUT_ID == 1)
#define RTE_SPI1_CLK_OUTPUT_PORT              P3_0
#define RTE_SPI1_CLK_AF                       P3_0_AF_U0C1_SCLKOUT
#elif  (RTE_SPI1_CLK_OUTPUT_ID == 2)
#define RTE_SPI1_CLK_OUTPUT_PORT              P3_6
#define RTE_SPI1_CLK_AF                       P3_6_AF_U0C1_SCLKOUT
#elif  (RTE_SPI1_CLK_OUTPUT_ID == 3)
#define RTE_SPI1_CLK_OUTPUT_PORT              P6_2
#define RTE_SPI1_CLK_AF                       P6_2_AF_U0C1_SCLKOUT
#else
#error "Invalid SPI1 CLOCK OUTPUT Pin Configuration!"
#endif

// <h> SPI1_SLAVE SELECT Pins
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI1_SLAVE SELECT LINE 0 Pin <0=>P2_3 <1=>P3_1 <2=>P4_1 <3=>P6_1
#if    (RTE_SPI1_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI1_SLAVE_SELECT_LINE_0_PORT              P2_3
#define RTE_SPI1_SLAVE_SELECT_LINE_0_AF                P2_3_AF_U0C1_SELO0
#elif  (RTE_SPI1_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI1_SLAVE_SELECT_LINE_0_PORT              P3_1
#define RTE_SPI1_SLAVE_SELECT_LINE_0_AF                P3_1_AF_U0C1_SELO0
#elif  (RTE_SPI1_SLAVE_SELECT_LINE_0_ID == 2)
#define RTE_SPI1_SLAVE_SELECT_LINE_0_PORT              P4_1
#define RTE_SPI1_SLAVE_SELECT_LINE_0_AF                P4_1_AF_U0C1_SELO0
#elif  (RTE_SPI1_SLAVE_SELECT_LINE_0_ID == 3)
#define RTE_SPI1_SLAVE_SELECT_LINE_0_PORT              P6_1
#define RTE_SPI1_SLAVE_SELECT_LINE_0_AF                P6_1_AF_U0C1_SELO0
#else
#error "Invalid SPI1 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI1_SLAVE SELECT LINE 1 Pin <0=>P3_12 <1=>P6_0
#if    (RTE_SPI1_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI1_SLAVE_SELECT_LINE_1_PORT              P3_12
#define RTE_SPI1_SLAVE_SELECT_LINE_1_AF                P3_12_AF_U0C1_SELO1
#elif  (RTE_SPI1_SLAVE_SELECT_LINE_1_ID == 1)
#define RTE_SPI1_SLAVE_SELECT_LINE_1_PORT              P6_0
#define RTE_SPI1_SLAVE_SELECT_LINE_1_AF                P6_0_AF_U0C1_SELO1
#else
#error "Invalid SPI1 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI1_SLAVE SELECT LINE 2 Pin <0=>P1_14  <1=>P3_11
#if    (RTE_SPI1_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI1_SLAVE_SELECT_LINE_2_PORT              P1_14
#define RTE_SPI1_SLAVE_SELECT_LINE_2_AF                P1_14_AF_U0C1_SELO2
#elif  (RTE_SPI1_SLAVE_SELECT_LINE_2_ID == 1)
#define RTE_SPI1_SLAVE_SELECT_LINE_2_PORT              P3_11
#define RTE_SPI1_SLAVE_SELECT_LINE_2_AF                P3_11_AF_U0C1_SELO2
#else
#error "Invalid SPI1 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 3
// <i> Enable slave select line 3
//   <o> SPI1_SLAVE SELECT LINE 3 Pin <0=>P1_13 <1=>P3_8
#if    (RTE_SPI1_SLAVE_SELECT_LINE_3_ID == 0)
#define RTE_SPI1_SLAVE_SELECT_LINE_3_PORT              P1_13
#define RTE_SPI1_SLAVE_SELECT_LINE_3_AF                P1_13_AF_U0C1_SELO3
#elif    (RTE_SPI1_SLAVE_SELECT_LINE_3_ID == 1)
#define RTE_SPI1_SLAVE_SELECT_LINE_3_PORT              P3_8
#define RTE_SPI1_SLAVE_SELECT_LINE_3_AF                P3_8_AF_U0C1_SELO3
#else
#error "Invalid SPI1 SLAVE SELECT LINE 3 Pin Configuration!"
#endif
// </e>
// </h>

//   <o> SPI1_CLK INPUT Pin <0=>P2_4 <1=>P3_0 <2=>P6_2 
#if    (RTE_SPI1_CLK_INPUT_ID == 0)
#define RTE_SPI1_CLK_INPUT_PORT              P2_4
#define RTE_SPI1_CLK_INPUT                   USIC0_C1_DX1_P2_4
#elif  (RTE_SPI1_CLK_INPUT_ID == 1)
#define RTE_SPI1_CLK_INPUT_PORT              P3_0
#define RTE_SPI1_CLK_INPUT                   USIC0_C1_DX1_P3_0
#elif  (RTE_SPI1_CLK_INPUT_ID == 2)
#define RTE_SPI1_CLK_INPUT_PORT              P6_2
#define RTE_SPI1_CLK_INPUT                   USIC0_C1_DX1_P6_2
#else
#error "Invalid SPI1 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI1_SLAVE_SELECT INPUT Pin <0=>P2_3 <1=>P3_1 <2=>P6_1
#if    (RTE_SPI1_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI1_SLAVE_SELECT_INPUT_PORT              P2_3
#define RTE_SPI1_SLAVE_SELECT_INPUT                   USIC0_C1_DX2_P2_3
#elif  (RTE_SPI1_SLAVE_SELECT_INPUT_ID == 1)
#define RTE_SPI1_SLAVE_SELECT_INPUT_PORT              P3_1
#define RTE_SPI1_SLAVE_SELECT_INPUT                   USIC0_C1_DX2_P3_1
#elif  (RTE_SPI1_SLAVE_SELECT_INPUT_ID == 2)
#define RTE_SPI1_SLAVE_SELECT_INPUT_PORT              P6_1
#define RTE_SPI1_SLAVE_SELECT_INPUT                   USIC0_C1_DX2_P6_1
#else
#error "Invalid SPI1 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI1_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI1_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI1_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI1_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI1_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI1_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI1_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI1_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI1_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI1_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI1_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI1_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI1_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI1_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI1_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI1_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI1_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI1_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI1_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI1_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI1_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI1_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI1_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI1_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI1_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI1_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI1_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI1_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI1_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI1_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI1_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI1_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI1_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI1_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI1_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI1_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI1_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI1_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI1_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI1_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> SPI2 (Serial peripheral interface) [Driver_SPI2]
// <i> Configuration settings for Driver_SPI2 in component ::Drivers:SPI
//   <o> SPI2 TX MOSI(master) MISO(slave) Pin <0=>P0_5 <1=>P1_15 <2=>P2_14
#if    (RTE_SPI2_TX_ID == 0)
#define RTE_SPI2_TX_PORT              P0_5
#define RTE_SPI2_TX_AF                P0_5_AF_U1C0_DOUT0
#elif  (RTE_SPI2_TX_ID == 1)
#define RTE_SPI2_TX_PORT              P1_15
#define RTE_SPI2_TX_AF                P1_15_AF_U1C0_DOUT0
#elif  (RTE_SPI2_TX_ID == 2)
#define RTE_SPI2_TX_PORT              P2_14
#define RTE_SPI2_TX_AF                P2_14_AF_U1C0_DOUT0
#else
#error "Invalid SPI2_TX Pin Configuration!"
#endif

//   <o> SPI2 RX MISO(master) MOSI(slave) Pin <0=>P0_4 <1=>P0_5 <2=>P1_14 <3=>P2_14 <4=>P2_15
#if    (RTE_SPI2_RX_ID == 0)
#define RTE_SPI2_RX_PORT              P0_4
#define RTE_SPI2_RX_INPUT             USIC1_C0_DX0_P0_4
#elif  (RTE_SPI2_RX_ID == 1)
#define RTE_SPI2_RX_PORT              P0_5
#define RTE_SPI2_RX_INPUT             USIC1_C0_DX0_P0_5
#elif  (RTE_SPI2_RX_ID == 2)
#define RTE_SPI2_RX_PORT              P1_14
#define RTE_SPI2_RX_INPUT             USIC1_C0_DX0_P1_14
#elif  (RTE_SPI2_RX_ID == 3)
#define RTE_SPI2_RX_PORT              P2_14
#define RTE_SPI2_RX_INPUT             USIC1_C0_DX0_P2_14
#elif  (RTE_SPI2_RX_ID == 4)
#define RTE_SPI2_RX_PORT              P2_15
#define RTE_SPI2_RX_INPUT             USIC1_C0_DX0_P2_15
#else
#error "Invalid SPI2_RX Pin Configuration!"
#endif

//   <o> SPI2_CLK OUTPUT Pin <0=>P0_11 <1=>P4_0 <2=>P5_8
#if    (RTE_SPI2_CLK_OUTPUT_ID == 0)
#define RTE_SPI2_CLK_OUTPUT_PORT              P0_11
#define RTE_SPI2_CLK_AF                       P0_11_AF_U1C0_SCLKOUT
#elif  (RTE_SPI2_CLK_OUTPUT_ID == 1)
#define RTE_SPI2_CLK_OUTPUT_PORT              P4_0
#define RTE_SPI2_CLK_AF                       P4_0_AF_U1C0_SCLKOUT
#elif  (RTE_SPI2_CLK_OUTPUT_ID == 2)
#define RTE_SPI2_CLK_OUTPUT_PORT              P5_8
#define RTE_SPI2_CLK_AF                       P5_8_AF_U1C0_SCLKOUT
#else
#error "Invalid SPI2 CLOCK OUTPUT Pin Configuration!"
#endif

//   <h> SPI2_SLAVE SELECT Pins 
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI2_SLAVE SELECT LINE 0 Pin <0=>P0_6 <1=>P5_9
#if    (RTE_SPI2_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI2_SLAVE_SELECT_LINE_0_PORT              P0_6
#define RTE_SPI2_SLAVE_SELECT_LINE_0_AF                P0_6_AF_U1C0_SELO0
#elif  (RTE_SPI2_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI2_SLAVE_SELECT_LINE_0_PORT              P5_9
#define RTE_SPI2_SLAVE_SELECT_LINE_0_AF                P5_9_AF_U1C0_SELO0
#else
#error "Invalid SPI2 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI2_SLAVE SELECT LINE 1 Pin <0=>P0_14 <1=>P5_11
#if    (RTE_SPI2_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI2_SLAVE_SELECT_LINE_1_PORT              P0_14
#define RTE_SPI2_SLAVE_SELECT_LINE_1_AF                P0_14_AF_U1C0_SELO1
#elif  (RTE_SPI2_SLAVE_SELECT_LINE_1_ID == 1)
#define RTE_SPI2_SLAVE_SELECT_LINE_1_PORT              P5_11
#define RTE_SPI2_SLAVE_SELECT_LINE_1_AF                P5_11_AF_U1C0_SELO1
#else
#error "Invalid SPI2 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI2_SLAVE SELECT LINE 2 Pin <0=>P0_15
#if    (RTE_SPI2_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI2_SLAVE_SELECT_LINE_2_PORT              P0_15
#define RTE_SPI2_SLAVE_SELECT_LINE_2_AF                P0_15_AF_U1C0_SELO2
#else
#error "Invalid SPI2 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
// </e>
// <e> SLAVE SELECT LINE 3
// <i> Enable slave select line 3
//   <o> SPI2_SLAVE SELECT LINE 3 Pin <0=>P3_14
#if    (RTE_SPI2_SLAVE_SELECT_LINE_3_ID == 0)
#define RTE_SPI2_SLAVE_SELECT_LINE_3_PORT              P3_14
#define RTE_SPI2_SLAVE_SELECT_LINE_3_AF                P3_14_AF_U1C0_SELO3
#else
#error "Invalid SPI2 SLAVE SELECT LINE 3 Pin Configuration!"
#endif
// </e>
// </h>

//   <o> SPI2_CLK INPUT Pin <0=>P0_11 <1=>P5_8
#if    (RTE_SPI2_CLK_INPUT_ID == 0)
#define RTE_SPI2_CLK_INPUT_PORT              P0_11
#define RTE_SPI2_CLK_INPUT                   USIC1_C0_DX1_P0_11
#elif  (RTE_SPI2_CLK_INPUT_ID == 1)
#define RTE_SPI2_CLK_INPUT_PORT              P5_8
#define RTE_SPI2_CLK_INPUT                   USIC1_C0_DX1_P5_8
#else
#error "Invalid SPI2 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI2_SLAVE_SELECT INPUT Pin <0=>P0_6 <1=>P5_9
#if    (RTE_SPI2_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI2_SLAVE_SELECT_INPUT_PORT              P0_6
#define RTE_SPI2_SLAVE_SELECT_INPUT                   USIC1_C0_DX2_P0_6
#elif  (RTE_SPI2_SLAVE_SELECT_INPUT_ID == 1)
#define RTE_SPI2_SLAVE_SELECT_INPUT_PORT              P5_9
#define RTE_SPI2_SLAVE_SELECT_INPUT                   USIC1_C0_DX2_P5_9
#else
#error "Invalid SPI2 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI2_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI2_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI2_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI2_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI2_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI2_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI2_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI2_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI2_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI2_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI2_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI2_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI2_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI2_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI2_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI2_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI2_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI2_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI2_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI2_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI2_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI2_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI2_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI2_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI2_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI2_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI2_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI2_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI2_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI2_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI2_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI2_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI2_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI2_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI2_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI2_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI2_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI2_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI2_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI2_TX FIFO SIZE Configuration!"
#endif
//</e>
// <e> SPI3 (Serial peripheral interface) [Driver_SPI3]
// <i> Configuration settings for Driver_SPI3 in component ::Drivers:SPI
//   <o> SPI3 TX MOSI(master) MISO(slave) Pin <0=>P0_1 <1=>P1_9 <2=>P3_15 <3=>P4_2
#if    (RTE_SPI3_TX_ID == 0)
#define RTE_SPI3_TX_PORT              P0_1
#define RTE_SPI3_TX_AF                P0_1_AF_U1C1_DOUT0
#elif  (RTE_SPI3_TX_ID == 1)
#define RTE_SPI3_TX_PORT              P1_9
#define RTE_SPI3_TX_AF                P1_9_AF_U1C1_DOUT0
#elif  (RTE_SPI3_TX_ID == 2)
#define RTE_SPI3_TX_PORT              P3_15
#define RTE_SPI3_TX_AF                P3_15_AF_U1C1_DOUT0
#elif  (RTE_SPI3_TX_ID == 3)
#define RTE_SPI3_TX_PORT              P4_2
#define RTE_SPI3_TX_AF                P4_2_AF_U1C1_DOUT0
#else
#error "Invalid SPI3_TX Pin Configuration!"
#endif

//   <o> SPI3 RX MISO(master) MOSI(slave)Pin <0=>P0_0 <1=>P3_14 <2=>P3_15 <3=>P4_2 
#if    (RTE_SPI3_RX_ID == 0)
#define RTE_SPI3_RX_PORT              P0_0
#define RTE_SPI3_RX_INPUT             USIC1_C1_DX0_P0_0
#elif  (RTE_SPI3_RX_ID == 1)
#define RTE_SPI3_RX_PORT              P3_14
#define RTE_SPI3_RX_INPUT             USIC1_C1_DX0_P3_14
#elif  (RTE_SPI3_RX_ID == 2)
#define RTE_SPI3_RX_PORT              P3_15
#define RTE_SPI3_RX_INPUT             USIC1_C1_DX0_P3_15
#elif  (RTE_SPI3_RX_ID == 3)
#define RTE_SPI3_RX_PORT              P4_2
#define RTE_SPI3_RX_INPUT             USIC1_C1_DX0_P4_2
#else
#error "Invalid SPI3_RX Pin Configuration!"
#endif

//   <o> SPI3_CLK OUTPUT Pin <0=>P0_10 <1=>P0_13 <2=>P1_8
#if    (RTE_SPI3_CLK_OUTPUT_ID == 0)
#define RTE_SPI3_CLK_OUTPUT_PORT              P0_10
#define RTE_SPI3_CLK_AF                       P0_10_AF_U1C1_SCLKOUT
#elif  (RTE_SPI3_CLK_OUTPUT_ID == 1)
#define RTE_SPI3_CLK_OUTPUT_PORT              P0_13
#define RTE_SPI3_CLK_AF                       P0_13_AF_U1C1_SCLKOUT
#elif  (RTE_SPI3_CLK_OUTPUT_ID == 2)
#define RTE_SPI3_CLK_OUTPUT_PORT              P1_8
#define RTE_SPI3_CLK_AF                       P1_8_AF_U1C1_SCLKOUT
#else
#error "Invalid SPI3 CLOCK OUTPUT Pin Configuration!"
#endif

//   <h> SPI3_SLAVE SELECT Pins 
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI3_SLAVE SELECT LINE 0 Pin <0=>P0_9 <1=>P0_12
#if    (RTE_SPI3_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_LINE_0_PORT              P0_9
#define RTE_SPI3_SLAVE_SELECT_LINE_0_AF                P0_9_AF_U1C1_SELO0
#elif  (RTE_SPI3_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI3_SLAVE_SELECT_LINE_0_PORT              P0_12
#define RTE_SPI3_SLAVE_SELECT_LINE_0_AF                P0_12_AF_U1C1_SELO0
#else
#error "Invalid SPI3 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI3_SLAVE SELECT LINE 1 Pin <0=>P0_2 <1=>P3_3
#if    (RTE_SPI3_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_LINE_1_PORT              P0_2
#define RTE_SPI3_SLAVE_SELECT_LINE_1_AF                P0_2_AF_U1C1_SELO1
#elif  (RTE_SPI3_SLAVE_SELECT_LINE_1_ID == 1)
#define RTE_SPI3_SLAVE_SELECT_LINE_1_PORT              P3_3
#define RTE_SPI3_SLAVE_SELECT_LINE_1_AF                P3_3_AF_U1C1_SELO1
#else
#error "Invalid SPI3 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI3_SLAVE SELECT LINE 2 Pin <0=>P3_4
#if    (RTE_SPI3_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_LINE_2_PORT              P3_4
#define RTE_SPI3_SLAVE_SELECT_LINE_2_AF                P3_4_AF_U1C1_SELO2
#else
#error "Invalid SPI3 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 3
// <i> Enable slave select line 3
//   <o> SPI3_SLAVE SELECT LINE 3 Pin <0=>P3_5
#if    (RTE_SPI3_SLAVE_SELECT_LINE_3_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_LINE_3_PORT              P3_5
#define RTE_SPI3_SLAVE_SELECT_LINE_3_AF                P3_5_AF_U1C1_SELO3
#else
#error "Invalid SPI3 SLAVE SELECT LINE 3 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 4
// <i> Enable slave select line 4
//   <o> SPI3_SLAVE SELECT LINE 4 Pin <0=>P3_6
#if    (RTE_SPI3_SLAVE_SELECT_LINE_4_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_LINE_4_PORT              P3_6
#define RTE_SPI3_SLAVE_SELECT_LINE_4_AF                P3_6_AF_U1C1_SELO4
#else
#error "Invalid SPI3 SLAVE SELECT LINE 4 Pin Configuration!"
#endif
// </e>
// </h>

//   <o> SPI3_CLK INPUT Pin <0=>P0_10 <1=>P0_13 <2=>P4_0
#if    (RTE_SPI3_CLK_INPUT_ID == 0)
#define RTE_SPI3_CLK_INPUT_PORT              P0_10
#define RTE_SPI3_CLK_INPUT                   USIC1_C1_DX1_P0_10
#elif  (RTE_SPI3_CLK_INPUT_ID == 1)
#define RTE_SPI3_CLK_INPUT_PORT              P0_13
#define RTE_SPI3_CLK_INPUT                   USIC1_C1_DX1_P0_13
#elif  (RTE_SPI3_CLK_INPUT_ID == 2)
#define RTE_SPI3_CLK_INPUT_PORT              P4_0
#define RTE_SPI3_CLK_INPUT                   USIC1_C1_DX1_P4_0
#else
#error "Invalid SPI3 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI3_SLAVE_SELECT INPUT Pin <0=>P0_9 <1=>P0_12
#if    (RTE_SPI3_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI3_SLAVE_SELECT_INPUT_PORT              P0_9
#define RTE_SPI3_SLAVE_SELECT_INPUT                   USIC1_C1_DX2_P0_9
#elif  (RTE_SPI3_SLAVE_SELECT_INPUT_ID == 1)
#define RTE_SPI3_SLAVE_SELECT_INPUT_PORT              P0_12
#define RTE_SPI3_SLAVE_SELECT_INPUT                   USIC1_C1_DX2_P0_12
#else
#error "Invalid SPI3 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI3_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI3_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI3_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI3_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI3_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI3_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI3_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI3_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI3_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI3_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI3_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI3_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI3_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI3_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI3_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI3_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI3_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI3_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI3_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI3_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI3_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI3_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI3_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI3_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI3_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI3_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI3_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI3_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI3_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI3_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI3_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI3_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI3_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI3_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI3_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI3_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI3_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI3_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI3_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI3_TX FIFO SIZE Configuration!"
#endif

//</e>
// <e> SPI4 (Serial peripheral interface) [Driver_SPI4]
// <i> Configuration settings for Driver_SPI4 in component ::Drivers:SPI
//   <o> SPI4 TX MOSI(master) MISO(slave) Pin <0=>P3_8 <1=>P5_0 <2=>P6_6
#if    (RTE_SPI4_TX_ID == 0)
#define RTE_SPI4_TX_PORT              P3_8
#define RTE_SPI4_TX_AF                P3_8_AF_U2C0_DOUT0
#elif  (RTE_SPI4_TX_ID == 1)
#define RTE_SPI4_TX_PORT              P5_0
#define RTE_SPI4_TX_AF                P5_0_AF_U2C0_DOUT0
#elif  (RTE_SPI4_TX_ID == 2)
#define RTE_SPI4_TX_PORT              P6_6
#define RTE_SPI4_TX_AF                P6_6_AF_U2C0_DOUT0
#else
#error "Invalid SPI4_TX Pin Configuration!"
#endif

//   <o> SPI4 RX MISO(master) MOSI(slave) Pin <0=>P3_7 <1=>P5_0 <2=>P5_1 <3=>P6_5
#if    (RTE_SPI4_RX_ID == 0)
#define RTE_SPI4_RX_PORT              P3_7
#define RTE_SPI4_RX_INPUT             USIC2_C0_DX0_P3_7
#elif  (RTE_SPI4_RX_ID == 1)
#define RTE_SPI4_RX_PORT              P5_0
#define RTE_SPI4_RX_INPUT             USIC2_C0_DX0_P5_0
#elif  (RTE_SPI4_RX_ID == 2)
#define RTE_SPI4_RX_PORT              P5_1
#define RTE_SPI4_RX_INPUT             USIC2_C0_DX0_P5_1
#elif  (RTE_SPI4_RX_ID == 3)
#define RTE_SPI4_RX_PORT              P6_5
#define RTE_SPI4_RX_INPUT             USIC2_C0_DX0_P6_5
#else
#error "Invalid SPI4_RX Pin Configuration!"
#endif

//   <o> SPI4_CLK OUTPUT Pin <0=>P3_9 <1=>P5_2
#if    (RTE_SPI4_CLK_OUTPUT_ID == 0)
#define RTE_SPI4_CLK_OUTPUT_PORT              P3_9
#define RTE_SPI4_CLK_AF                       P3_9_AF_U2C0_SCLKOUT
#elif  (RTE_SPI4_CLK_OUTPUT_ID == 1)
#define RTE_SPI4_CLK_OUTPUT_PORT              P5_2
#define RTE_SPI4_CLK_AF                       P5_2_AF_U2C0_SCLKOUT
#else
#error "Invalid SPI4 CLOCK OUTPUT Pin Configuration!"
#endif

//   <h> SPI4_SLAVE SELECT Pins 
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI4_SLAVE SELECT LINE 0 Pin <0=>P3_10 <1=>P5_3
#if    (RTE_SPI4_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_LINE_0_PORT              P3_10
#define RTE_SPI4_SLAVE_SELECT_LINE_0_AF                P3_10_AF_U2C0_SELO0
#elif  (RTE_SPI4_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI4_SLAVE_SELECT_LINE_0_PORT              P5_3
#define RTE_SPI4_SLAVE_SELECT_LINE_0_AF                P5_3_AF_U2C0_SELO0
#else
#error "Invalid SPI4 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI4_SLAVE SELECT LINE 1 Pin <0=>P5_4
#if    (RTE_SPI4_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_LINE_1_PORT              P5_4
#define RTE_SPI4_SLAVE_SELECT_LINE_1_AF                P5_4_AF_U2C0_SELO1
#else
#error "Invalid SPI4 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI4_SLAVE SELECT LINE 2 Pin <0=>P5_5
#if    (RTE_SPI4_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_LINE_2_PORT              P5_5
#define RTE_SPI4_SLAVE_SELECT_LINE_2_AF                P5_5_AF_U2C0_SELO2
#else
#error "Invalid SPI4 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 3
// <i> Enable slave select line 3
//   <o> SPI4_SLAVE SELECT LINE 3 Pin <0=>P5_6
#if    (RTE_SPI4_SLAVE_SELECT_LINE_3_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_LINE_3_PORT              P5_6
#define RTE_SPI4_SLAVE_SELECT_LINE_3_AF                P5_6_AF_U2C0_SELO3
#else
#error "Invalid SPI4 SLAVE SELECT LINE 3 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 4
// <i> Enable slave select line 4
//   <o> SPI4_SLAVE SELECT LINE 4 Pin <0=>P2_6
#if    (RTE_SPI4_SLAVE_SELECT_LINE_4_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_LINE_4_PORT              P2_6
#define RTE_SPI4_SLAVE_SELECT_LINE_4_AF                P2_6_AF_U2C0_SELO4
#else
#error "Invalid SPI4 SLAVE SELECT LINE 4 Pin Configuration!"
#endif
// </e>
// </h>

//   <o> SPI4_CLK INPUT Pin <0=>P5_2
#if    (RTE_SPI4_CLK_INPUT_ID == 0)
#define RTE_SPI4_CLK_INPUT_PORT              P5_2
#define RTE_SPI4_CLK_INPUT                   USIC2_C0_DX1_P5_2
#else
#error "Invalid SPI4 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI4_SLAVE_SELECT INPUT Pin <0=>P5_3
#if    (RTE_SPI4_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI4_SLAVE_SELECT_INPUT_PORT              P5_3
#define RTE_SPI4_SLAVE_SELECT_INPUT                   USIC2_C0_DX2_P5_3
#else
#error "Invalid SPI4 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI4_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI4_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI4_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI4_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI4_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI4_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI4_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI4_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI4_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI4_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI4_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI4_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI4_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI4_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI4_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI4_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI4_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI4_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI4_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI4_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI4_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI4_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI4_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI4_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI4_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI4_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI4_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI4_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI4_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI4_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI4_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI4_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI4_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI4_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI4_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI4_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI4_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI4_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI4_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI4_TX FIFO SIZE Configuration!"
#endif

//</e>

// <e> SPI5 (Serial peripheral interface) [Driver_SPI5]
// <i> Configuration settings for Driver_SPI5 in component ::Drivers:SPI
//   <o> SPI5 TX MOSI(master) MISO(slave)Pin <0=>P3_5 <1=>P3_11 <2=>P4_7
#if    (RTE_SPI5_TX_ID == 0)
#define RTE_SPI5_TX_PORT              P3_5
#define RTE_SPI5_TX_AF                P3_5_AF_U2C1_DOUT0
#elif  (RTE_SPI5_TX_ID == 1)
#define RTE_SPI5_TX_PORT              P3_11
#define RTE_SPI5_TX_AF                P3_11_AF_U2C1_DOUT0
#elif  (RTE_SPI5_TX_ID == 2)
#define RTE_SPI5_TX_PORT              P4_7
#define RTE_SPI5_TX_AF                P4_7_AF_U2C1_DOUT0
#else
#error "Invalid SPI5_TX Pin Configuration!"
#endif

//   <o> SPI5 RX MISO(master) MOSI(slave) <0=>P3_4 <1=>P3_5 <2=>P3_12 <3=>P4_0 <4=>P4_6
#if    (RTE_SPI5_RX_ID == 0)
#define RTE_SPI5_RX_PORT              P3_4
#define RTE_SPI5_RX_INPUT             USIC2_C1_DX0_P3_4
#elif  (RTE_SPI5_RX_ID == 1)
#define RTE_SPI5_RX_PORT              P3_5
#define RTE_SPI5_RX_INPUT             USIC2_C1_DX0_P3_5
#elif  (RTE_SPI5_RX_ID == 3)
#define RTE_SPI5_RX_PORT              P3_12
#define RTE_SPI5_RX_INPUT             USIC2_C1_DX0_P3_12
#elif  (RTE_SPI5_RX_ID == 3)
#define RTE_SPI5_RX_PORT              P4_0
#define RTE_SPI5_RX_INPUT             USIC2_C1_DX0_P4_0
#elif  (RTE_SPI5_RX_ID == 4)
#define RTE_SPI5_RX_PORT              P4_6
#define RTE_SPI5_RX_INPUT             USIC2_C1_DX0_P4_6
#else
#error "Invalid SPI5_RX Pin Configuration!"
#endif

//   <o> SPI5_CLK OUTPUT Pin <0=>P3_6 <1=>P3_13 <2=>P4_2
#if    (RTE_SPI5_CLK_OUTPUT_ID == 0)
#define RTE_SPI5_CLK_OUTPUT_PORT              P3_6
#define RTE_SPI5_CLK_AF                       P3_6_AF_U2C1_SCLKOUT
#elif  (RTE_SPI5_CLK_OUTPUT_ID == 1)
#define RTE_SPI5_CLK_OUTPUT_PORT              P3_13
#define RTE_SPI5_CLK_AF                       P3_13_AF_U2C1_SCLKOUT
#elif  (RTE_SPI5_CLK_OUTPUT_ID == 2)
#define RTE_SPI5_CLK_OUTPUT_PORT              P4_2
#define RTE_SPI5_CLK_AF                       P4_2_AF_U2C1_SCLKOUT
#else
#error "Invalid SPI5 CLOCK OUTPUT Pin Configuration!"
#endif

//   <h> SPI5_SLAVE SELECT Pins 
// <e> SLAVE SELECT LINE 0
// <i> Enable slave select line 0
//   <o> SPI5_SLAVE SELECT LINE 0 Pin <0=>P3_0 <1=>P4_1
#if    (RTE_SPI5_SLAVE_SELECT_LINE_0_ID == 0)
#define RTE_SPI5_SLAVE_SELECT_LINE_0_PORT              P3_0
#define RTE_SPI5_SLAVE_SELECT_LINE_0_AF                P3_0_AF_U2C1_SELO0
#elif  (RTE_SPI5_SLAVE_SELECT_LINE_0_ID == 1)
#define RTE_SPI5_SLAVE_SELECT_LINE_0_PORT              P4_1
#define RTE_SPI5_SLAVE_SELECT_LINE_0_AF                P4_1_AF_U2C1_SELO0
#else
#error "Invalid SPI5 SLAVE SELECT LINE 0 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 1
// <i> Enable slave select line 1
//   <o> SPI5_SLAVE SELECT LINE 1 Pin <0=>P4_2
#if    (RTE_SPI5_SLAVE_SELECT_LINE_1_ID == 0)
#define RTE_SPI5_SLAVE_SELECT_LINE_1_PORT              P4_2
#define RTE_SPI5_SLAVE_SELECT_LINE_1_AF                P4_2_AF_U2C1_SELO1
#else
#error "Invalid SPI5 SLAVE SELECT LINE 1 Pin Configuration!"
#endif
// </e>

// <e> SLAVE SELECT LINE 2
// <i> Enable slave select line 2
//   <o> SPI5_SLAVE SELECT LINE 2 Pin <0=>P4_3
#if    (RTE_SPI5_SLAVE_SELECT_LINE_2_ID == 0)
#define RTE_SPI5_SLAVE_SELECT_LINE_2_PORT              P4_3
#define RTE_SPI5_SLAVE_SELECT_LINE_2_AF                P4_3_AF_U2C1_SELO2
#else
#error "Invalid SPI5 SLAVE SELECT LINE 2 Pin Configuration!"
#endif
//</e>
// </h>

//   <o> SPI5_CLK INPUT Pin <0=>P3_6 <1=>P4_2
#if    (RTE_SPI5_CLK_INPUT_ID == 0)
#define RTE_SPI5_CLK_INPUT_PORT              P3_6
#define RTE_SPI5_CLK_INPUT                   USIC2_C1_DX1_P3_6
#elif  (RTE_SPI5_CLK_INPUT_ID == 1)
#define RTE_SPI5_CLK_INPUT_PORT              P4_2
#define RTE_SPI5_CLK_INPUT                   USIC2_C1_DX1_P4_2
#else
#error "Invalid SPI5 CLOCK INPUT Pin Configuration!"
#endif

//   <o> RTE_SPI5_SLAVE_SELECT INPUT Pin <0=>P4_1
#if    (RTE_SPI5_SLAVE_SELECT_INPUT_ID == 0)
#define RTE_SPI5_SLAVE_SELECT_INPUT_PORT              P4_1
#define RTE_SPI5_SLAVE_SELECT_INPUT                   USIC2_C1_DX2_P4_1
#else
#error "Invalid SPI5 SLAVE SELECT INPUT Pin Configuration!"
#endif

//   <o> SPI5_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI5_RX_FIFO_SIZE_ID == 0)
#define RTE_SPI5_RX_FIFO_SIZE         NO_FIFO
#define RTE_SPI5_RX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI5_RX_FIFO_SIZE_ID == 1)
#define RTE_SPI5_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI5_RX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI5_RX_FIFO_SIZE_ID == 2)
#define RTE_SPI5_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI5_RX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI5_RX_FIFO_SIZE_ID == 3)
#define RTE_SPI5_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI5_RX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI5_RX_FIFO_SIZE_ID == 4)
#define RTE_SPI5_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI5_RX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI5_RX_FIFO_SIZE_ID == 5)
#define RTE_SPI5_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI5_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI5_RX FIFO SIZE Configuration!"
#endif

//   <o> SPI5_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#if    (RTE_SPI5_TX_FIFO_SIZE_ID == 0)
#define RTE_SPI5_TX_FIFO_SIZE         NO_FIFO
#define RTE_SPI5_TX_FIFO_SIZE_NUM     0
#elif  (RTE_SPI5_TX_FIFO_SIZE_ID == 1)
#define RTE_SPI5_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_SPI5_TX_FIFO_SIZE_NUM     2
#elif  (RTE_SPI5_TX_FIFO_SIZE_ID == 2)
#define RTE_SPI5_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_SPI5_TX_FIFO_SIZE_NUM     4
#elif  (RTE_SPI5_TX_FIFO_SIZE_ID == 3)
#define RTE_SPI5_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_SPI5_TX_FIFO_SIZE_NUM     8
#elif  (RTE_SPI5_TX_FIFO_SIZE_ID == 4)
#define RTE_SPI5_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_SPI5_TX_FIFO_SIZE_NUM     16
#elif  (RTE_SPI5_TX_FIFO_SIZE_ID == 5)
#define RTE_SPI5_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_SPI5_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid SPI5_TX FIFO SIZE Configuration!"
#endif
#endif // RTE_Drivers_SPI

#ifdef RTE_Drivers_I2C
//</e>
// <e> I2C0 (Inter-Integrated circuit) [Driver_I2C0]
// <i> Configuration settings for Driver_I2C0 in component ::Drivers:I2C
#define RTE_I2C0                      0

//   <o> I2C0_TX Pin <0=>P1_5 <1=>P1_7 <2=>P5_1
#define RTE_I2C0_TX_ID                0
#if    (RTE_I2C0_TX_ID == 0)
#define RTE_I2C0_TX_PORT              P1_5
#define RTE_I2C0_TX_AF                P1_5_AF_U0C0_DOUT0
#elif  (RTE_I2C0_TX_ID == 1)
#define RTE_I2C0_TX_PORT              P1_7
#define RTE_I2C0_TX_AF                P1_7_AF_U0C0_DOUT0
#elif  (RTE_I2C0_TX_ID == 2)
#define RTE_I2C0_TX_PORT              P5_1
#define RTE_I2C0_TX_AF                P5_1_AF_U0C0_DOUT0
#else
#error "Invalid I2C0_TX Pin Configuration!"
#endif

//   <o> I2C0_RX Pin <0=>P1_4 <1=>P1_5 <2=>P4_7 <3=>P5_0
#define RTE_I2C0_RX_ID                1
#if    (RTE_I2C0_RX_ID == 0)
#define RTE_I2C0_RX_PORT              P1_4
#define RTE_I2C0_RX_INPUT             USIC0_C0_DX0_P1_4
#elif  (RTE_I2C0_RX_ID == 1)
#define RTE_I2C0_RX_PORT              P1_5
#define RTE_I2C0_RX_INPUT             USIC0_C0_DX0_P1_5
#elif  (RTE_I2C0_RX_ID == 2)
#define RTE_I2C0_RX_PORT              P4_7
#define RTE_I2C0_RX_INPUT             USIC0_C0_DX0_P4_7
#elif  (RTE_I2C0_RX_ID == 3)
#define RTE_I2C0_RX_PORT              P5_0
#define RTE_I2C0_RX_INPUT             USIC0_C0_DX0_P5_0
#else
#error "Invalid I2C0_RX Pin Configuration!"
#endif
//   <o> I2C0_CLK OUTPUT Pin <0=>P0_8 <1=>P1_1 <2=>P1_6 <3=>P1_9 <4=>P1_10
#define RTE_I2C0_CLK_OUTPUT_ID                0
#if    (RTE_I2C0_CLK_OUTPUT_ID == 0)
#define RTE_I2C0_CLK_OUTPUT_PORT              P0_8
#define RTE_I2C0_CLK_AF                       P0_8_AF_U0C0_SCLKOUT 
#elif  (RTE_I2C0_CLK_OUTPUT_ID == 1)
#define RTE_I2C0_CLK_OUTPUT_PORT              P1_1
#define RTE_I2C0_CLK_AF                       P1_1_AF_U0C0_SCLKOUT
#elif  (RTE_I2C0_CLK_OUTPUT_ID == 2)
#define RTE_I2C0_CLK_OUTPUT_PORT              P1_6
#define RTE_I2C0_CLK_AF                       P1_6_AF_U0C0_SCLKOUT
#elif  (RTE_I2C0_CLK_OUTPUT_ID == 3)
#define RTE_I2C0_CLK_OUTPUT_PORT              P1_9
#define RTE_I2C0_CLK_AF                       P1_9_AF_U0C0_SCLKOUT
#elif  (RTE_I2C0_CLK_OUTPUT_ID == 4)
#define RTE_I2C0_CLK_OUTPUT_PORT              P1_10
#define RTE_I2C0_CLK_AF                       P1_10_AF_U0C0_SCLKOUT
#else
#error "Invalid I2C0 CLOCK OUTPUT Pin Configuration!"
#endif
//   <o> I2C0_CLK INPUT Pin <0=>P0_8 <1=>P1_1
#define RTE_I2C0_CLK_INPUT_ID                0
#if    (RTE_I2C0_CLK_INPUT_ID == 0)
#define RTE_I2C0_CLK_INPUT_PORT              P0_8
#define RTE_I2C0_CLK_INPUT                   USIC0_C0_DX1_P0_8
#elif  (RTE_I2C0_CLK_INPUT_ID == 1)
#define RTE_I2C0_CLK_INPUT_PORT              P1_1
#define RTE_I2C0_CLK_INPUT                   USIC0_C0_DX1_P1_1
#else
#error "Invalid I2C0 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2C0_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C0_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C0_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C0_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C0_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C0_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C0_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C0_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C0_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C0_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C0_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C0_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C0_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C0_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C0_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C0_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C0_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C0_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C0_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C0_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C0_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C0_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C0_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C0_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C0_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C0_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C0_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C0_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C0_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C0_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C0_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C0_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C0_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C0_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C0_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C0_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C0_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C0_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C0_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C0_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C0_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C0_TX FIFO SIZE Configuration!"
#endif

//</e>

// <e> I2C1 (Inter-Integrated circuit) [Driver_I2C1]
// <i> Configuration settings for Driver_I2C1 in component ::Drivers:I2C
#define RTE_I2C1                      0

//   <o> I2C1_TX Pin <0=>P2_5 <1=>P3_5 <2=>P3_13 <3=>P6_4
#define RTE_I2C1_TX_ID                0
#if    (RTE_I2C1_TX_ID == 0)
#define RTE_I2C1_TX_PORT              P2_5
#define RTE_I2C1_TX_AF                P2_5_AF_U0C1_DOUT0
#elif  (RTE_I2C1_TX_ID == 1)
#define RTE_I2C1_TX_PORT              P3_5
#define RTE_I2C1_TX_AF                P3_5_AF_U0C1_DOUT0
#elif  (RTE_I2C1_TX_ID == 2)
#define RTE_I2C1_TX_PORT              P3_13
#define RTE_I2C1_TX_AF                P3_13_AF_U0C1_DOUT0
#elif  (RTE_I2C1_TX_ID == 3)
#define RTE_I2C1_TX_PORT              P6_4
#define RTE_I2C1_TX_AF                P6_4_AF_U0C1_DOUT0
#else
#error "Invalid I2C1_TX Pin Configuration!"
#endif

//   <o> I2C1_RX Pin <0=>P2_2 <1=>P2_5 <2=>P3_13 <3=>P4_0 <4=>P6_3
#define RTE_I2C1_RX_ID                1
#if    (RTE_I2C1_RX_ID == 0)
#define RTE_I2C1_RX_PORT              P2_2
#define RTE_I2C1_RX_INPUT             USIC0_C1_DX0_P2_2
#elif  (RTE_I2C1_RX_ID == 1)
#define RTE_I2C1_RX_PORT              P2_5
#define RTE_I2C1_RX_INPUT             USIC0_C1_DX0_P2_5
#elif  (RTE_I2C1_RX_ID == 2)
#define RTE_I2C1_RX_PORT              P3_13
#define RTE_I2C1_RX_INPUT             USIC0_C1_DX0_P3_13
#elif  (RTE_I2C1_RX_ID == 3)
#define RTE_I2C1_RX_PORT              P4_0
#define RTE_I2C1_RX_INPUT             USIC0_C1_DX0_P4_0
#elif  (RTE_I2C1_RX_ID == 4)
#define RTE_I2C1_RX_PORT              P6_3
#define RTE_I2C1_RX_INPUT             USIC0_C1_DX0_P6_3
#else
#error "Invalid I2C1_RX Pin Configuration!"
#endif

//   <o> I2C1_CLK OUTPUT Pin <0=>P2_4 <1=>P3_0 <2=>P3_6 <3=>P6_2
#define RTE_I2C1_CLK_OUTPUT_ID                0
#if    (RTE_I2C1_CLK_OUTPUT_ID == 0)
#define RTE_I2C1_CLK_OUTPUT_PORT              P2_4
#define RTE_I2C1_CLK_AF                       P2_4_AF_U0C1_SCLKOUT
#elif  (RTE_I2C1_CLK_OUTPUT_ID == 1)
#define RTE_I2C1_CLK_OUTPUT_PORT              P3_0
#define RTE_I2C1_CLK_AF                       P3_0_AF_U0C1_SCLKOUT
#elif  (RTE_I2C1_CLK_OUTPUT_ID == 2)
#define RTE_I2C1_CLK_OUTPUT_PORT              P3_6
#define RTE_I2C1_CLK_AF                       P3_6_AF_U0C1_SCLKOUT
#elif  (RTE_I2C1_CLK_OUTPUT_ID == 3)
#define RTE_I2C1_CLK_OUTPUT_PORT              P6_2
#define RTE_I2C1_CLK_AF                       P6_2_AF_U0C1_SCLKOUT
#else
#error "Invalid I2C1 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2C1_CLK INPUT Pin <0=>P2_4 <1=>P3_0 <2=>P6_2 
#define RTE_I2C1_CLK_INPUT_ID                0
#if    (RTE_I2C1_CLK_INPUT_ID == 0)
#define RTE_I2C1_CLK_INPUT_PORT              P2_4
#define RTE_I2C1_CLK_INPUT                   USIC0_C1_DX1_P2_4
#elif  (RTE_I2C1_CLK_INPUT_ID == 1)
#define RTE_I2C1_CLK_INPUT_PORT              P3_0
#define RTE_I2C1_CLK_INPUT                   USIC0_C1_DX1_P3_0
#elif  (RTE_I2C1_CLK_INPUT_ID == 2)
#define RTE_I2C1_CLK_INPUT_PORT              P6_2
#define RTE_I2C1_CLK_INPUT                   USIC0_C1_DX1_P6_2
#else
#error "Invalid I2C1 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2C1_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C1_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C1_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C1_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C1_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C1_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C1_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C1_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C1_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C1_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C1_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C1_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C1_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C1_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C1_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C1_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C1_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C1_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C1_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C1_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C1_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C1_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C1_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C1_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C1_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C1_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C1_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C1_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C1_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C1_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C1_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C1_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C1_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C1_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C1_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C1_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C1_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C1_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C1_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C1_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C1_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C1_TX FIFO SIZE Configuration!"
#endif
//</e>
// <e> I2C2 (Inter-Integrated circuit) [Driver_I2C2]
// <i> Configuration settings for Driver_I2C2 in component ::Drivers:I2C
#define RTE_I2C2                      0

//   <o> I2C2_TX Pin <0=>P0_5 <1=>P2_14
#define RTE_I2C2_TX_ID                0
#if    (RTE_I2C2_TX_ID == 0)
#define RTE_I2C2_TX_PORT              P0_5
#define RTE_I2C2_TX_AF                P0_5_AF_U1C0_DOUT0
#elif  (RTE_I2C2_TX_ID == 1)
#define RTE_I2C2_TX_PORT              P2_14
#define RTE_I2C2_TX_AF                P2_14_AF_U1C0_DOUT0
#else
#error "Invalid I2C2_TX Pin Configuration!"
#endif

//   <o> I2C2_RX Pin <0=>P0_4 <1=>P0_5 <2=>P2_14 <3=>P2_15
#define RTE_I2C2_RX_ID                1
#if    (RTE_I2C2_RX_ID == 0)
#define RTE_I2C2_RX_PORT              P0_4
#define RTE_I2C2_RX_INPUT             USIC1_C0_DX0_P0_4
#elif  (RTE_I2C2_RX_ID == 1)
#define RTE_I2C2_RX_PORT              P0_5
#define RTE_I2C2_RX_INPUT             USIC1_C0_DX0_P0_5
#elif  (RTE_I2C2_RX_ID == 2)
#define RTE_I2C2_RX_PORT              P2_14
#define RTE_I2C2_RX_INPUT             USIC1_C0_DX0_P2_14
#elif  (RTE_I2C2_RX_ID == 3)
#define RTE_I2C2_RX_PORT              P2_15
#define RTE_I2C2_RX_INPUT             USIC1_C0_DX0_P2_15
#else
#error "Invalid I2C2_RX Pin Configuration!"
#endif

//   <o> I2C2_CLK OUTPUT Pin <0=>P0_11 <1=>P5_8
#define RTE_I2C2_CLK_OUTPUT_ID                0
#if    (RTE_I2C2_CLK_OUTPUT_ID == 0)
#define RTE_I2C2_CLK_OUTPUT_PORT              P0_11
#define RTE_I2C2_CLK_AF                       P0_11_AF_U1C0_SCLKOUT
#elif  (RTE_I2C2_CLK_OUTPUT_ID == 1)
#define RTE_I2C2_CLK_OUTPUT_PORT              P5_8
#define RTE_I2C2_CLK_AF                       P5_8_AF_U1C0_SCLKOUT
#else
#error "Invalid I2C2 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2C2_CLK INPUT Pin <0=>P0_11 <1=>P5_8
#define RTE_I2C2_CLK_INPUT_ID                0
#if    (RTE_I2C2_CLK_INPUT_ID == 0)
#define RTE_I2C2_CLK_INPUT_PORT              P0_11
#define RTE_I2C2_CLK_INPUT                   USIC1_C0_DX1_P0_11
#elif  (RTE_I2C2_CLK_INPUT_ID == 1)
#define RTE_I2C2_CLK_INPUT_PORT              P5_8
#define RTE_I2C2_CLK_INPUT                   USIC1_C0_DX1_P5_8
#else
#error "Invalid I2C2 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2C2_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C2_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C2_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C2_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C2_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C2_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C2_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C2_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C2_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C2_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C2_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C2_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C2_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C2_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C2_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C2_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C2_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C2_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C2_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C2_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C2_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C2_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C2_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C2_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C2_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C2_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C2_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C2_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C2_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C2_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C2_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C2_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C2_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C2_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C2_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C2_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C2_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C2_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C2_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C2_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C2_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C2_TX FIFO SIZE Configuration!"
#endif

//</e>
// <e> I2C3 (Inter-Integrated circuit) [Driver_I2C3]
// <i> Configuration settings for Driver_I2C3 in component ::Drivers:I2C
#define RTE_I2C3                      0

//   <o> I2C3_TX Pin <0=>P0_1 <1=>P1_9 <2=>P3_15 <3=>P4_2
#define RTE_I2C3_TX_ID                2
#if    (RTE_I2C3_TX_ID == 0)
#define RTE_I2C3_TX_PORT              P0_1
#define RTE_I2C3_TX_AF                P0_1_AF_U1C1_DOUT0
#elif  (RTE_I2C3_TX_ID == 1)
#define RTE_I2C3_TX_PORT              P1_9
#define RTE_I2C3_TX_AF                P1_9_AF_U1C1_DOUT0
#elif  (RTE_I2C3_TX_ID == 2)
#define RTE_I2C3_TX_PORT              P3_15
#define RTE_I2C3_TX_AF                P3_15_AF_U1C1_DOUT0
#elif  (RTE_I2C3_TX_ID == 3)
#define RTE_I2C3_TX_PORT              P4_2
#define RTE_I2C3_TX_AF                P4_2_AF_U1C1_DOUT0
#else
#error "Invalid I2C3_TX Pin Configuration!"
#endif

//   <o> I2C3_RX Pin <0=>P0_0 <1=>P3_14 <2=>P3_15 <3=>P4_2
#define RTE_I2C3_RX_ID                2
#if    (RTE_I2C3_RX_ID == 0)
#define RTE_I2C3_RX_PORT              P0_0
#define RTE_I2C3_RX_INPUT             USIC1_C1_DX0_P0_0
#elif  (RTE_I2C3_RX_ID == 1)
#define RTE_I2C3_RX_PORT              P3_14
#define RTE_I2C3_RX_INPUT             USIC1_C1_DX0_P3_14
#elif  (RTE_I2C3_RX_ID == 2)
#define RTE_I2C3_RX_PORT              P3_15
#define RTE_I2C3_RX_INPUT             USIC1_C1_DX0_P3_15
#elif  (RTE_I2C3_RX_ID == 3)
#define RTE_I2C3_RX_PORT              P4_2
#define RTE_I2C3_RX_INPUT             USIC1_C1_DX0_P4_2
#else
#error "Invalid I2C3_RX Pin Configuration!"
#endif


//   <o> I2C3_CLK OUTPUT Pin <0=>P0_10 <1=>P0_13
#define RTE_I2C3_CLK_OUTPUT_ID                0
#if    (RTE_I2C3_CLK_OUTPUT_ID == 0)
#define RTE_I2C3_CLK_OUTPUT_PORT              P0_10
#define RTE_I2C3_CLK_AF                       P0_10_AF_U1C1_SCLKOUT
#elif  (RTE_I2C3_CLK_OUTPUT_ID == 1)
#define RTE_I2C3_CLK_OUTPUT_PORT              P0_13
#define RTE_I2C3_CLK_AF                       P0_13_AF_U1C1_SCLKOUT
#else
#error "Invalid I2C3 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2C3_CLK INPUT Pin <0=>P0_10 <1=>P0_13 <2=>P4_0
#define RTE_I2C3_CLK_INPUT_ID                0
#if    (RTE_I2C3_CLK_INPUT_ID == 0)
#define RTE_I2C3_CLK_INPUT_PORT              P0_10
#define RTE_I2C3_CLK_INPUT                   USIC1_C1_DX1_P0_10
#elif  (RTE_I2C3_CLK_INPUT_ID == 1)
#define RTE_I2C3_CLK_INPUT_PORT              P0_13
#define RTE_I2C3_CLK_INPUT                   USIC1_C1_DX1_P0_13
#elif  (RTE_I2C3_CLK_INPUT_ID == 2)
#define RTE_I2C3_CLK_INPUT_PORT              P4_0
#define RTE_I2C3_CLK_INPUT                   USIC1_C1_DX1_P4_0
#else
#error "Invalid I2C3 CLOCK INPUT Pin Configuration!"
#endif


//   <o> I2C3_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C3_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C3_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C3_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C3_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C3_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C3_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C3_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C3_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C3_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C3_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C3_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C3_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C3_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C3_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C3_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C3_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C3_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C3_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C3_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C3_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C3_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C3_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C3_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C3_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C3_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C3_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C3_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C3_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C3_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C3_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C3_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C3_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C3_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C3_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C3_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C3_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C3_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C3_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C3_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C3_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C3_TX FIFO SIZE Configuration!"
#endif


//</e>
// <e> I2C4 (Inter-Integrated circuit) [Driver_I2C4]
// <i> Configuration settings for Driver_I2C4 in component ::Drivers:I2C
#define RTE_I2C4                      0

//   <o> I2C4_TX Pin <0=>P3_8 <1=>P5_0
#define RTE_I2C4_TX_ID                1
#if    (RTE_I2C4_TX_ID == 0)
#define RTE_I2C4_TX_PORT              P3_8
#define RTE_I2C4_TX_AF                P3_8_AF_U2C0_DOUT0
#elif  (RTE_I2C4_TX_ID == 1)
#define RTE_I2C4_TX_PORT              P5_0
#define RTE_I2C4_TX_AF                P5_0_AF_U2C0_DOUT0
#else
#error "Invalid I2C4_TX Pin Configuration!"
#endif

//   <o> I2C4_RX Pin <0=>P3_7 <1=>P5_0 <2=>P5_1
#define RTE_I2C4_RX_ID                1
#if    (RTE_I2C4_RX_ID == 0)
#define RTE_I2C4_RX_PORT              P3_7
#define RTE_I2C4_RX_INPUT             USIC2_C0_DX0_P3_7
#elif  (RTE_I2C4_RX_ID == 1)
#define RTE_I2C4_RX_PORT              P5_0
#define RTE_I2C4_RX_INPUT             USIC2_C0_DX0_P5_0
#elif  (RTE_I2C4_RX_ID == 2)
#define RTE_I2C4_RX_PORT              P5_1
#define RTE_I2C4_RX_INPUT             USIC2_C0_DX0_P5_1
#else
#error "Invalid I2C4_RX Pin Configuration!"
#endif
//   <o> I2C4_CLK OUTPUT Pin <0=>P3_9 <1=>P5_2
#define RTE_I2C4_CLK_OUTPUT_ID                1
#if    (RTE_I2C4_CLK_OUTPUT_ID == 0)
#define RTE_I2C4_CLK_OUTPUT_PORT              P3_9
#define RTE_I2C4_CLK_AF                       P3_9_AF_U2C0_SCLKOUT
#elif  (RTE_I2C4_CLK_OUTPUT_ID == 1)
#define RTE_I2C4_CLK_OUTPUT_PORT              P5_2
#define RTE_I2C4_CLK_AF                       P5_2_AF_U2C0_SCLKOUT
#else
#error "Invalid I2C4 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2C4_CLK INPUT Pin <0=>P5_2
#define RTE_I2C4_CLK_INPUT_ID                0
#if    (RTE_I2C4_CLK_INPUT_ID == 0)
#define RTE_I2C4_CLK_INPUT_PORT              P5_2
#define RTE_I2C4_CLK_INPUT                   USIC2_C0_DX1_P5_2
#else
#error "Invalid I2C4 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2C4_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C4_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C4_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C4_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C4_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C4_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C4_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C4_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C4_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C4_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C4_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C4_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C4_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C4_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C4_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C4_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C4_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C4_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C4_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C4_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C4_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C4_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C4_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C4_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C4_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C4_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C4_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C4_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C4_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C4_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C4_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C4_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C4_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C4_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C4_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C4_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C4_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C4_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C4_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C4_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C4_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C4_TX FIFO SIZE Configuration!"
#endif

//</e>
// <e> I2C5 (Inter-Integrated circuit) [Driver_I2C5]
// <i> Configuration settings for Driver_I2C5 in component ::Drivers:I2C
#define RTE_I2C5                      0

//   <o> I2C5_TX Pin <0=>P3_5 <1=>P3_11 <2=>P4_7
#define RTE_I2C5_TX_ID                0
#if    (RTE_I2C5_TX_ID == 0)
#define RTE_I2C5_TX_PORT              P3_5
#define RTE_I2C5_TX_AF                P3_5_AF_U2C1_DOUT0
#elif  (RTE_I2C5_TX_ID == 1)
#define RTE_I2C5_TX_PORT              P3_11
#define RTE_I2C5_TX_AF                P3_11_AF_U2C1_DOUT0
#elif  (RTE_I2C5_TX_ID == 2)
#define RTE_I2C5_TX_PORT              P4_7
#define RTE_I2C5_TX_AF                P4_7_AF_U2C1_DOUT0
#else
#error "Invalid I2C5_TX Pin Configuration!"
#endif

//   <o> I2C5_RX Pin <0=>P3_4 <1=>P3_5 <2=>P3_12 <3=>P4_0 <3=>P4_6
#define RTE_I2C5_RX_ID                1
#if    (RTE_I2C5_RX_ID == 0)
#define RTE_I2C5_RX_PORT              P3_4
#define RTE_I2C5_RX_INPUT             USIC2_C1_DX0_P3_4
#elif  (RTE_I2C5_RX_ID == 1)
#define RTE_I2C5_RX_PORT              P3_5
#define RTE_I2C5_RX_INPUT             USIC2_C1_DX0_P3_5
#elif  (RTE_I2C5_RX_ID == 2)
#define RTE_I2C5_RX_PORT              P3_12
#define RTE_I2C5_RX_INPUT             USIC2_C1_DX0_P3_12
#elif  (RTE_I2C5_RX_ID == 3)
#define RTE_I2C5_RX_PORT              P4_0
#define RTE_I2C5_RX_INPUT             USIC2_C1_DX0_P4_0
#elif  (RTE_I2C5_RX_ID == 4)
#define RTE_I2C5_RX_PORT              P4_6
#define RTE_I2C5_RX_INPUT             USIC2_C1_DX0_P4_6
#else
#error "Invalid I2C5_RX Pin Configuration!"
#endif

//   <o> I2C5_CLK OUTPUT Pin <0=>P3_6 <1=>P3_13 <2=>P4_2
#define RTE_I2C5_CLK_OUTPUT_ID                0
#if    (RTE_I2C5_CLK_OUTPUT_ID == 0)
#define RTE_I2C5_CLK_OUTPUT_PORT              P3_6
#define RTE_I2C5_CLK_AF                       P3_6_AF_U2C1_SCLKOUT
#elif  (RTE_I2C5_CLK_OUTPUT_ID == 1)
#define RTE_I2C5_CLK_OUTPUT_PORT              P3_13
#define RTE_I2C5_CLK_AF                       P3_13_AF_U2C1_SCLKOUT
#elif  (RTE_I2C5_CLK_OUTPUT_ID == 2)
#define RTE_I2C5_CLK_OUTPUT_PORT              P4_2
#define RTE_I2C5_CLK_AF                       P4_2_AF_U2C1_SCLKOUT
#else
#error "Invalid I2C5 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2C5_CLK INPUT Pin <0=>P3_6 <1=>P4_2
#define RTE_I2C5_CLK_INPUT_ID                0
#if    (RTE_I2C5_CLK_INPUT_ID == 0)
#define RTE_I2C5_CLK_INPUT_PORT              P3_6
#define RTE_I2C5_CLK_INPUT                   USIC2_C1_DX1_P3_6
#elif   (RTE_I2C5_CLK_INPUT_ID == 1)
#define RTE_I2C5_CLK_INPUT_PORT              P4_2
#define RTE_I2C5_CLK_INPUT                   USIC2_C1_DX1_P4_2
#else
#error "Invalid I2C5 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2C5_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C5_RX_FIFO_SIZE_ID                4
#if    (RTE_I2C5_RX_FIFO_SIZE_ID == 0)
#define RTE_I2C5_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2C5_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C5_RX_FIFO_SIZE_ID == 1)
#define RTE_I2C5_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C5_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C5_RX_FIFO_SIZE_ID == 2)
#define RTE_I2C5_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C5_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C5_RX_FIFO_SIZE_ID == 3)
#define RTE_I2C5_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C5_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C5_RX_FIFO_SIZE_ID == 4)
#define RTE_I2C5_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C5_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C5_RX_FIFO_SIZE_ID == 5)
#define RTE_I2C5_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C5_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C5_RX FIFO SIZE Configuration!"
#endif

//   <o> I2C5_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2C5_TX_FIFO_SIZE_ID                4
#if    (RTE_I2C5_TX_FIFO_SIZE_ID == 0)
#define RTE_I2C5_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2C5_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2C5_TX_FIFO_SIZE_ID == 1)
#define RTE_I2C5_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2C5_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2C5_TX_FIFO_SIZE_ID == 2)
#define RTE_I2C5_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2C5_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2C5_TX_FIFO_SIZE_ID == 3)
#define RTE_I2C5_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2C5_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2C5_TX_FIFO_SIZE_ID == 4)
#define RTE_I2C5_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2C5_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2C5_TX_FIFO_SIZE_ID == 5)
#define RTE_I2C5_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2C5_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2C5_TX FIFO SIZE Configuration!"
#endif
//</e>
#endif // RTE_Drivers_I2C

#ifdef RTE_Drivers_I2S
// <e> I2S0 (Inter-Integrated Sound) [Driver_SAI0]
// <i> Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define RTE_I2S0                      0

//   <o> I2S0_TX Pin <0=>P1_5 <1=>P1_7 <2=>P5_1
#define RTE_I2S0_TX_ID                0
#if    (RTE_I2S0_TX_ID == 0)
#define RTE_I2S0_TX_PORT              P1_5
#define RTE_I2S0_TX_AF                P1_5_AF_U0C0_DOUT0
#elif  (RTE_I2S0_TX_ID == 1)
#define RTE_I2S0_TX_PORT              P1_7
#define RTE_I2S0_TX_AF                P1_7_AF_U0C0_DOUT0
#elif  (RTE_I2S0_TX_ID == 2)
#define RTE_I2S0_TX_PORT              P5_1
#define RTE_I2S0_TX_AF                P5_1_AF_U0C0_DOUT0
#else
#error "Invalid I2S0_TX Pin Configuration!"
#endif

//   <o> I2S0_RX Pin <0=>P1_4 <1=>P1_5 <2=>P4_7 <3=>P5_0
#define RTE_I2S0_RX_ID                0
#if    (RTE_I2S0_RX_ID == 0)
#define RTE_I2S0_RX_PORT              P1_4
#define RTE_I2S0_RX_INPUT             USIC0_C0_DX0_P1_4
#elif  (RTE_I2S0_RX_ID == 1)
#define RTE_I2S0_RX_PORT              P1_5
#define RTE_I2S0_RX_INPUT             USIC0_C0_DX0_P1_5
#elif  (RTE_I2S0_RX_ID == 2)
#define RTE_I2S0_RX_PORT              P4_7
#define RTE_I2S0_RX_INPUT             USIC0_C0_DX0_P4_7
#elif  (RTE_I2S0_RX_ID == 3)
#define RTE_I2S0_RX_PORT              P5_0
#define RTE_I2S0_RX_INPUT             USIC0_C0_DX0_P5_0
#else
#error "Invalid I2S0_RX Pin Configuration!"
#endif

//   <o> I2S0_CLK OUTPUT Pin <0=>P0_8 <1=>P1_1 <2=>P1_6 <3=>P1_9 <4=>P1_10
#define RTE_I2S0_CLK_OUTPUT_ID                0
#if    (RTE_I2S0_CLK_OUTPUT_ID == 0)
#define RTE_I2S0_CLK_OUTPUT_PORT              P0_8
#define RTE_I2S0_CLK_AF                       P0_8_AF_U0C0_SCLKOUT
#elif  (RTE_I2S0_CLK_OUTPUT_ID == 1)
#define RTE_I2S0_CLK_OUTPUT_PORT              P1_1
#define RTE_I2S0_CLK_AF                       P1_1_AF_U0C0_SCLKOUT
#elif  (RTE_I2S0_CLK_OUTPUT_ID == 2)
#define RTE_I2S0_CLK_OUTPUT_PORT              P1_6
#define RTE_I2S0_CLK_AF                       P1_6_AF_U0C0_SCLKOUT
#elif  (RTE_I2S0_CLK_OUTPUT_ID == 3)
#define RTE_I2S0_CLK_OUTPUT_PORT              P1_9
#define RTE_I2S0_CLK_AF                       P1_9_AF_U0C0_SCLKOUT
#elif  (RTE_I2S0_CLK_OUTPUT_ID == 4)
#define RTE_I2S0_CLK_OUTPUT_PORT              P1_10
#define RTE_I2S0_CLK_AF                       P1_10_AF_U0C0_SCLKOUT
#else
#error "Invalid I2S0_CLK Output Pin Configuration!"
#endif

//   <o> I2S0_CLK INPUT Pin <0=>P0_8 <1=>P1_1
#define RTE_I2S0_CLK_INPUT_ID                0
#if    (RTE_I2S0_CLK_INPUT_ID == 0)
#define RTE_I2S0_CLK_INPUT_PORT              P0_8
#define RTE_I2S0_CLK_INPUT                   USIC0_C0_DX1_P0_8
#elif  (RTE_I2S0_CLK_INPUT_ID == 1)
#define RTE_I2S0_CLK_INPUT_PORT              P1_1
#define RTE_I2S0_CLK_INPUT                   USIC0_C0_DX1_P1_1
#else
#error "Invalid I2S0_CLK Input Pin Configuration!"
#endif

//   <o> I2S0_WS OUTPUT Pin <0=>P0_7 <1=>P1_0 <2=>P1_11
#define RTE_I2S0_WS_OUTPUT_ID                0
#if     (RTE_I2S0_WS_OUTPUT_ID == 0)
#define RTE_I2S0_WS_OUTPUT_PORT              P0_7
#define RTE_I2S0_WS_AF                       P0_7_AF_U0C0_SELO0
#elif   (RTE_I2S0_WS_OUTPUT_ID == 1)
#define RTE_I2S0_WS_OUTPUT_PORT              P1_0
#define RTE_I2S0_WS_AF                       P1_0_AF_U0C0_SELO0
#elif   (RTE_I2S0_WS_OUTPUT_ID == 2)
#define RTE_I2S0_WS_OUTPUT_PORT              P1_11
#define RTE_I2S0_WS_AF                       P1_11_AF_U0C0_SELO0
#else
#error "Invalid I2S0_WS Output Pin Configuration!"
#endif

//   <o> I2S0_WS INPUT Pin <0=>P0_7 <1=>P1_0
#define RTE_I2S0_WS_INPUT_ID                 0
#if    (RTE_I2S0_WS_INPUT_ID == 0)
#define RTE_I2S0_WS_INPUT_PORT               P0_7
#define RTE_I2S0_WS_INPUT_ID_INPUT           USIC0_C0_DX2_P0_7
#elif  (RTE_I2S0_WS_INPUT_ID == 1)
#define RTE_I2S0_WS_INPUT_PORT               P1_0
#define RTE_I2S0_WS_INPUT_ID_INPUT           USIC0_C0_DX2_P1_0
#else
#error "Invalid I2S0_WS Input Pin Configuration!"
#endif

// <e> Enable MCLK output
#define RTE_I2S0_MCLK_OUTPUT_ENABLE          0
//   <o> I2S0_MCLK OUTPUT Pin <0=>P1_3
#define RTE_I2S0_MCLK_OUTPUT_ID              0
#if    (RTE_I2S0_MCLK_OUTPUT_ID == 0)
#define RTE_I2S0_MCLK_OUTPUT_PORT            P1_3
#define RTE_I2S0_MCLK_AF                     P1_3_AF_U0C0_MCLKOUT
#else
#error "Invalid I2S0_MCLK Output Pin Configuration!"
#endif
// </e>

//   <o> I2S0_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S0_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S0_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S0_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S0_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S0_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S0_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S0_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S0_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S0_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S0_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S0_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S0_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S0_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S0_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S0_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S0_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S0_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S0_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S0_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S0_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S0_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S0_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S0_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S0_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S0_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S0_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S0_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S0_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S0_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S0_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S0_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S0_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S0_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S0_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S0_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S0_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S0_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S0_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S0_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S0_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S0_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> I2S1 (Inter-Integrated Sound) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::Drivers:SAI
#define RTE_I2S1                      0

//   <o> I2S1_TX Pin <0=>P2_5 <1=>P3_5 <2=>P3_13 <3=>P6_4
#define RTE_I2S1_TX_ID                0
#if    (RTE_I2S1_TX_ID == 0)
#define RTE_I2S1_TX_PORT              P2_5
#define RTE_I2S1_TX_AF                P2_5_AF_U0C1_DOUT0
#elif  (RTE_I2S1_TX_ID == 1)
#define RTE_I2S1_TX_PORT              P3_5
#define RTE_I2S1_TX_AF                P3_5_AF_U0C1_DOUT0
#elif  (RTE_I2S1_TX_ID == 2)
#define RTE_I2S1_TX_PORT              P3_13
#define RTE_I2S1_TX_AF                P3_13_AF_U0C1_DOUT0
#elif  (RTE_I2S1_TX_ID == 3)
#define RTE_I2S1_TX_PORT              P6_4
#define RTE_I2S1_TX_AF                P6_4_AF_U0C1_DOUT0
#else
#error "Invalid I2S1_TX Pin Configuration!"
#endif

//   <o> I2S1_RX Pin <0=>P2_2 <1=>P2_5 <2=>P3_13 <3=>P4_0 <4=>P6_3
#define RTE_I2S1_RX_ID                0
#if    (RTE_I2S1_RX_ID == 0)
#define RTE_I2S1_RX_PORT              P2_2
#define RTE_I2S1_RX_INPUT             USIC0_C1_DX0_P2_2
#elif  (RTE_I2S1_RX_ID == 1)
#define RTE_I2S1_RX_PORT              P2_5
#define RTE_I2S1_RX_INPUT             USIC0_C1_DX0_P2_5
#elif  (RTE_I2S1_RX_ID == 2)
#define RTE_I2S1_RX_PORT              P3_13
#define RTE_I2S1_RX_INPUT             USIC0_C1_DX0_P3_13
#elif  (RTE_I2S1_RX_ID == 3)
#define RTE_I2S1_RX_PORT              P4_0
#define RTE_I2S1_RX_INPUT             USIC0_C1_DX0_P4_0
#elif  (RTE_I2S1_RX_ID == 4)
#define RTE_I2S1_RX_PORT              P6_3
#define RTE_I2S1_RX_INPUT             USIC0_C1_DX0_P6_3
#else
#error "Invalid I2S1_RX Pin Configuration!"
#endif

//   <o> I2S1_CLK OUTPUT Pin <0=>P2_4 <1=>P3_0 <2=>P3_6 <3=>P6_2
#define RTE_I2S1_CLK_OUTPUT_ID                0
#if    (RTE_I2S1_CLK_OUTPUT_ID == 0)
#define RTE_I2S1_CLK_OUTPUT_PORT              P2_4
#define RTE_I2S1_CLK_AF                       P2_4_AF_U0C1_SCLKOUT
#elif  (RTE_I2S1_CLK_OUTPUT_ID == 1)
#define RTE_I2S1_CLK_OUTPUT_PORT              P3_0
#define RTE_I2S1_CLK_AF                       P3_0_AF_U0C1_SCLKOUT
#elif  (RTE_I2S1_CLK_OUTPUT_ID == 2)
#define RTE_I2S1_CLK_OUTPUT_PORT              P3_6
#define RTE_I2S1_CLK_AF                       P3_6_AF_U0C1_SCLKOUT
#elif  (RTE_I2S1_CLK_OUTPUT_ID == 3)
#define RTE_I2S1_CLK_OUTPUT_PORT              P6_2
#define RTE_I2S1_CLK_AF                       P6_2_AF_U0C1_SCLKOUT
#else
#error "Invalid I2S1 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2S1_CLK INPUT Pin <0=>P2_4 <1=>P3_0 <2=>P6_2
#define RTE_I2S1_CLK_INPUT_ID                0
#if    (RTE_I2S1_CLK_INPUT_ID == 0)
#define RTE_I2S1_CLK_INPUT_PORT              P2_4
#define RTE_I2S1_CLK_INPUT                   USIC0_C1_DX1_P2_4
#elif  (RTE_I2S1_CLK_INPUT_ID == 1)
#define RTE_I2S1_CLK_INPUT_PORT              P3_0
#define RTE_I2S1_CLK_INPUT                   USIC0_C1_DX1_P3_0
#elif  (RTE_I2S1_CLK_INPUT_ID == 2)
#define RTE_I2S1_CLK_INPUT_PORT              P6_2
#define RTE_I2S1_CLK_INPUT                   USIC0_C1_DX1_P6_2
#else
#error "Invalid I2S1 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2S1_WS OUTPUT Pin <0=>P2_3 <1=>P3_1 <2=>P4_1 <3=>P6_1
#define RTE_I2S1_WS_OUTPUT_ID                0
#if     (RTE_I2S1_WS_OUTPUT_ID == 0)
#define RTE_I2S1_WS_OUTPUT_PORT              P2_3
#define RTE_I2S1_WS_AF                       P2_3_AF_U0C1_SELO0
#elif   (RTE_I2S1_WS_OUTPUT_ID == 1)
#define RTE_I2S1_WS_OUTPUT_PORT              P3_1
#define RTE_I2S1_WS_AF                       P3_1_AF_U0C1_SELO0
#elif   (RTE_I2S1_WS_OUTPUT_ID == 2)
#define RTE_I2S1_WS_OUTPUT_PORT              P4_1
#define RTE_I2S1_WS_AF                       P4_1_AF_U0C1_SELO0
#elif   (RTE_I2S1_WS_OUTPUT_ID == 3)
#define RTE_I2S1_WS_OUTPUT_PORT              P6_1
#define RTE_I2S1_WS_AF                       P6_1_AF_U0C1_SELO0
#else
#error "Invalid I2S1_WS Output Pin Configuration!"
#endif

//   <o> I2S1_WS INPUT Pin <0=>P2_3 <1=>P3_1 <2=>P6_1
#define RTE_I2S1_WS_INPUT_ID                 0
#if    (RTE_I2S1_WS_INPUT_ID == 0)
#define RTE_I2S1_WS_INPUT_PORT               P2_3
#define RTE_I2S1_WS_INPUT_ID_INPUT           USIC0_C1_DX2_P2_3
#elif  (RTE_I2S1_WS_INPUT_ID == 1)
#define RTE_I2S1_WS_INPUT_PORT               P3_1
#define RTE_I2S1_WS_INPUT_ID_INPUT           USIC0_C1_DX2_P3_1
#elif  (RTE_I2S1_WS_INPUT_ID == 2)
#define RTE_I2S1_WS_INPUT_PORT               P6_1
#define RTE_I2S1_WS_INPUT_ID_INPUT           USIC0_C1_DX2_P6_1
#else
#error "Invalid I2S1_WS Input Pin Configuration!"
#endif

// <e> Enable MCLK output
#define RTE_I2S1_MCLK_OUTPUT_ENABLE          0
//   <o> I2S1_MCLK OUTPUT Pin <0=>P6_5
#define RTE_I2S1_MCLK_OUTPUT_ID              0
#if    (RTE_I2S1_MCLK_OUTPUT_ID == 0)
#define RTE_I2S1_MCLK_OUTPUT_PORT            P6_5
#define RTE_I2S1_MCLK_AF                     P6_5_AF_U0C1_MCLKOUT
#else
#error "Invalid I2S1_MCLK Output Pin Configuration!"
#endif
// </e>

//   <o> I2S1_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S1_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S1_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S1_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S1_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S1_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S1_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S1_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S1_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S1_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S1_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S1_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S1_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S1_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S1_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S1_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S1_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S1_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S1_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S1_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S1_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S1_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S1_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S1_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S1_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S1_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S1_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S1_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S1_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S1_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S1_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S1_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S1_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S1_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S1_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S1_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S1_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S1_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S1_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S1_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S1_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S1_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> I2S2 (Inter-Integrated Sound) [Driver_SAI2]
// <i> Configuration settings for Driver_SAI2 in component ::Drivers:SAI
#define RTE_I2S2                      0

//   <o> I2S2_TX Pin <0=>P0_5 <1=>P1_15 <2=>P2_14
#define RTE_I2S2_TX_ID                0
#if    (RTE_I2S2_TX_ID == 0)
#define RTE_I2S2_TX_PORT              P0_5
#define RTE_I2S2_TX_AF                P0_5_AF_U1C0_DOUT0
#elif  (RTE_I2S2_TX_ID == 1)
#define RTE_I2S2_TX_PORT              P1_15
#define RTE_I2S2_TX_AF                P1_15_AF_U1C0_DOUT0
#elif  (RTE_I2S2_TX_ID == 2)
#define RTE_I2S2_TX_PORT              P2_14
#define RTE_I2S2_TX_AF                P2_14_AF_U1C0_DOUT0
#else
#error "Invalid I2S2_TX Pin Configuration!"
#endif

//   <o> I2S2_RX Pin <0=>P0_4 <1=>P0_5 <2=>P1_14 <3=>P2_14 <4=>P2_15
#define RTE_I2S2_RX_ID                0
#if    (RTE_I2S2_RX_ID == 0)
#define RTE_I2S2_RX_PORT              P0_4
#define RTE_I2S2_RX_INPUT             USIC1_C0_DX0_P0_4
#elif  (RTE_I2S2_RX_ID == 1)
#define RTE_I2S2_RX_PORT              P0_5
#define RTE_I2S2_RX_INPUT             USIC1_C0_DX0_P0_5
#elif  (RTE_I2S2_RX_ID == 2)
#define RTE_I2S2_RX_PORT              P1_14
#define RTE_I2S2_RX_INPUT             USIC1_C0_DX0_P1_14
#elif  (RTE_I2S2_RX_ID == 3)
#define RTE_I2S2_RX_PORT              P2_14
#define RTE_I2S2_RX_INPUT             USIC1_C0_DX0_P2_14
#elif  (RTE_I2S2_RX_ID == 4)
#define RTE_I2S2_RX_PORT              P2_15
#define RTE_I2S2_RX_INPUT             USIC1_C0_DX0_P2_15
#else
#error "Invalid I2S2_RX Pin Configuration!"
#endif

//   <o> I2S2_CLK OUTPUT Pin <0=>P0_11 <1=>P4_0 <2=>P5_8
#define RTE_I2S2_CLK_OUTPUT_ID                0
#if    (RTE_I2S2_CLK_OUTPUT_ID == 0)
#define RTE_I2S2_CLK_OUTPUT_PORT              P0_11
#define RTE_I2S2_CLK_AF                       P0_11_AF_U1C0_SCLKOUT
#elif  (RTE_I2S2_CLK_OUTPUT_ID == 1)
#define RTE_I2S2_CLK_OUTPUT_PORT              P4_0
#define RTE_I2S2_CLK_AF                       P4_0_AF_U1C0_SCLKOUT
#elif  (RTE_I2S2_CLK_OUTPUT_ID == 2)
#define RTE_I2S2_CLK_OUTPUT_PORT              P5_8
#define RTE_I2S2_CLK_AF                       P5_8_AF_U1C0_SCLKOUT
#else
#error "Invalid I2S2 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2S2_CLK INPUT Pin <0=>P0_11 <1=>P5_8
#define RTE_I2S2_CLK_INPUT_ID                0
#if    (RTE_I2S2_CLK_INPUT_ID == 0)
#define RTE_I2S2_CLK_INPUT_PORT              P0_11
#define RTE_I2S2_CLK_INPUT                   USIC1_C0_DX1_P0_11
#elif  (RTE_I2S2_CLK_INPUT_ID == 1)
#define RTE_I2S2_CLK_INPUT_PORT              P5_8
#define RTE_I2S2_CLK_INPUT                   USIC1_C0_DX1_P5_8
#else
#error "Invalid I2S2 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2S2_WS OUTPUT Pin <0=>P0_6 <1=>P5_9
#define RTE_I2S2_WS_OUTPUT_ID                0
#if     (RTE_I2S2_WS_OUTPUT_ID == 0)
#define RTE_I2S2_WS_OUTPUT_PORT              P0_6
#define RTE_I2S2_WS_AF                       P0_6_AF_U1C0_SELO0
#elif   (RTE_I2S2_WS_OUTPUT_ID == 1)
#define RTE_I2S2_WS_OUTPUT_PORT              P5_9
#define RTE_I2S2_WS_AF                       P5_9_AF_U1C0_SELO0
#else
#error "Invalid I2S2_WS Output Pin Configuration!"
#endif

//   <o> I2S2_WS INPUT Pin <0=>P0_6 <1=>P5_9
#define RTE_I2S2_WS_INPUT_ID                 0
#if    (RTE_I2S2_WS_INPUT_ID == 0)
#define RTE_I2S2_WS_INPUT_PORT               P0_6
#define RTE_I2S2_WS_INPUT_ID_INPUT           USIC1_C0_DX2_P0_6
#elif  (RTE_I2S2_WS_INPUT_ID == 1)
#define RTE_I2S2_WS_INPUT_PORT               P5_9
#define RTE_I2S2_WS_INPUT_ID_INPUT           USIC1_C0_DX2_P5_9
#else
#error "Invalid I2S2_WS Input Pin Configuration!"
#endif

// <e> Enable MCLK output
#define RTE_I2S2_MCLK_OUTPUT_ENABLE          0
//   <o> I2S2_MCLK OUTPUT Pin <0=>P5_10
#define RTE_I2S2_MCLK_OUTPUT_ID              0
#if     (RTE_I2S2_MCLK_OUTPUT_ID == 0)
#define RTE_I2S2_MCLK_OUTPUT_PORT            P5_10
#define RTE_I2S2_MCLK_AF                     P5_10_AF_U1C0_MCLKOUT
#else
#error "Invalid I2S2_MCLK Output Pin Configuration!"
#endif
// </e>

//   <o> I2S2_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S2_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S2_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S2_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S2_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S2_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S2_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S2_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S2_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S2_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S2_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S2_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S2_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S2_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S2_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S2_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S2_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S2_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S2_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S2_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S2_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S2_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S2_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S2_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S2_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S2_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S2_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S2_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S2_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S2_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S2_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S2_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S2_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S2_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S2_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S2_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S2_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S2_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S2_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S2_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S2_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S2_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> I2S3 (Inter-Integrated Sound) [Driver_SAI3]
// <i> Configuration settings for Driver_SAI3 in component ::Drivers:SAI
#define RTE_I2S3                      0

//   <o> I2S3_TX Pin <0=>P0_1 <1=>P1_9 <2=>P3_15 <3=>P4_2
#define RTE_I2S3_TX_ID                0
#if    (RTE_I2S3_TX_ID == 0)
#define RTE_I2S3_TX_PORT              P0_1
#define RTE_I2S3_TX_AF                P0_1_AF_U1C1_DOUT0
#elif  (RTE_I2S3_TX_ID == 1)
#define RTE_I2S3_TX_PORT              P1_9
#define RTE_I2S3_TX_AF                P1_9_AF_U1C1_DOUT0
#elif  (RTE_I2S3_TX_ID == 2)
#define RTE_I2S3_TX_PORT              P3_15
#define RTE_I2S3_TX_AF                P3_15_AF_U1C1_DOUT0
#elif  (RTE_I2S3_TX_ID == 3)
#define RTE_I2S3_TX_PORT              P4_2
#define RTE_I2S3_TX_AF                P4_2_AF_U1C1_DOUT0
#else
#error "Invalid I2S3_TX Pin Configuration!"
#endif

//   <o> I2S3_RX Pin <0=>P0_0 <1=>P3_14 <2=>P3_15 <3=>P4_2 
#define RTE_I2S3_RX_ID                0
#if    (RTE_I2S3_RX_ID == 0)
#define RTE_I2S3_RX_PORT              P0_0
#define RTE_I2S3_RX_INPUT             USIC1_C1_DX0_P0_0
#elif  (RTE_I2S3_RX_ID == 1)
#define RTE_I2S3_RX_PORT              P3_14
#define RTE_I2S3_RX_INPUT             USIC1_C1_DX0_P3_14
#elif  (RTE_I2S3_RX_ID == 2)
#define RTE_I2S3_RX_PORT              P3_15
#define RTE_I2S3_RX_INPUT             USIC1_C1_DX0_P3_15
#elif  (RTE_I2S3_RX_ID == 3)
#define RTE_I2S3_RX_PORT              P4_2
#define RTE_I2S3_RX_INPUT             USIC1_C1_DX0_P4_2
#else
#error "Invalid I2S3_RX Pin Configuration!"
#endif

//   <o> I2S3_CLK OUTPUT Pin <0=>P0_10 <1=>P0_13 <2=>P1_8
#define RTE_I2S3_CLK_OUTPUT_ID                0
#if    (RTE_I2S3_CLK_OUTPUT_ID == 0)
#define RTE_I2S3_CLK_OUTPUT_PORT              P0_10
#define RTE_I2S3_CLK_AF                       P0_10_AF_U1C1_SCLKOUT
#elif  (RTE_I2S3_CLK_OUTPUT_ID == 1)
#define RTE_I2S3_CLK_OUTPUT_PORT              P0_13
#define RTE_I2S3_CLK_AF                       P0_13_AF_U1C1_SCLKOUT
#elif  (RTE_I2S3_CLK_OUTPUT_ID == 2)
#define RTE_I2S3_CLK_OUTPUT_PORT              P1_8
#define RTE_I2S3_CLK_AF                       P1_8_AF_U1C1_SCLKOUT
#else
#error "Invalid I2S3 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2S3_CLK INPUT Pin <0=>P0_10 <1=>P0_13 <2=>P4_0
#define RTE_I2S3_CLK_INPUT_ID                0
#if    (RTE_I2S3_CLK_INPUT_ID == 0)
#define RTE_I2S3_CLK_INPUT_PORT              P0_10
#define RTE_I2S3_CLK_INPUT                   USIC1_C1_DX1_P0_10
#elif  (RTE_I2S3_CLK_INPUT_ID == 1)
#define RTE_I2S3_CLK_INPUT_PORT              P0_13
#define RTE_I2S3_CLK_INPUT                   USIC1_C1_DX1_P0_13
#elif  (RTE_I2S3_CLK_INPUT_ID == 2)
#define RTE_I2S3_CLK_INPUT_PORT              P4_0
#define RTE_I2S3_CLK_INPUT                   USIC1_C1_DX1_P4_0
#else
#error "Invalid I2S3 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2S3_WS OUTPUT Pin <0=>P0_9 <1=>P0_12
#define RTE_I2S3_WS_OUTPUT_ID                0
#if     (RTE_I2S3_WS_OUTPUT_ID == 0)
#define RTE_I2S3_WS_OUTPUT_PORT              P0_9
#define RTE_I2S3_WS_AF                       P0_9_AF_U1C1_SELO0
#elif   (RTE_I2S3_WS_OUTPUT_ID == 1)
#define RTE_I2S3_WS_OUTPUT_PORT              P0_12
#define RTE_I2S3_WS_AF                       P0_12_AF_U1C1_SELO0
#else
#error "Invalid I2S3_WS Output Pin Configuration!"
#endif

//   <o> I2S3_WS INPUT Pin <0=>P0_9 <1=>P0_12
#define RTE_I2S3_WS_INPUT_ID                 0
#if    (RTE_I2S3_WS_INPUT_ID == 0)
#define RTE_I2S3_WS_INPUT_PORT               P0_9
#define RTE_I2S3_WS_INPUT_ID_INPUT           USIC1_C1_DX2_P0_9
#elif  (RTE_I2S3_WS_INPUT_ID == 1)
#define RTE_I2S3_WS_INPUT_PORT               P0_12
#define RTE_I2S3_WS_INPUT_ID_INPUT           USIC1_C1_DX2_P0_12
#else
#error "Invalid I2S3_WS Input Pin Configuration!"
#endif

// <e> Enable MCLK output
#define RTE_I2S3_MCLK_OUTPUT_ENABLE          0
//   <o> I2S3_MCLK OUTPUT Pin <0=>P4_1
#define RTE_I2S3_MCLK_OUTPUT_ID              0
#if    (RTE_I2S3_MCLK_OUTPUT_ID == 0)
#define RTE_I2S3_MCLK_OUTPUT_PORT            P4_1
#define RTE_I2S3_MCLK_AF                     P4_1_AF_U1C1_MCLKOUT
#else
#error "Invalid I2S3_MCLK Output Pin Configuration!"
#endif
// </e>

//   <o> I2S3_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S3_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S3_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S3_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S3_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S3_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S3_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S3_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S3_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S3_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S3_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S3_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S3_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S3_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S3_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S3_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S3_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S3_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S3_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S3_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S3_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S3_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S3_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S3_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S3_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S3_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S3_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S3_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S3_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S3_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S3_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S3_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S3_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S3_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S3_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S3_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S3_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S3_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S3_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S3_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S3_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S3_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> I2S4 (Inter-Integrated Sound) [Driver_SAI4]
// <i> Configuration settings for Driver_SAI4 in component ::Drivers:SAI
#define RTE_I2S4                      0

//   <o> I2S4_TX Pin <0=>P3_8 <1=>P5_0 <2=>P6_6
#define RTE_I2S4_TX_ID                0
#if    (RTE_I2S4_TX_ID == 0)
#define RTE_I2S4_TX_PORT              P3_8
#define RTE_I2S4_TX_AF                P3_8_AF_U2C0_DOUT0
#elif  (RTE_I2S4_TX_ID == 1)
#define RTE_I2S4_TX_PORT              P5_0
#define RTE_I2S4_TX_AF                P5_0_AF_U2C0_DOUT0
#elif  (RTE_I2S4_TX_ID == 2)
#define RTE_I2S4_TX_PORT              P6_6
#define RTE_I2S4_TX_AF                P6_6_AF_U2C0_DOUT0
#else
#error "Invalid I2S4_TX Pin Configuration!"
#endif

//   <o> I2S4_RX Pin <0=>P3_7 <1=>P5_0 <2=>P5_1 <3=>P6_5
#define RTE_I2S4_RX_ID                0
#if    (RTE_I2S4_RX_ID == 0)
#define RTE_I2S4_RX_PORT              P3_7
#define RTE_I2S4_RX_INPUT             USIC2_C0_DX0_P3_7
#elif  (RTE_I2S4_RX_ID == 1)
#define RTE_I2S4_RX_PORT              P5_0
#define RTE_I2S4_RX_INPUT             USIC2_C0_DX0_P5_0
#elif  (RTE_I2S4_RX_ID == 2)
#define RTE_I2S4_RX_PORT              P5_1
#define RTE_I2S4_RX_INPUT             USIC2_C0_DX0_P5_1
#elif  (RTE_I2S4_RX_ID == 3)
#define RTE_I2S4_RX_PORT              P6_5
#define RTE_I2S4_RX_INPUT             USIC2_C0_DX0_P6_5
#else
#error "Invalid I2S4_RX Pin Configuration!"
#endif

//   <o> I2S4_CLK OUTPUT Pin <0=>P3_9 <1=>P5_2
#define RTE_I2S4_CLK_OUTPUT_ID                0
#if    (RTE_I2S4_CLK_OUTPUT_ID == 0)
#define RTE_I2S4_CLK_OUTPUT_PORT              P3_9
#define RTE_I2S4_CLK_AF                       P3_9_AF_U2C0_SCLKOUT
#elif  (RTE_I2S4_CLK_OUTPUT_ID == 1)
#define RTE_I2S4_CLK_OUTPUT_PORT              P5_2
#define RTE_I2S4_CLK_AF                       P5_2_AF_U2C0_SCLKOUT
#else
#error "Invalid I2S4 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2S4_CLK INPUT Pin <0=>P5_2
#define RTE_I2S4_CLK_INPUT_ID                0
#if    (RTE_I2S4_CLK_INPUT_ID == 0)
#define RTE_I2S4_CLK_INPUT_PORT              P5_2
#define RTE_I2S4_CLK_INPUT                   USIC2_C0_DX1_P5_2
#else
#error "Invalid I2S4 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2S4_WS OUTPUT Pin <0=>P3_10 <1=>P5_3
#define RTE_I2S4_WS_OUTPUT_ID                0
#if     (RTE_I2S4_WS_OUTPUT_ID == 0)
#define RTE_I2S4_WS_OUTPUT_PORT              P3_10
#define RTE_I2S4_WS_AF                       P3_10_AF_U2C0_SELO0
#elif   (RTE_I2S4_WS_OUTPUT_ID == 1)
#define RTE_I2S4_WS_OUTPUT_PORT              P5_3
#define RTE_I2S4_WS_AF                       P5_3_AF_U2C0_SELO0
#else
#error "Invalid I2S4_WS Output Pin Configuration!"
#endif

//   <o> I2S4_WS INPUT Pin <0=>P5_3
#define RTE_I2S4_WS_INPUT_ID                 0
#if    (RTE_I2S4_WS_INPUT_ID == 0)
#define RTE_I2S4_WS_INPUT_PORT               P5_3
#define RTE_I2S4_WS_INPUT_ID_INPUT           USIC2_C0_DX2_P5_3
#else
#error "Invalid I2S4_WS Input Pin Configuration!"
#endif

#define RTE_I2S4_MCLK_OUTPUT_ENABLE          0

//   <o> I2S4_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S4_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S4_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S4_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S4_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S4_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S4_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S4_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S4_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S4_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S4_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S4_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S4_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S4_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S4_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S4_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S4_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S4_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S4_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S4_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S4_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S4_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S4_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S4_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S4_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S4_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S4_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S4_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S4_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S4_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S4_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S4_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S4_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S4_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S4_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S4_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S4_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S4_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S4_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S4_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S4_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S4_TX FIFO SIZE Configuration!"
#endif
//</e>

// <e> I2S5 (Inter-Integrated Sound) [Driver_SAI5]
// <i> Configuration settings for Driver_SAI5 in component ::Drivers:SAI
#define RTE_I2S5                      0

//   <o> I2S5_TX Pin <0=>P3_5 <1=>P3_11 <2=>P4_7
#define RTE_I2S5_TX_ID                0
#if    (RTE_I2S5_TX_ID == 0)
#define RTE_I2S5_TX_PORT              P3_5
#define RTE_I2S5_TX_AF                P3_5_AF_U2C1_DOUT0
#elif  (RTE_I2S5_TX_ID == 1)
#define RTE_I2S5_TX_PORT              P3_11
#define RTE_I2S5_TX_AF                P3_11_AF_U2C1_DOUT0
#elif  (RTE_I2S5_TX_ID == 2)
#define RTE_I2S5_TX_PORT              P4_7
#define RTE_I2S5_TX_AF                P4_7_AF_U2C1_DOUT0
#else
#error "Invalid I2S5_TX Pin Configuration!"
#endif

//   <o> I2S5_RX Pin <0=>P3_4 <1=>P3_5 <2=>P3_12 <3=>P4_0 <3=>P4_6
#define RTE_I2S5_RX_ID                0
#if    (RTE_I2S5_RX_ID == 0)
#define RTE_I2S5_RX_PORT              P3_4
#define RTE_I2S5_RX_INPUT             USIC2_C1_DX0_P3_4
#elif  (RTE_I2S5_RX_ID == 1)
#define RTE_I2S5_RX_PORT              P3_5
#define RTE_I2S5_RX_INPUT             USIC2_C1_DX0_P3_5
#elif  (RTE_I2S5_RX_ID == 2)
#define RTE_I2S5_RX_PORT              P3_12
#define RTE_I2S5_RX_INPUT             USIC2_C1_DX0_P3_12
#elif  (RTE_I2S5_RX_ID == 3)
#define RTE_I2S5_RX_PORT              P4_0
#define RTE_I2S5_RX_INPUT             USIC2_C1_DX0_P4_0
#elif  (RTE_I2S5_RX_ID == 4)
#define RTE_I2S5_RX_PORT              P4_6
#define RTE_I2S5_RX_INPUT             USIC2_C1_DX0_P4_6
#else
#error "Invalid I2S5_RX Pin Configuration!"
#endif

//   <o> I2S5_CLK OUTPUT Pin <0=>P3_6 <1=>P3_13 <2=>P4_2
#define RTE_I2S5_CLK_OUTPUT_ID                0
#if    (RTE_I2S5_CLK_OUTPUT_ID == 0)
#define RTE_I2S5_CLK_OUTPUT_PORT              P3_6
#define RTE_I2S5_CLK_AF                       P3_6_AF_U2C1_SCLKOUT
#elif  (RTE_I2S5_CLK_OUTPUT_ID == 1)
#define RTE_I2S5_CLK_OUTPUT_PORT              P3_13
#define RTE_I2S5_CLK_AF                       P3_13_AF_U2C1_SCLKOUT
#elif  (RTE_I2S5_CLK_OUTPUT_ID == 2)
#define RTE_I2S5_CLK_OUTPUT_PORT              P4_2
#define RTE_I2S5_CLK_AF                       P4_2_AF_U2C1_SCLKOUT
#else
#error "Invalid I2S5 CLOCK OUTPUT Pin Configuration!"
#endif

//   <o> I2S5_CLK INPUT Pin <0=>P3_6 <1=>P4_2
#define RTE_I2S5_CLK_INPUT_ID                0
#if    (RTE_I2S5_CLK_INPUT_ID == 0)
#define RTE_I2S5_CLK_INPUT_PORT              P3_6
#define RTE_I2S5_CLK_INPUT                   USIC2_C1_DX1_P3_6
#elif   (RTE_I2S5_CLK_INPUT_ID == 1)
#define RTE_I2S5_CLK_INPUT_PORT              P4_2
#define RTE_I2S5_CLK_INPUT                   USIC2_C1_DX1_P4_2
#else
#error "Invalid I2S5 CLOCK INPUT Pin Configuration!"
#endif

//   <o> I2S5_WS OUTPUT Pin <0=>P3_0 <1=>P4_1
#define RTE_I2S5_WS_OUTPUT_ID                0
#if     (RTE_I2S5_WS_OUTPUT_ID == 0)
#define RTE_I2S5_WS_OUTPUT_PORT              P3_0
#define RTE_I2S5_WS_AF                       P3_0_AF_U2C1_SELO0
#elif   (RTE_I2S5_WS_OUTPUT_ID == 1)
#define RTE_I2S5_WS_OUTPUT_PORT              P4_1
#define RTE_I2S5_WS_AF                       P4_1_AF_U2C1_SELO0
#else
#error "Invalid I2S5_WS Output Pin Configuration!"
#endif

//   <o> I2S5_WS INPUT Pin <0=>P4_1
#define RTE_I2S5_WS_INPUT_ID                 0
#if    (RTE_I2S5_WS_INPUT_ID == 0)
#define RTE_I2S5_WS_INPUT_PORT               P4_1
#define RTE_I2S5_WS_INPUT_ID_INPUT           USIC2_C1_DX2_P4_1
#else
#error "Invalid I2S5_WS Input Pin Configuration!"
#endif

// <e> Enable MCLK output
#define RTE_I2S5_MCLK_OUTPUT_ENABLE          0
//   <o> I2S5_MCLK OUTPUT Pin <0=>P3_4
#define RTE_I2S5_MCLK_OUTPUT_ID              0
#if    (RTE_I2S5_MCLK_OUTPUT_ID == 0)
#define RTE_I2S5_MCLK_OUTPUT_PORT            P3_4
#define RTE_I2S5_MCLK_AF                     P3_4_AF_U2C1_MCLKOUT
#else
#error "Invalid I2S5_MCLK Output Pin Configuration!"
#endif
// </e>

//   <o> I2S5_RX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S5_RX_FIFO_SIZE_ID                4
#if    (RTE_I2S5_RX_FIFO_SIZE_ID == 0)
#define RTE_I2S5_RX_FIFO_SIZE         NO_FIFO
#define RTE_I2S5_RX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S5_RX_FIFO_SIZE_ID == 1)
#define RTE_I2S5_RX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S5_RX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S5_RX_FIFO_SIZE_ID == 2)
#define RTE_I2S5_RX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S5_RX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S5_RX_FIFO_SIZE_ID == 3)
#define RTE_I2S5_RX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S5_RX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S5_RX_FIFO_SIZE_ID == 4)
#define RTE_I2S5_RX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S5_RX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S5_RX_FIFO_SIZE_ID == 5)
#define RTE_I2S5_RX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S5_RX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S5_RX FIFO SIZE Configuration!"
#endif

//   <o> I2S5_TX FIFO ENTRIES <0=>0 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
#define RTE_I2S5_TX_FIFO_SIZE_ID                4
#if    (RTE_I2S5_TX_FIFO_SIZE_ID == 0)
#define RTE_I2S5_TX_FIFO_SIZE         NO_FIFO
#define RTE_I2S5_TX_FIFO_SIZE_NUM     0
#elif  (RTE_I2S5_TX_FIFO_SIZE_ID == 1)
#define RTE_I2S5_TX_FIFO_SIZE         FIFO_SIZE_2
#define RTE_I2S5_TX_FIFO_SIZE_NUM     2
#elif  (RTE_I2S5_TX_FIFO_SIZE_ID == 2)
#define RTE_I2S5_TX_FIFO_SIZE         FIFO_SIZE_4
#define RTE_I2S5_TX_FIFO_SIZE_NUM     4
#elif  (RTE_I2S5_TX_FIFO_SIZE_ID == 3)
#define RTE_I2S5_TX_FIFO_SIZE         FIFO_SIZE_8
#define RTE_I2S5_TX_FIFO_SIZE_NUM     8
#elif  (RTE_I2S5_TX_FIFO_SIZE_ID == 4)
#define RTE_I2S5_TX_FIFO_SIZE         FIFO_SIZE_16
#define RTE_I2S5_TX_FIFO_SIZE_NUM     16
#elif  (RTE_I2S5_TX_FIFO_SIZE_ID == 5)
#define RTE_I2S5_TX_FIFO_SIZE         FIFO_SIZE_32
#define RTE_I2S5_TX_FIFO_SIZE_NUM     32
#else
#error "Invalid I2S5_TX FIFO SIZE Configuration!"
#endif
//</e>
#endif // RTE_Drivers_I2S

#ifdef RTE_Drivers_ENET
// <e> ENET (Ethernet Interface) [Driver_ETH_MAC0]
// <i> Configuration settings for Driver_ETH_MAC0 in component ::Drivers:Ethernet MAC

//   <e> MII (Media Independent Interface)

//     <o> TXD0 (Transmit data bit 0) pin 
//       <0=>P0_5
//       <1=>P1_13
//       <2=>P2_8
//       <3=>P2_12
#if (RTE_ENET_MII_TXD0_PIN_ID == 0)
 #define RTE_ENET_MII_TXD0_PIN  P0_5
 #define RTE_ENET_MII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD0_PIN_ID == 1)
 #define RTE_ENET_MII_TXD0_PIN  P1_13
 #define RTE_ENET_MII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD0_PIN_ID == 2)
 #define RTE_ENET_MII_TXD0_PIN  P2_8
 #define RTE_ENET_MII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD0_PIN_ID == 3)
 #define RTE_ENET_MII_TXD0_PIN  P2_12
 #define RTE_ENET_MII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXD0 pin!"
#endif

//     <o> TXD1 (Transmit data bit 1) pin 
//       <0=>P0_6
//       <1=>P1_14
//       <2=>P2_9
//       <3=>P2_13
#if (RTE_ENET_MII_TXD1_PIN_ID == 0)
 #define RTE_ENET_MII_TXD1_PIN  P0_6
 #define RTE_ENET_MII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD1_PIN_ID == 1)
 #define RTE_ENET_MII_TXD1_PIN  P1_14
 #define RTE_ENET_MII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD1_PIN_ID == 2)
 #define RTE_ENET_MII_TXD1_PIN  P2_9
 #define RTE_ENET_MII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD1_PIN_ID == 3)
 #define RTE_ENET_MII_TXD1_PIN  P2_13
 #define RTE_ENET_MII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXD1 pin!"
#endif

//     <o> TXD2 (Transmit data bit 2) pin 
//       <0=>P2_12
//       <1=>P6_0
#if (RTE_ENET_MII_TXD2_PIN_ID == 0)
 #define RTE_ENET_MII_TXD2_PIN  P2_12
 #define RTE_ENET_MII_TXD2_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD2_PIN_ID == 1)
 #define RTE_ENET_MII_TXD2_PIN  P6_0
 #define RTE_ENET_MII_TXD2_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#else
 #error "Invalid TXD2 pin!"
#endif

//     <o> TXD3 (Transmit data bit 3) pin 
//       <0=>P2_13
//       <1=>P6_1
#if (RTE_ENET_MII_TXD3_PIN_ID == 0)
 #define RTE_ENET_MII_TXD3_PIN  P2_13
 #define RTE_ENET_MII_TXD3_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXD3_PIN_ID == 1)
 #define RTE_ENET_MII_TXD3_PIN  P6_1
 #define RTE_ENET_MII_TXD3_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#else
 #error "Invalid TXD3 pin!"
#endif

//     <o> TXEN (Transmit Enable) pin 
//       <0=>P0_4
//       <1=>P1_12
//       <2=>P2_5
//       <3=>P5_9
#if (RTE_ENET_MII_TXEN_PIN_ID == 0)
 #define RTE_ENET_MII_TXEN_PIN  P0_4
 #define RTE_ENET_MII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXEN_PIN_ID == 1)
 #define RTE_ENET_MII_TXEN_PIN  P1_12
 #define RTE_ENET_MII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXEN_PIN_ID == 2)
 #define RTE_ENET_MII_TXEN_PIN  P2_5
 #define RTE_ENET_MII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXEN_PIN_ID == 3)
 #define RTE_ENET_MII_TXEN_PIN  P5_9
 #define RTE_ENET_MII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXEN pin!"
#endif

//     <o> TXER (Transmit Error) pin 
//       <0=>P2_11
//       <1=>P6_2
#if (RTE_ENET_MII_TXER_PIN_ID == 0)
 #define RTE_ENET_MII_TXER_PIN  P2_11
 #define RTE_ENET_MII_TXER_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MII_TXER_PIN_ID == 1)
 #define RTE_ENET_MII_TXER_PIN  P6_2
 #define RTE_ENET_MII_TXER_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#else
 #error "Invalid TXER pin!"
#endif

//     <o> TXCLK (PHY Transmit clock) pin 
//       <0=>P5_10
//       <1=>P6_6
#if (RTE_ENET_MII_TXCLK_PIN_ID == 0)
 #define RTE_ENET_MII_TXCLK_PIN  P5_10
#elif (RTE_ENET_MII_TXCLK_PIN_ID == 1)
 #define RTE_ENET_MII_TXCLK_PIN  P6_6
#else
 #error "Invalid TXCLK pin!"
#endif

//     <o> RXD0 (Receive data bit 0) pin 
//       <0=>P2_2
//       <1=>P0_2
//       <2=>P14_8
//       <3=>P5_0
#if (RTE_ENET_MII_RXD0_PIN_ID == 0)
 #define RTE_ENET_MII_RXD0_PIN  P2_2
#elif (RTE_ENET_MII_RXD0_PIN_ID == 1)
 #define RTE_ENET_MII_RXD0_PIN  P0_2
#elif (RTE_ENET_MII_RXD0_PIN_ID == 2)
 #define RTE_ENET_MII_RXD0_PIN  P14_8
#elif (RTE_ENET_MII_RXD0_PIN_ID == 3)
 #define RTE_ENET_MII_RXD0_PIN  P5_0
#else
 #error "Invalid RXD0 pin!"
#endif

//     <o> RXD1 (Receive data bit 1) pin 
//       <0=>P2_3
//       <1=>P0_3
//       <2=>P14_9
//       <3=>P5_1
#if (RTE_ENET_MII_RXD1_PIN_ID == 0)
 #define RTE_ENET_MII_RXD1_PIN  P2_3
#elif (RTE_ENET_MII_RXD1_PIN_ID == 1)
 #define RTE_ENET_MII_RXD1_PIN  P0_3
#elif (RTE_ENET_MII_RXD1_PIN_ID == 2)
 #define RTE_ENET_MII_RXD1_PIN  P14_9
#elif (RTE_ENET_MII_RXD1_PIN_ID == 3)
 #define RTE_ENET_MII_RXD1_PIN  P5_1
#else
 #error "Invalid RXD1 pin!"
#endif

//     <o> RXD2 (Receive data bit 2) pin 
//       <0=>P5_8
//       <1=>P6_4
#if (RTE_ENET_MII_RXD2_PIN_ID == 0)
 #define RTE_ENET_MII_RXD2_PIN  P5_8
#elif (RTE_ENET_MII_RXD2_PIN_ID == 1)
 #define RTE_ENET_MII_RXD2_PIN  P6_4
#else
 #error "Invalid RXD2 pin!"
#endif

//     <o> RXD3 (Receive data bit 3) pin 
//       <0=>P5_9
//       <1=>P6_3
#if (RTE_ENET_MII_RXD3_PIN_ID == 0)
 #define RTE_ENET_MII_RXD3_PIN  P5_9
#elif (RTE_ENET_MII_RXD3_PIN_ID == 1)
 #define RTE_ENET_MII_RXD3_PIN  P6_3
#else
 #error "Invalid RXD3 pin!"
#endif

//     <o> RXDV (Receive Data Valid) pin 
//       <0=>P2_5
//       <1=>P0_1
//       <2=>P15_9
//       <3=>P5_2
#if (RTE_ENET_MII_RXDV_PIN_ID == 0)
 #define RTE_ENET_MII_RXDV_PIN  P2_5
#elif (RTE_ENET_MII_RXDV_PIN_ID == 1)
 #define RTE_ENET_MII_RXDV_PIN  P0_1
#elif (RTE_ENET_MII_RXDV_PIN_ID == 2)
 #define RTE_ENET_MII_RXDV_PIN  P15_9
#elif (RTE_ENET_MII_RXDV_PIN_ID == 3)
 #define RTE_ENET_MII_RXDV_PIN  P5_2
#else
 #error "Invalid RXDV pin!"
#endif

//     <o> COL (Collision Detect) pin 
//       <0=>P2_15
//       <3=>P5_5
#if (RTE_ENET_MII_COL_PIN_ID == 0)
 #define RTE_ENET_MII_COL_PIN  P2_15
#elif (RTE_ENET_MII_COL_PIN_ID == 3)
 #define RTE_ENET_MII_COL_PIN  P5_5
#else
 #error "Invalid COL pin!"
#endif

//     <o> CRS (Carrier Sense) pin 
//       <0=>P5_11
//       <3=>P5_4
#if (RTE_ENET_MII_CRS_PIN_ID == 0)
 #define RTE_ENET_MII_CRS_PIN  P5_11
#elif (RTE_ENET_MII_CRS_PIN_ID == 3)
 #define RTE_ENET_MII_CRS_PIN  P5_4
#else
 #error "Invalid CRS pin!"
#endif

//     <o> RXER (Receive Error) pin 
//       <0=>P2_4
//       <1=>P0_11
//       <3=>P5_3
#if (RTE_ENET_MII_RXER_PIN_ID == 0)
 #define RTE_ENET_MII_RXER_PIN  P2_4
#elif (RTE_ENET_MII_RXER_PIN_ID == 1)
 #define RTE_ENET_MII_RXER_PIN  P0_11
#elif (RTE_ENET_MII_RXER_PIN_ID == 3)
 #define RTE_ENET_MII_RXER_PIN  P5_3
#else
 #error "Invalid RXER pin!"
#endif

//     <o> RXCLK (PHY Receive Clock) pin 
//       <0=>P2_1
//       <1=>P0_0
//       <2=>P15_8
//       <3=>P6_5
#if (RTE_ENET_MII_RXCLK_PIN_ID == 0)
 #define RTE_ENET_MII_RXCLK_PIN  P2_1
#elif (RTE_ENET_MII_RXCLK_PIN_ID == 1)
 #define RTE_ENET_MII_RXCLK_PIN  P0_0
#elif (RTE_ENET_MII_RXCLK_PIN_ID == 2)
 #define RTE_ENET_MII_RXCLK_PIN  P15_8
#elif (RTE_ENET_MII_RXCLK_PIN_ID == 3)
 #define RTE_ENET_MII_RXCLK_PIN  P6_5
#else
 #error "Invalid RXCLK pin!"
#endif

//   </e> MII (Media Independent Interface)

//   <e> RMII (Reduced Media Independent Interface)

//     <o> TXD0 (Transmit data bit 0) pin 
//       <0=>P0_5
//       <1=>P1_13
//       <2=>P2_8
//       <3=>P2_12
#if (RTE_ENET_RMII_TXD0_PIN_ID == 0)
 #define RTE_ENET_RMII_TXD0_PIN  P0_5
 #define RTE_ENET_RMII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD0_PIN_ID == 1)
 #define RTE_ENET_RMII_TXD0_PIN  P1_13
 #define RTE_ENET_RMII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD0_PIN_ID == 2)
 #define RTE_ENET_RMII_TXD0_PIN  P2_8
 #define RTE_ENET_RMII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD0_PIN_ID == 3)
 #define RTE_ENET_RMII_TXD0_PIN  P2_12
 #define RTE_ENET_RMII_TXD0_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXD0 pin!"
#endif

//     <o> TXD1 (Transmit data bit 1) pin 
//       <0=>P0_6
//       <1=>P1_14
//       <2=>P2_9
//       <3=>P2_13
#if (RTE_ENET_RMII_TXD1_PIN_ID == 0)
 #define RTE_ENET_RMII_TXD1_PIN  P0_6
 #define RTE_ENET_RMII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD1_PIN_ID == 1)
 #define RTE_ENET_RMII_TXD1_PIN  P1_14
 #define RTE_ENET_RMII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD1_PIN_ID == 2)
 #define RTE_ENET_RMII_TXD1_PIN  P2_9
 #define RTE_ENET_RMII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXD1_PIN_ID == 3)
 #define RTE_ENET_RMII_TXD1_PIN  P2_13
 #define RTE_ENET_RMII_TXD1_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXD1 pin!"
#endif

//     <o> TXEN (Transmit Enable) pin 
//       <0=>P0_4
//       <1=>P1_12
//       <2=>P2_5
//       <3=>P5_9
#if (RTE_ENET_RMII_TXEN_PIN_ID == 0)
 #define RTE_ENET_RMII_TXEN_PIN  P0_4
 #define RTE_ENET_RMII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXEN_PIN_ID == 1)
 #define RTE_ENET_RMII_TXEN_PIN  P1_12
 #define RTE_ENET_RMII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXEN_PIN_ID == 2)
 #define RTE_ENET_RMII_TXEN_PIN  P2_5
 #define RTE_ENET_RMII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_RMII_TXEN_PIN_ID == 3)
 #define RTE_ENET_RMII_TXEN_PIN  P5_9
 #define RTE_ENET_RMII_TXEN_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4
#else
 #error "Invalid TXEN pin!"
#endif

//     <o> RXD0 (Receive data bit 0) pin 
//       <0=>P2_2
//       <1=>P0_2
//       <2=>P14_8
//       <3=>P5_0
#if (RTE_ENET_RMII_RXD0_PIN_ID == 0)
 #define RTE_ENET_RMII_RXD0_PIN  P2_2
#elif (RTE_ENET_RMII_RXD0_PIN_ID == 1)
 #define RTE_ENET_RMII_RXD0_PIN  P0_2
#elif (RTE_ENET_RMII_RXD0_PIN_ID == 2)
 #define RTE_ENET_RMII_RXD0_PIN  P14_8
#elif (RTE_ENET_RMII_RXD0_PIN_ID == 3)
 #define RTE_ENET_RMII_RXD0_PIN  P5_0
#else
 #error "Invalid RXD0 pin!"
#endif

//     <o> RXD1 (Receive data bit 1) pin 
//       <0=>P2_3
//       <1=>P0_3
//       <2=>P14_9
//       <3=>P5_1
#if (RTE_ENET_RMII_RXD1_PIN_ID == 0)
 #define RTE_ENET_RMII_RXD1_PIN  P2_3
#elif (RTE_ENET_RMII_RXD1_PIN_ID == 1)
 #define RTE_ENET_RMII_RXD1_PIN  P0_3
#elif (RTE_ENET_RMII_RXD1_PIN_ID == 2)
 #define RTE_ENET_RMII_RXD1_PIN  P14_9
#elif (RTE_ENET_RMII_RXD1_PIN_ID == 3)
 #define RTE_ENET_RMII_RXD1_PIN  P5_1
#else
 #error "Invalid RXD1 pin!"
#endif

//     <o> CRS_DV (Carrier Sense Data Valid) pin 
//       <0=>P2_5
//       <1=>P0_1
//       <2=>P15_9
//       <3=>P5_2
#if (RTE_ENET_RMII_CRS_DV_PIN_ID == 0)
 #define RTE_ENET_RMII_CRS_DV_PIN  P2_5
#elif (RTE_ENET_RMII_CRS_DV_PIN_ID == 1)
 #define RTE_ENET_RMII_CRS_DV_PIN  P0_1
#elif (RTE_ENET_RMII_CRS_DV_PIN_ID == 2)
 #define RTE_ENET_RMII_CRS_DV_PIN  P15_9
#elif (RTE_ENET_RMII_CRS_DV_PIN_ID == 3)
 #define RTE_ENET_RMII_CRS_DV_PIN  P5_2
#else
 #error "Invalid CRS_DV pin!"
#endif

//     <o> RXER (Receive Error) pin 
//       <0=>P2_4
//       <2=>P0_11
//       <3=>P5_3
#if (RTE_ENET_RMII_RXER_PIN_ID == 0)
 #define RTE_ENET_RMII_RXER_PIN  P2_4
#elif (RTE_ENET_RMII_RXER_PIN_ID == 2)
 #define RTE_ENET_RMII_RXER_PIN  P0_11
#elif (RTE_ENET_RMII_RXER_PIN_ID == 3)
 #define RTE_ENET_RMII_RXER_PIN  P5_3
#else
 #error "Invalid RXER pin!"
#endif

//     <o> REFCLK (Reference clock) pin 
//       <0=>P2_1
//       <1=>P0_0
//       <2=>P15_8
//       <3=>P6_5
#if (RTE_ENET_RMII_REFCLK_PIN_ID == 0)
 #define RTE_ENET_RMII_REFCLK_PIN  P2_1
#elif (RTE_ENET_RMII_REFCLK_PIN_ID == 1)
 #define RTE_ENET_RMII_REFCLK_PIN  P0_0
#elif (RTE_ENET_RMII_REFCLK_PIN_ID == 2)
 #define RTE_ENET_RMII_REFCLK_PIN  P15_8
#elif (RTE_ENET_RMII_REFCLK_PIN_ID == 3)
 #define RTE_ENET_RMII_REFCLK_PIN  P6_5
#else
 #error "Invalid REFCLK pin!"
#endif

//   </e> RMII (Reduce Media Independent Interface)

//   <h> MIIM (Management Data Interface)
//     <o> MDIO (Management data I/O line) pin 
//       <0=>P0_9
//       <1=>P2_0
//       <2=>P1_11
#if (RTE_ENET_MIIM_MDIO_PIN_ID == 0)
 #define RTE_ENET_MIIM_MDIO_PIN   P0_9
 #define RTE_ENET_MIIM_MDIO_HWSEL XMC_GPIO_HWCTRL_PERIPHERAL1
#elif (RTE_ENET_MIIM_MDIO_PIN_ID == 1)
 #define RTE_ENET_MIIM_MDIO_PIN   P2_0
 #define RTE_ENET_MIIM_MDIO_HWSEL XMC_GPIO_HWCTRL_PERIPHERAL1
#elif (RTE_ENET_MIIM_MDIO_PIN_ID == 2)
 #define RTE_ENET_MIIM_MDIO_PIN   P1_11
 #define RTE_ENET_MIIM_MDIO_HWSEL XMC_GPIO_HWCTRL_PERIPHERAL1
#else
 #error "Invalid MDIO pin!"
#endif

//     <o> MDC (Management data clock line) pin 
//       <0=>P0_10
//       <1=>P1_10
//       <2=>P2_7
#if (RTE_ENET_MIIM_MDC_PIN_ID == 0)
 #define RTE_ENET_MIIM_MDC_PIN  P0_10
 #define RTE_ENET_MIIM_MDC_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MIIM_MDC_PIN_ID == 1)
 #define RTE_ENET_MIIM_MDC_PIN  P1_10
 #define RTE_ENET_MIIM_MDC_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#elif (RTE_ENET_MIIM_MDC_PIN_ID == 2)
 #define RTE_ENET_MIIM_MDC_PIN  P2_7
 #define RTE_ENET_MIIM_MDC_MODE XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1
#else
 #error "Invalid MDC pin!"
#endif
#endif // RTE_Drivers_ENET

#ifdef RTE_Drivers_USB
//   </h> MIIM (Management Data Interface)
// </e> ENET (Ethernet Interface) [Driver_ETH_MAC0]

// <e> USB (Universal Serial Bus) [Driver_USBH]
//     <o> VBUS drive pin setting <0=>P0_1 <1=>P3_2
#if (RTE_USB_HOST_ID == 0)
 #define XMC_USB_DRIVE_PORT    P0_1
#elif  (RTE_USB_HOST_ID == 1)
 #define XMC_USB_DRIVE_PORT    P3_2
#else
 #error "Invalid USB Host pin!"
#endif

// </e> USB (Universal Serial Bus) [Driver_USBH]
#endif // RTE_Drivers_USB

#ifdef RTE_Drivers_SDMMC
// <e> SD/MMC Interface [Driver_MCI0]
// <i> Configuration settings for Driver_MCI0 in component ::Drivers:MCI
#define RTE_SDMMC0                      0

// <e> MCI Card detection pin enable
#define RTE_SDMMC_CD_PIN_EN 0
//   <q> MCI Card detection event enable
#define RTE_SDMMC_CD_EVENT_EN           0
//   <o> MCI Card Detection Pin <0=>P1_10
#define RTE_SDMMC0_CD_PIN_ID            0
#if (RTE_SDMMC0_CD_PIN_ID == 0)
#define RTE_SDMMC_CARD_DETECT_PORT      PORT1_BASE
#define RTE_SDMMC_CARD_DETECT_PIN       10
#else
#error "Invalid SDMMC0_CD Pin Configuration!"
#endif
//</e>

// <e> MCI Write protection pin enable
#define RTE_SDMMC_WP_PIN_EN             0
//   <o> MCI Write Protection Pin <0=>P1_1
#define RTE_SDMMC0_WP_PIN_ID            0
#if (RTE_SDMMC0_WP_PIN_ID == 0)
#define RTE_SDMMC_WRITE_PROTECT_PORT    PORT1_BASE
#define RTE_SDMMC_WRITE_PROTECT_PIN     1
#else
#error "Invalid SDMMC0_WP Pin Configuration!"
#endif
// </e>

// <e> MCI Power pin enable
#define RTE_SDMMC_PWR_PIN_EN            0
//   <o> MCI Bus Power Pin <0=>P3_4
#define RTE_SDMMC0_BUS_POWER_PIN_ID     0
#if (RTE_SDMMC0_BUS_POWER_PIN_ID == 0)
#define RTE_SDMMC_BUS_POWER_PORT        PORT3_BASE
#define RTE_SDMMC_BUS_POWER_PIN         4
#else
#error "Invalid SDMMC0_BUS_POWER Pin Configuration!"
#endif
// </e>

// <e> MCI Reset pin enable
#define RTE_SDMMC_RESET_PIN_EN          0
//   <o> MCI Reset Pin <0=>P0_11
#define RTE_SDMMC0_RESET_PIN_ID         0
#if (RTE_SDMMC0_RESET_PIN_ID == 0)
#define RTE_SDMMC_RESET_PORT            PORT0_BASE
#define RTE_SDMMC_RESET_PIN             11
#else
#error "Invalid SDMMC0_RESET Pin Configuration!"
#endif
// </e>

// <o> MCI DATA0 Pin <0=>P4_0
#define RTE_SDMMC0_DATA0_PIN_ID         0
#if (RTE_SDMMC0_DATA0_PIN_ID == 0)
#define RTE_SDMMC_DATA_0_PORT           PORT4_BASE
#define RTE_SDMMC_DATA_0_PIN            0
#else
#error "Invalid SDMMC0_DATA0 Pin Configuration!"
#endif

// <e> MCI 4 bit bus width enable
#define RTE_SDMMC_BUS_WIDTH_4           1

//   <o> MCI DATA1 Pin <0=>P1_6
#define RTE_SDMMC0_DATA1_PIN_ID         0
#if (RTE_SDMMC0_DATA1_PIN_ID == 0)
#define RTE_SDMMC_DATA_1_PORT           PORT1_BASE
#define RTE_SDMMC_DATA_1_PIN            6
#else
#error "Invalid SDMMC0_DATA1 Pin Configuration!"
#endif

//   <o> MCI DATA2 Pin <0=>P1_7
#define RTE_SDMMC0_DATA2_PIN_ID         0
#if (RTE_SDMMC0_DATA2_PIN_ID == 0)
#define RTE_SDMMC_DATA_2_PORT           PORT1_BASE
#define RTE_SDMMC_DATA_2_PIN            7
#else
#error "Invalid SDMMC0_DATA2 Pin Configuration!"
#endif

//   <o> MCI DATA3 Pin <0=>P4_1
#define RTE_SDMMC0_DATA3_PIN_ID         0
#if (RTE_SDMMC0_DATA3_PIN_ID == 0)
#define RTE_SDMMC_DATA_3_PORT           PORT4_BASE
#define RTE_SDMMC_DATA_3_PIN            1
#else
#error "Invalid SDMMC0_DATA3 Pin Configuration!"
#endif
// </e>

// <o> MCI CMD Pin <0=>P3_5
#define RTE_SDMMC0_CMD_PIN_ID           0
#if (RTE_SDMMC0_CMD_PIN_ID == 0)
#define RTE_SDMMC_COMMAND_PORT          PORT3_BASE
#define RTE_SDMMC_COMMAND_PIN           5
#else
#error "Invalid SDMMC0_CMD Pin Configuration!"
#endif

// <o> MCI CLK Pin <0=>P3_6
#define RTE_SDMMC0_CLK_PIN_ID           0
#if (RTE_SDMMC0_CLK_PIN_ID == 0)
#define RTE_SDMMC_CLK_PORT              PORT3_BASE
#define RTE_SDMMC_CLK_PIN               6
#else
#error "Invalid SDMMC0_CLK Pin Configuration!"
#endif
// </e> SD/MMC Interface [Driver_MCI0]
#endif // RTE_Drivers_SDMMC

#if ((RTE_UART0+RTE_I2C0+RTE_SPI0+RTE_I2S0)>1)
#error "Choose just one Driver_I2C0/SPI0/UART0/SAI0 driver !"
#elif ((RTE_UART1+RTE_I2C1+RTE_SPI1+RTE_I2S1)>1)
#error "Choose just one Driver_I2C1/SPI1/UART1/SAI1 driver !"
#elif ((RTE_UART2+RTE_I2C2+RTE_SPI2+RTE_I2S2)>1)
#error "Choose just one Driver_I2C2/SPI2/UART2/SAI2 driver !"
#elif ((RTE_UART3+RTE_I2C3+RTE_SPI3+RTE_I2S3)>1)
#error "Choose just one Driver_I2C3/SPI3/UART3/SAI3 driver !"
#elif ((RTE_UART4+RTE_I2C4+RTE_SPI4+RTE_I2S4)>1)
#error "Choose just one Driver_I2C4/SPI4/UART4/SAI4 driver !"
#elif ((RTE_UART5+RTE_I2C5+RTE_SPI5+RTE_I2S5)>1)
#error "Choose just one Driver_I2C5/SPI5/UART5/SAI5 driver !"
#endif

#if (((RTE_UART0_RX_FIFO_SIZE_NUM*RTE_UART0)+\
      (RTE_I2C0_RX_FIFO_SIZE_NUM*RTE_I2C0)+\
      (RTE_SPI0_RX_FIFO_SIZE_NUM*RTE_SPI0)+\
      (RTE_I2S0_RX_FIFO_SIZE_NUM*RTE_I2S0)+\
      (RTE_UART1_RX_FIFO_SIZE_NUM*RTE_UART1)+\
      (RTE_I2C1_RX_FIFO_SIZE_NUM*RTE_I2C1)+\
      (RTE_SPI1_RX_FIFO_SIZE_NUM*RTE_SPI1)+\
      (RTE_I2S1_RX_FIFO_SIZE_NUM*RTE_I2S1)+\
      (RTE_UART0_TX_FIFO_SIZE_NUM*RTE_UART0)+\
      (RTE_I2C0_TX_FIFO_SIZE_NUM*RTE_I2C0)+\
      (RTE_SPI0_TX_FIFO_SIZE_NUM*RTE_SPI0)+\
      (RTE_I2S0_TX_FIFO_SIZE_NUM*RTE_I2S0)+\
      (RTE_UART1_TX_FIFO_SIZE_NUM*RTE_UART1)+\
      (RTE_I2C1_TX_FIFO_SIZE_NUM*RTE_I2C1)+\
      (RTE_SPI1_TX_FIFO_SIZE_NUM*RTE_SPI1)+\
      (RTE_I2S1_TX_FIFO_SIZE_NUM*RTE_I2S1))>64)
#error "Just 64 positions in a FIFO shared by RTE_x0 and RTE_x1 !"
#elif (((RTE_UART2_RX_FIFO_SIZE_NUM*RTE_UART2)+\
        (RTE_I2C2_RX_FIFO_SIZE_NUM*RTE_I2C2)+\
        (RTE_SPI2_RX_FIFO_SIZE_NUM*RTE_SPI2)+\
        (RTE_I2S2_RX_FIFO_SIZE_NUM*RTE_I2S2)+\
        (RTE_UART3_RX_FIFO_SIZE_NUM*RTE_UART3)+\
        (RTE_I2C3_RX_FIFO_SIZE_NUM*RTE_I2C3)+\
        (RTE_SPI3_RX_FIFO_SIZE_NUM*RTE_SPI3)+\
        (RTE_I2S3_RX_FIFO_SIZE_NUM*RTE_I2S3)+\
        (RTE_UART2_TX_FIFO_SIZE_NUM*RTE_UART2)+\
        (RTE_I2C2_TX_FIFO_SIZE_NUM*RTE_I2C2)+\
        (RTE_SPI2_TX_FIFO_SIZE_NUM*RTE_SPI2)+\
        (RTE_I2S2_TX_FIFO_SIZE_NUM*RTE_I2S2)+\
        (RTE_UART3_TX_FIFO_SIZE_NUM*RTE_UART3)+\
        (RTE_I2C3_TX_FIFO_SIZE_NUM*RTE_I2C3)+\
        (RTE_SPI3_TX_FIFO_SIZE_NUM*RTE_SPI3)+\
        (RTE_I2S3_TX_FIFO_SIZE_NUM*RTE_I2S3))>64)
#error "Just 64 positions in a FIFO shared by RTE_x2 and RTE_x3 !"
#elif (((RTE_UART4_RX_FIFO_SIZE_NUM*RTE_UART4)+\
        (RTE_I2C4_RX_FIFO_SIZE_NUM*RTE_I2C4)+\
        (RTE_SPI4_RX_FIFO_SIZE_NUM*RTE_SPI4)+\
        (RTE_I2S4_RX_FIFO_SIZE_NUM*RTE_I2S4)+\
        (RTE_UART5_RX_FIFO_SIZE_NUM*RTE_UART5)+\
        (RTE_I2C5_RX_FIFO_SIZE_NUM*RTE_I2C5)+\
        (RTE_SPI5_RX_FIFO_SIZE_NUM*RTE_SPI5)+\
        (RTE_I2S5_RX_FIFO_SIZE_NUM*RTE_I2S5)+\
        (RTE_UART4_TX_FIFO_SIZE_NUM*RTE_UART4)+\
        (RTE_I2C4_TX_FIFO_SIZE_NUM*RTE_I2C4)+\
        (RTE_SPI4_TX_FIFO_SIZE_NUM*RTE_SPI4)+\
        (RTE_I2S4_TX_FIFO_SIZE_NUM*RTE_I2S4)+\
        (RTE_UART5_TX_FIFO_SIZE_NUM*RTE_UART5)+\
        (RTE_I2C5_TX_FIFO_SIZE_NUM*RTE_I2C5)+\
        (RTE_SPI5_TX_FIFO_SIZE_NUM*RTE_SPI5)+\
        (RTE_I2S5_TX_FIFO_SIZE_NUM*RTE_I2S5))>64)
#error "Just 64 positions in a FIFO shared by RTE_x4 and RTE_x5 !"
#endif
