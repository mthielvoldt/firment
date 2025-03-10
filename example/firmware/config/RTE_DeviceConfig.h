/** Provides project-specific peripheral configuration.
 * These macros have been extracted from RTE_Device.h.
 * Normally, parts of RTE_Device.h are edited by a GUI assistant (uVision etc.)
 * Rather than rely on such an assistant, I chose to extract the configurable
 * parts of RTE_Device.h, leaving the written-in-silicon relationships in place.
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

#ifndef RTE_DeviceConfig_H
#define RTE_DeviceConfig_H

#include <Device_IOC.h>

/* Interrupt-on-Change pins.  See Device_IOC.h for your target hardware.*/
/* Note: IOC_n_RTE_INDEX should match position of IOC_n in AVAILABLE_IOCs */
#define AVAILABLE_IOCs {IOC_4, IOC_14}
#define IOC_4_RTE_INDEX 0
#define IOC_14_RTE_INDEX 1

// clearToSend OUTPUT 2
// msgWaiting OUTPUT 3
#define IOC_USE_ISR_0 0
#define IOC_USE_ISR_1 0
#define IOC_USE_ISR_2 1
#define IOC_USE_ISR_3 1
#define IOC_USE_ISR_4 0
#define IOC_USE_ISR_5 0
#define IOC_USE_ISR_6 0
#define IOC_USE_ISR_7 0

#define RTE_CAN0 0
#define RTE_CAN1 0
#define RTE_CAN2 0
#define RTE_CAN3 0
#define RTE_CAN4 0
#define RTE_CAN5 0

#define RTE_UART0 0
#define RTE_UART1 0
#define RTE_UART2 0
#define RTE_UART3 0
#define RTE_UART4 0
#define RTE_UART5 0

#define RTE_SPI0 0 // XMC_SPI0_CH0
#define RTE_SPI1 0 // XMC_SPI0_CH1
#define RTE_SPI2 0 // XMC_SPI1_CH0
#define RTE_SPI3 0 // XMC_SPI1_CH1
#define RTE_SPI4 1 // XMC_SPI2_CH0
#define RTE_SPI5 0 // XMC_SPI2_CH1

#define RTE_ENET 0

#define RTE_USB_HOST_ID 0 // valid range: 0-1

/* CAN sub-configuration.  Translates to specific pin mappings*/
#define RTE_CAN0_TX_ID 0 // valid range: 0-4
#define RTE_CAN0_RX_ID 0 // valid range: 0-2

#define RTE_CAN1_TX_ID 0 // valid range: 0-3
#define RTE_CAN1_RX_ID 0 // valid range: 0-3

#define RTE_CAN2_TX_ID 0 // valid range: 0-2
#define RTE_CAN2_RX_ID 0 // valid range: 0-2

#define RTE_CAN3_TX_ID 0 // valid range: 0-1
#define RTE_CAN3_RX_ID 0 // valid range: 0-1

#define RTE_CAN4_TX_ID 0 // valid range: 0-1
#define RTE_CAN4_RX_ID 0 // valid range: 0-1

#define RTE_CAN5_TX_ID 0 // valid range: 0-1
#define RTE_CAN5_RX_ID 0 // valid range: 0-1

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

/** SPI sub-config.
 * RTE_SPIm_TX_ID  select Tx pin (MOSI when main, MISO when sub).
 * RTE_SPIm_RX_ID  select Rx pin (MISO when main, MOSI when sub).
 * RTE_SPIm_RX_FIFO_SIZE_ID  select FIFO size.  0:no fifo, otherwise 2^x.
 * RTE_SPIm_TX_FIFO_SIZE_ID  select FIFO size.  0:no fifo, otherwise 2^x.
 *
 * Relevant when module m is configured as main:
 * RTE_SPIm_CLK_OUTPUT_ID          select pin for clock.
 * RTE_SPIm_SLAVE_SELECT_LINEn     enable/disable sub-select n.
 * RTE_SPIm_SLAVE_SELECT_LINE_n_ID select pin for sub-select n.
 *
 * Relevant when module m is configured as sub:
 * RTE_SPIm_CLK_INPUT_ID           select pin for clock when config'd as sub.
 * RTE_SPIm_SLAVE_SELECT_INPUT_ID  select pin for sub-select.
 */
