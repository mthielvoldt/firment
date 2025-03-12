/** Provides project-specific config for CMSIS (ARM) SPI Driver.
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
 * - MCU choice (RTE_Device.h) determines which macros need defining
 */
#ifndef spi_pcbDetails_h
#define spi_pcbDetails_h

#define RTE_SPI0 0 // XMC_SPI0_CH0
#define RTE_SPI1 0 // XMC_SPI0_CH1
#define RTE_SPI2 0 // XMC_SPI1_CH0
#define RTE_SPI3 0 // XMC_SPI1_CH1
#define RTE_SPI4 0 // XMC_SPI2_CH0
#define RTE_SPI5 0 // XMC_SPI2_CH1

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
#define RTE_SPI4_SLAVE_SELECT_LINE0 0     // valid range: 0-1 (enable)
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


#endif // include once
