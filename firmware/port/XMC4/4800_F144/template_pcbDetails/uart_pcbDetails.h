/** Provides project-specific config for CMSIS (ARM) UART Driver.
 * 
 * Origins:
 * These macros have been extracted from RTE_Device.h.
 * Normally, parts of RTE_Device.h are edited by a GUI assistant (uVision etc.)
 * and RTE_Device.h intermingles MCU and Project details.
 * 
 * Rather than rely on such an assistant, this file extracts the project-config
 * parts of RTE_Device.h, leaving the fixed-in-silicon details in place.
 *
 * Source-of-Truth for:
 * - availability of HW resources consistent with PCB connections.
 *
 * Language level:
 * - Project  num inbound
 * - HAL      yes outbound
 * - dev.fam  no
 * - device   no
 *
 * Constraints:
 * - Device choice (RTE_Device.h) determines which macros need defining
 */

#ifndef uart_pcbDetails_h
#define uart_pcbDetails_h


#define RTE_UART0 0
#define RTE_UART1 0
#define RTE_UART2 0
#define RTE_UART3 0
#define RTE_UART4 0
#define RTE_UART5 0


/* UART sub-config.  Selects Tx/Rx pins and FIFO sizes. */
#define RTE_UART0_TX_ID 0           // valid range: 0-2
#define RTE_UART0_RX_ID 0           // valid range: 0-3
#define RTE_UART0_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART0_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#define RTE_UART1_TX_ID 0           // valid range: 0-3
#define RTE_UART1_RX_ID 0           // valid range: 0-4
#define RTE_UART1_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART1_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#define RTE_UART2_TX_ID 0           // valid range: 0-2
#define RTE_UART2_RX_ID 0           // valid range: 0-4
#define RTE_UART2_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART2_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#define RTE_UART3_TX_ID 0           // valid range: 0-3
#define RTE_UART3_RX_ID 0           // valid range: 0-3
#define RTE_UART3_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART3_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#define RTE_UART4_TX_ID 0           // valid range: 0-2
#define RTE_UART4_RX_ID 0           // valid range: 0-3
#define RTE_UART4_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART4_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#define RTE_UART5_TX_ID 0           // valid range: 0-2
#define RTE_UART5_RX_ID 0           // valid range: 0-4
#define RTE_UART5_RX_FIFO_SIZE_ID 4 // Valid range: 0-5
#define RTE_UART5_TX_FIFO_SIZE_ID 4 // Valid range: 0-5

#endif // include once ifdef guard