#define RTE_SPI0_TX_ID 0                  // valid range: 0-2
#define RTE_SPI0_RX_ID 0                  // valid range: 0-3
#define RTE_SPI0_CLK_OUTPUT_ID 0          // valid range: 0-4
#define RTE_SPI0_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE3 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE4 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE5 0     // valid range: 0-1 (enable)
#define RTE_SPI0_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-2
#define RTE_SPI0_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI0_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI0_SLAVE_SELECT_LINE_3_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI0_SLAVE_SELECT_LINE_4_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI0_SLAVE_SELECT_LINE_5_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI0_CLK_INPUT_ID 0           // valid range: 0-1
#define RTE_SPI0_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-1
#define RTE_SPI0_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI0_TX_FIFO_SIZE_ID 4        // valid range: 0-5

#define RTE_SPI1_TX_ID 0                  // valid range: 0-3
#define RTE_SPI1_RX_ID 0                  // valid range: 0-4
#define RTE_SPI1_CLK_OUTPUT_ID 0          // valid range: 0-3
#define RTE_SPI1_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
#define RTE_SPI1_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI1_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI1_SLAVE_SELECT_LINE3 0     // valid range: 0-1 (enable)
#define RTE_SPI1_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-3
#define RTE_SPI1_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-1
#define RTE_SPI1_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-1
#define RTE_SPI1_SLAVE_SELECT_LINE_3_ID 0 // valid range: 0-1
#define RTE_SPI1_CLK_INPUT_ID 0           // valid range: 0-2
#define RTE_SPI1_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-2
#define RTE_SPI1_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI1_TX_FIFO_SIZE_ID 4        // valid range: 0-5

#define RTE_SPI2_TX_ID 0                  // valid range: 0-2
#define RTE_SPI2_RX_ID 0                  // valid range: 0-4
#define RTE_SPI2_CLK_OUTPUT_ID 0          // valid range: 0-2
#define RTE_SPI2_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
#define RTE_SPI2_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI2_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI2_SLAVE_SELECT_LINE3 0     // valid range: 0-1 (enable)
#define RTE_SPI2_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-1
#define RTE_SPI2_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-1
#define RTE_SPI2_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI2_SLAVE_SELECT_LINE_3_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI2_CLK_INPUT_ID 0           // valid range: 0-1
#define RTE_SPI2_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-1
#define RTE_SPI2_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI2_TX_FIFO_SIZE_ID 4        // valid range: 0-5

#define RTE_SPI3_TX_ID 0                  // valid range: 0-3
#define RTE_SPI3_RX_ID 0                  // valid range: 0-3
#define RTE_SPI3_CLK_OUTPUT_ID 0          // valid range: 0-2
#define RTE_SPI3_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
#define RTE_SPI3_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI3_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI3_SLAVE_SELECT_LINE3 0     // valid range: 0-1 (enable)
#define RTE_SPI3_SLAVE_SELECT_LINE4 0     // valid range: 0-1 (enable)
#define RTE_SPI3_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-1
#define RTE_SPI3_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-1
#define RTE_SPI3_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI3_SLAVE_SELECT_LINE_3_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI3_SLAVE_SELECT_LINE_4_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI3_CLK_INPUT_ID 0           // valid range: 0-2
#define RTE_SPI3_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-1
#define RTE_SPI3_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI3_TX_FIFO_SIZE_ID 4        // valid range: 0-5

#define RTE_SPI4_TX_ID 0                  // valid range: 0-2
#define RTE_SPI4_RX_ID 0                  // valid range: 0-3
#define RTE_SPI4_CLK_OUTPUT_ID 0          // valid range: 0-1
#define RTE_SPI4_SLAVE_SELECT_LINE0 1     // valid range: 0-1 (enable)
#define RTE_SPI4_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI4_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI4_SLAVE_SELECT_LINE3 0     // valid range: 0-1 (enable)
#define RTE_SPI4_SLAVE_SELECT_LINE4 0     // valid range: 0-1 (enable)
#define RTE_SPI4_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-1
#define RTE_SPI4_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI4_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI4_SLAVE_SELECT_LINE_3_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI4_SLAVE_SELECT_LINE_4_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI4_CLK_INPUT_ID 0           // valid range: 0-0 (only option)
#define RTE_SPI4_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-0 (only option)
#define RTE_SPI4_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI4_TX_FIFO_SIZE_ID 4        // valid range: 0-5

#define RTE_SPI5_TX_ID 0                  // valid range: 0-2
#define RTE_SPI5_RX_ID 0                  // valid range: 0-4
#define RTE_SPI5_CLK_OUTPUT_ID 0          // valid range: 0-2
#define RTE_SPI5_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
#define RTE_SPI5_SLAVE_SELECT_LINE1 0     // valid range: 0-1 (enable)
#define RTE_SPI5_SLAVE_SELECT_LINE2 0     // valid range: 0-1 (enable)
#define RTE_SPI5_SLAVE_SELECT_LINE_0_ID 0 // valid range: 0-1
#define RTE_SPI5_SLAVE_SELECT_LINE_1_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI5_SLAVE_SELECT_LINE_2_ID 0 // valid range: 0-0 (only option)
#define RTE_SPI5_CLK_INPUT_ID 0           // valid range: 0-1
#define RTE_SPI5_SLAVE_SELECT_INPUT_ID 0  // valid range: 0-0 (only option)
#define RTE_SPI5_RX_FIFO_SIZE_ID 4        // valid range: 0-5
#define RTE_SPI5_TX_FIFO_SIZE_ID 4        // valid range: 0-5

/* Ethernet Interface sub-config */
#define RTE_ENET_MII 0             // enable Media Independent Interface (MII)
#define RTE_ENET_MII_TXD0_PIN_ID 0 // select Tx data bit 0 pin. Range: 0-3
#define RTE_ENET_MII_TXD1_PIN_ID 0 // select Tx data bit 1 pin. Range: 0-3
#define RTE_ENET_MII_TXD2_PIN_ID 0 // select Tx data bit 2 pin. Range: 0-1
#define RTE_ENET_MII_TXD3_PIN_ID 0 // select Tx data bit 3 pin. Range: 0-1

#define RTE_ENET_MII_TXEN_PIN_ID 0  // select Tx Enable pin.  Range: 0-3
#define RTE_ENET_MII_TXER_PIN_ID 0  // select Tx Error pin. Range: 0-1
#define RTE_ENET_MII_TXCLK_PIN_ID 0 // select PHY Tx clock pin. Range: 0-1
#define RTE_ENET_MII_RXD0_PIN_ID 0  // select Rx data bit 0 pin. Range: 0-3
#define RTE_ENET_MII_RXD1_PIN_ID 0  // select Rx data bit 1 pin. Range: 0-3
#define RTE_ENET_MII_RXD2_PIN_ID 0  // select Rx data bit 2 pin. Range: 0-1
#define RTE_ENET_MII_RXD3_PIN_ID 0  // select Rx data bit 3 pin. Range: 0-1
#define RTE_ENET_MII_RXDV_PIN_ID 0  // select Rx data valid pin. Range: 0-3
#define RTE_ENET_MII_COL_PIN_ID 0   // select collision detect pin. Range: 0,3
#define RTE_ENET_MII_CRS_PIN_ID 0   // select carrier sense pin. Range: 0,3
#define RTE_ENET_MII_RXER_PIN_ID 0  // select Rx Err pin. Range: 0,1,3
#define RTE_ENET_MII_RXCLK_PIN_ID 0 // select PHY Rx Clock pin. Range: 0-3

#define RTE_ENET_RMII 1               // enable Reduced MII interface (RMII)
#define RTE_ENET_RMII_TXD0_PIN_ID 2   // (Transmit data bit 0) pin
#define RTE_ENET_RMII_TXD1_PIN_ID 2   // (Transmit data bit 1) pin
#define RTE_ENET_RMII_TXEN_PIN_ID 2   // (Transmit Enable) pin
#define RTE_ENET_RMII_RXD0_PIN_ID 0   // (Receive data bit 0) pin
#define RTE_ENET_RMII_RXD1_PIN_ID 0   // (Receive data bit 1) pin
#define RTE_ENET_RMII_CRS_DV_PIN_ID 2 // (Carrier Sense Data Valid) pin
#define RTE_ENET_RMII_RXER_PIN_ID 0   // (Receive Error) pin
#define RTE_ENET_RMII_REFCLK_PIN_ID 2 // (Reference clock) pin

#define RTE_ENET_MIIM_MDIO_PIN_ID 1 // (Management data I/O line) pin
#define RTE_ENET_MIIM_MDC_PIN_ID 2  // (Management data clock line) pin

#endif // RTE_DeviceConfig_H