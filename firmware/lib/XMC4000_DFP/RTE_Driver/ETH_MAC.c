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
 * @file ETH_MAC.c
 * @date 01 June, 2017
 * @version 2.7
 *
 * @brief Ethernet Media Access (MAC) Driver for Infineon XMC4000
 *
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                 Value
 *   ---------------------                 -----
 *   Connect to hardware via Driver_ETH_MAC# = 0
 * -------------------------------------------------------------------- 
 *
 * History
 *
 * Version 2.7
 *  Fixed ETH_MAC_SendFrame allowing the parameter combination (frame = NULL, len = 0) used to discard the ethernet frame.
 *
 * Version 2.6 
 *  Changed ETH_MAC_PowerControl to disable MMC IPC receive interrupts
 *
 * Version 2.5 
 *  Changed ETH_MAC_PowerControl to improve bus accesss
 *
 * Version 2.4 
 *  Fixed clock gating for other device than XMC45xx
 *  Fixes in Power control and Uninitilize based on CMSIS driver validation
 *
 * Version 2.2 
 *  Fixed MII setup
 *  Fixed ETH_MAC_SendFrame when EMAC_TIME_STAMP is passed as flag
 *  Fixed Initialize and PowerControl logic depending on handler.flags
 *
 * Version 2.1
 *  Initial version
 */

/*******************************************************************************
 * INCLUDES
 *******************************************************************************/

#include "xmc_device.h"
#include "xmc_gpio.h"

#include "Driver_ETH_MAC.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if (defined(RTE_Drivers_ETH_MAC) && !RTE_ENET)
#error "Ethernet not configured in RTE_Device.h!"
#endif

#if (!RTE_ENET_MII && !RTE_ENET_RMII) || (RTE_ENET_MII && RTE_ENET_RMII)
#error "Ethernet interface configuration in RTE_Device.h is invalid!"
#endif

#include "cmsis_os.h"

/*******************************************************************************
 * MACROS
 *******************************************************************************/

#define ARM_ETH_MAC_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(2, 6) /* driver version */

/* Timeouts */
#define ETH_MAC_PHY_TIMEOUT         200         /* PHY Register access timeout in us  */

/* Receive/transmit Checksum offload enable */
#ifndef ETH_MAC_CHECKSUM_OFFLOAD
#define ETH_MAC_CHECKSUM_OFFLOAD   1
#endif

/* IEEE 1588 time stamping enable (PTP) */
#ifndef ETH_MAC_TIME_STAMP
#define ETH_MAC_TIME_STAMP         0
#endif

/* Memory Buffer configuration */
#define ETH_MAC_NUM_RX_BUF          4
#define ETH_MAC_NUM_TX_BUF          2
#define ETH_MAC_BUF_SIZE         1536        /* ETH Receive/Transmit buffer size   */

/* TDES0 Descriptor TX Packet Control/Status */
#define ETH_MAC_DMA_TDES0_OWN      (0x80000000U)      // Own bit 1=DMA, 0=CPU
#define ETH_MAC_DMA_TDES0_IC       (0x40000000U)      // Interrupt on completition
#define ETH_MAC_DMA_TDES0_LS       (0x20000000U)      // Last segment
#define ETH_MAC_DMA_TDES0_FS       (0x10000000U)      // First segment
#define ETH_MAC_DMA_TDES0_DC       (0x08000000U)      // Disable CRC
#define ETH_MAC_DMA_TDES0_DP       (0x04000000U)      // Disable pad
#define ETH_MAC_DMA_TDES0_TTSE     (0x02000000U)      // Transmit time stamp enable
#define ETH_MAC_DMA_TDES0_CIC      (0x00C00000U)      // Checksum insertion control
#define ETH_MAC_DMA_TDES0_TER      (0x00200000U)      // Transmit end of ring
#define ETH_MAC_DMA_TDES0_TCH      (0x00100000U)      // Second address chained
#define ETH_MAC_DMA_TDES0_TTSS     (0x00020000U)      // Transmit time stamp status
#define ETH_MAC_DMA_TDES0_IHE      (0x00010000U)      // IP header error
#define ETH_MAC_DMA_TDES0_ES       (0x00008000U)      // Error summary
#define ETH_MAC_DMA_TDES0_JT       (0x00004000U)      // Jabber timeout
#define ETH_MAC_DMA_TDES0_FF       (0x00002000U)      // Frame flushed
#define ETH_MAC_DMA_TDES0_IPE      (0x00001000U)      // IP payload error
#define ETH_MAC_DMA_TDES0_LOC      (0x00000800U)      // Loss of carrier
#define ETH_MAC_DMA_TDES0_NC       (0x00000400U)      // No carrier
#define ETH_MAC_DMA_TDES0_LC       (0x00000200U)      // Late collision
#define ETH_MAC_DMA_TDES0_EC       (0x00000100U)      // Excessive collision
#define ETH_MAC_DMA_TDES0_VF       (0x00000080U)      // VLAN frame
#define ETH_MAC_DMA_TDES0_CC       (0x00000078U)      // Collision count
#define ETH_MAC_DMA_TDES0_ED       (0x00000004U)      // Excessive deferral
#define ETH_MAC_DMA_TDES0_UF       (0x00000002U)      // Underflow error
#define ETH_MAC_DMA_TDES0_DB       (0x00000001U)      // Deferred bit

/* RDES0 Descriptor RX Packet Status */
#define ETH_MAC_DMA_RDES0_OWN      (0x80000000U)      // Own bit 1=DMA, 0=CPU
#define ETH_MAC_DMA_RDES0_AFM      (0x40000000U)      // Destination address filter fail
#define ETH_MAC_DMA_RDES0_FL       (0x3FFF0000U)      // Frame length mask
#define ETH_MAC_DMA_RDES0_ES       (0x00008000U)      // Error summary
#define ETH_MAC_DMA_RDES0_DE       (0x00004000U)      // Descriptor error
#define ETH_MAC_DMA_RDES0_SAF      (0x00002000U)      // Source address filter fail
#define ETH_MAC_DMA_RDES0_LE       (0x00001000U)      // Length error
#define ETH_MAC_DMA_RDES0_OE       (0x00000800U)      // Overflow error
#define ETH_MAC_DMA_RDES0_VLAN     (0x00000400U)      // VLAN tag
#define ETH_MAC_DMA_RDES0_FS       (0x00000200U)      // First descriptor
#define ETH_MAC_DMA_RDES0_LS       (0x00000100U)      // Last descriptor
#define ETH_MAC_DMA_RDES0_TSA      (0x00000080U)      // Timestamp available
#define ETH_MAC_DMA_RDES0_LC       (0x00000040U)      // Late collision
#define ETH_MAC_DMA_RDES0_FT       (0x00000020U)      // Frame type
#define ETH_MAC_DMA_RDES0_RWT      (0x00000010U)      // Receive watchdog timeout
#define ETH_MAC_DMA_RDES0_RE       (0x00000008U)      // Receive error
#define ETH_MAC_DMA_RDES0_DBE      (0x00000004U)      // Dribble bit error
#define ETH_MAC_DMA_RDES0_CE       (0x00000002U)      // CRC error
#define ETH_MAC_DMA_RDES0_ESA      (0x00000001U)      // Extended Status/Rx MAC address

/* RDES1 Descriptor RX Packet Control */
#define ETH_MAC_DMA_RDES1_RBS2     (0x1FFF0000U)      // Receive buffer 2 size
#define ETH_MAC_DMA_RDES1_RER      (0x00008000U)      // Receive end of ring
#define ETH_MAC_DMA_RDES1_RCH      (0x00004000U)      // Second address chained
#define ETH_MAC_DMA_RDES1_RBS1     (0x00001FFFU)      // Receive buffer 1 size

#define ETH_MAC_MDC_DIVIDER_16     (2U << ETH_GMII_ADDRESS_CR_Pos)
#define ETH_MAC_MDC_DIVIDER_26     (3U << ETH_GMII_ADDRESS_CR_Pos)
#define ETH_MAC_MDC_DIVIDER_42     (0U << ETH_GMII_ADDRESS_CR_Pos)
#define ETH_MAC_MDC_DIVIDER_62     (1U << ETH_GMII_ADDRESS_CR_Pos)
#define ETH_MAC_MDC_DIVIDER_102    (4U << ETH_GMII_ADDRESS_CR_Pos)
#define ETH_MAC_MDC_DIVIDER_124    (5U << ETH_GMII_ADDRESS_CR_Pos)

/* ETH MAC Driver state flags */
#define ETH_MAC_FLAG_INIT      (1U << 0)        // Driver initialized
#define ETH_MAC_FLAG_POWER     (1U << 1)        // Driver power on

/*******************************************************************************
 * DATA STRUCTURES
 *******************************************************************************/

/* DMA Descriptor */
typedef struct ETH_MAC_DMA_DESC {
  uint32_t status;
  uint32_t length;
  uint32_t buffer1;
  uint32_t buffer2;
#if ((ETH_MAC_CHECKSUM_OFFLOAD != 0) || (ETH_MAC_TIME_STAMP != 0))
  uint32_t extended_status;
  uint32_t reserved;
  uint32_t time_stamp_low;
  uint32_t time_stamp_high;
#endif    
} ETH_MAC_DMA_DESC_t;

typedef struct ETH_MAC_HANDLER {
  ARM_ETH_MAC_SignalEvent_t cb_event;   // Event callback
  uint8_t flags;                        // Control and state flags
  uint8_t tx_index;                     // Transmit descriptor index
  uint8_t rx_index;                     // Receive descriptor index
#if (ETH_MAC_CHECKSUM_OFFLOAD != 0)   
  bool tx_checksum_offload;
#endif
#if (ETH_MAC_TIME_STAMP != 0)
  uint8_t tx_ts_index;                  // Transmit Timestamp descriptor index
#endif
  uint8_t *frame_end;                   // End of assembled frame fragments
} ETH_MAC_HANDLER_t;


/*******************************************************************************
 * LOCAL VARIABLES
 *******************************************************************************/

static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_MAC_API_VERSION,
  ARM_ETH_MAC_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_ETH_MAC_CAPABILITIES DriverCapabilities = {
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_rx_ip4  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_rx_ip6  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_rx_udp  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_rx_tcp  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_rx_icmp */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_tx_ip4  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_tx_ip6  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_tx_udp  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_tx_tcp  */
  (ETH_MAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                        /* checksum_offload_tx_icmp */
  (RTE_ENET_RMII) ? ARM_ETH_INTERFACE_RMII : ARM_ETH_INTERFACE_MII, /* media_interface          */
  0,                                                                /* mac_address              */
  1,                                                                /* event_rx_frame           */
  1,                                                                /* event_tx_frame           */
  1,                                                                /* event_wakeup             */
  (ETH_MAC_TIME_STAMP) ? 1 : 0                                      /* precision_timer          */
};

static ETH_MAC_HANDLER_t handler = { 0 };

static __align(4) ETH_MAC_DMA_DESC_t rx_desc[ETH_MAC_NUM_RX_BUF] __attribute__((section ("RW_IRAM1")));
static __align(4) ETH_MAC_DMA_DESC_t tx_desc[ETH_MAC_NUM_TX_BUF] __attribute__((section ("RW_IRAM1")));
static __align(4) uint32_t rx_buf[ETH_MAC_NUM_RX_BUF][ETH_MAC_BUF_SIZE >> 2] __attribute__((section ("RW_IRAM1")));
static __align(4) uint32_t tx_buf[ETH_MAC_NUM_TX_BUF][ETH_MAC_BUF_SIZE >> 2] __attribute__((section ("RW_IRAM1")));

/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/

static void init_rx_desc(void) 
{
  uint32_t i;
  uint32_t next;
    
  /* Chained structure (ETH_MAC_DMA_RDES1_RCH), second address in the descriptor (buffer2) is the next descriptor address */
  for (i = 0U; i < ETH_MAC_NUM_RX_BUF; ++i) {
    rx_desc[i].status = ETH_MAC_DMA_RDES0_OWN;
    rx_desc[i].length = ETH_MAC_DMA_RDES1_RCH | ETH_MAC_BUF_SIZE;
    rx_desc[i].buffer1 = (uint32_t)&rx_buf[i];
    next = i + 1U;
    if (next == ETH_MAC_NUM_RX_BUF) {
        next = 0U;
    }
    rx_desc[i].buffer2 = (uint32_t)&rx_desc[next];
  }
  ETH0->RECEIVE_DESCRIPTOR_LIST_ADDRESS = (uint32_t)&rx_desc[0];
  handler.rx_index = 0U;
}

/**
  \fn          void init_tx_desc (void)
  \brief       Initialize Tx DMA descriptors.
*/
static void init_tx_desc(void) 
{
  uint32_t i;
  uint32_t next;

  /* Chained structure (ETH_MAC_DMA_TDES0_TCH), second address in the descriptor (buffer2) is the next descriptor address */
  for (i = 0U; i < ETH_MAC_NUM_TX_BUF; ++i) {
    tx_desc[i].status = ETH_MAC_DMA_TDES0_TCH | ETH_MAC_DMA_TDES0_LS | ETH_MAC_DMA_TDES0_FS;
    tx_desc[i].buffer1 = (uint32_t)&tx_buf[i];
    next = i + 1U;
    if (next == ETH_MAC_NUM_TX_BUF) {
        next = 0U;
    }
    tx_desc[i].buffer2 = (uint32_t)&tx_desc[next];
  }
  ETH0->TRANSMIT_DESCRIPTOR_LIST_ADDRESS = (uint32_t)&tx_desc[0];
  handler.tx_index = 0U;
}

static uint32_t crc32_8bit_rev(uint32_t crc32, uint8_t val) 
{
  uint32_t n;

  crc32 ^= __RBIT(val);
  for (n = 8; n; n--) {
    if (crc32 & 0x80000000) {
      crc32 <<= 1;
      crc32  ^= 0x04C11DB7;
    } else {
      crc32 <<= 1;
    }
  }
  return (crc32);
}

static uint32_t crc32_data(const uint8_t *data, uint32_t len) 
{
  uint32_t crc;

  for (crc = 0xFFFFFFFF; len; --len) {
    crc = crc32_8bit_rev(crc, *data++);
  }
  return (crc ^ 0xFFFFFFFF);
}

uint32_t GetEthernetClockFrequency(void)
{
  return (SystemCoreClock << ((SCU_CLK->CPUCLKCR & SCU_CLK_CPUCLKCR_CPUDIV_Msk) >> SCU_CLK_CPUCLKCR_CPUDIV_Pos)) >> 1U;
}

/*******************************************************************************
 * DRIVER IMPLEMENTATION
 *******************************************************************************/

static ARM_DRIVER_VERSION ETH_MAC_GetVersion(void)
{
  return DriverVersion;
}

static ARM_ETH_MAC_CAPABILITIES ETH_MAC_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ETH_MAC_Initialize(ARM_ETH_MAC_SignalEvent_t cb_event)
{
  XMC_GPIO_CONFIG_t gpio_config;
    
  if (handler.flags & ETH_MAC_FLAG_INIT) {
    /* Driver already initialized */
    return ARM_DRIVER_OK;
  }
  
  handler.flags = ETH_MAC_FLAG_INIT;

  /* Register driver callback function */
  handler.cb_event = cb_event;

  /* Configure ETH_MAC pins */
#if RTE_ENET_MII
  ETH0_CON->CON = (RTE_ENET_MII_RXD0_PIN_ID << ETH_CON_RXD0_Pos) |
                  (RTE_ENET_MII_RXD1_PIN_ID << ETH_CON_RXD1_Pos) |
                  (RTE_ENET_MII_RXD2_PIN_ID << ETH_CON_RXD2_Pos) |
                  (RTE_ENET_MII_RXD3_PIN_ID << ETH_CON_RXD3_Pos) |
                  (RTE_ENET_MII_RXCLK_PIN_ID << ETH_CON_CLK_RMII_Pos) |
                  (RTE_ENET_MII_RXDV_PIN_ID << ETH_CON_CRS_DV_Pos) |
                  (RTE_ENET_MII_CRS_PIN_ID << ETH_CON_CRS_Pos) |
                  (RTE_ENET_MII_RXER_PIN_ID << ETH_CON_RXER_Pos) |
                  (RTE_ENET_MII_COL_PIN_ID << ETH_CON_COL_Pos) |
                  (RTE_ENET_MII_TXCLK_PIN_ID << ETH_CON_CLK_TX_Pos) |
                  (RTE_ENET_MIIM_MDIO_PIN_ID << ETH_CON_MDIO_Pos);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(RTE_ENET_MII_RXD0_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXD1_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXD2_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXD3_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXCLK_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXDV_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_CRS_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_RXER_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_COL_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MII_TXCLK_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MIIM_MDIO_PIN, &gpio_config);
   
  gpio_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  
  gpio_config.mode = RTE_ENET_MII_TXD0_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXD0_PIN, &gpio_config);
  
  gpio_config.mode = RTE_ENET_MII_TXD1_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXD1_PIN, &gpio_config);

  gpio_config.mode = RTE_ENET_MII_TXD2_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXD2_PIN, &gpio_config);

  gpio_config.mode = RTE_ENET_MII_TXD3_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXD3_PIN, &gpio_config);
  
  gpio_config.mode = RTE_ENET_MII_TXEN_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXEN_PIN, &gpio_config);

  gpio_config.mode = RTE_ENET_MII_TXER_MODE;
  XMC_GPIO_Init(RTE_ENET_MII_TXER_PIN, &gpio_config);
  
  gpio_config.mode = RTE_ENET_MIIM_MDC_MODE;
  XMC_GPIO_Init(RTE_ENET_MIIM_MDC_PIN, &gpio_config);
  
  XMC_GPIO_SetHardwareControl(RTE_ENET_MIIM_MDIO_PIN, RTE_ENET_MIIM_MDIO_HWSEL);
                  
#else  
  ETH0_CON->CON = ETH_CON_INFSEL_Msk |
                  (RTE_ENET_RMII_RXD0_PIN_ID << ETH_CON_RXD0_Pos) |
                  (RTE_ENET_RMII_RXD1_PIN_ID << ETH_CON_RXD1_Pos) |
                  (RTE_ENET_RMII_REFCLK_PIN_ID << ETH_CON_CLK_RMII_Pos) |
                  (RTE_ENET_RMII_CRS_DV_PIN_ID << ETH_CON_CRS_DV_Pos) |
                  (RTE_ENET_RMII_RXER_PIN_ID << ETH_CON_RXER_Pos) |
                  (RTE_ENET_MIIM_MDIO_PIN_ID << ETH_CON_MDIO_Pos);  
    
  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(RTE_ENET_RMII_RXD0_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_RMII_RXD1_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_RMII_REFCLK_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_RMII_CRS_DV_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_RMII_RXER_PIN, &gpio_config);
  XMC_GPIO_Init(RTE_ENET_MIIM_MDIO_PIN, &gpio_config);
   
  gpio_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  
  gpio_config.mode = RTE_ENET_RMII_TXD0_MODE;
  XMC_GPIO_Init(RTE_ENET_RMII_TXD0_PIN, &gpio_config);
  
  gpio_config.mode = RTE_ENET_RMII_TXD1_MODE;
  XMC_GPIO_Init(RTE_ENET_RMII_TXD1_PIN, &gpio_config);

  gpio_config.mode = RTE_ENET_RMII_TXEN_MODE;
  XMC_GPIO_Init(RTE_ENET_RMII_TXEN_PIN, &gpio_config);
  
  gpio_config.mode = RTE_ENET_MIIM_MDC_MODE;
  XMC_GPIO_Init(RTE_ENET_MIIM_MDC_PIN, &gpio_config);
  
  XMC_GPIO_SetHardwareControl(RTE_ENET_MIIM_MDIO_PIN, RTE_ENET_MIIM_MDIO_HWSEL);
#endif
    
  return ARM_DRIVER_OK;
}

static int32_t ETH_MAC_Uninitialize(void)
{
  handler.flags &= ~ETH_MAC_FLAG_INIT;

  return ARM_DRIVER_OK;
}

static int32_t ETH_MAC_PowerControl(ARM_POWER_STATE state)
{
  uint32_t eth_clk;
  
  switch (state)
  {
    case ARM_POWER_OFF:
      handler.flags &= ~ETH_MAC_FLAG_POWER;

      /* Disable EMAC interrupts */
      NVIC_DisableIRQ(ETH0_0_IRQn);
      ETH0->INTERRUPT_ENABLE = 0x0U;
      ETH0->TIMESTAMP_CONTROL = 0x0U;

      /* Disable DMA Tx and Rx */
      ETH0->OPERATION_MODE &= ~(ETH_OPERATION_MODE_ST_Msk | ETH_OPERATION_MODE_SR_Msk);

      /* Flush transmit FIFO */
      ETH0->OPERATION_MODE |= ETH_OPERATION_MODE_FTF_Msk;

      /* Disable ETH_MAC Tx and Rx */
      ETH0->MAC_CONFIGURATION = 0x0U;
      ETH0->OPERATION_MODE = 0x0U;

      /* Disable ETH_MAC peripheral clock */
      SCU_CLK->CLKSET &= ~SCU_CLK_CLKSET_ETH0CEN_Msk;
        
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if (handler.flags & ETH_MAC_FLAG_POWER) {
        /* Driver already powered */
        break;
      }
      
      if ((handler.flags & ETH_MAC_FLAG_INIT) == 0) {
        return ARM_DRIVER_ERROR;
      }
            
      /* Enable ETH_MAC peripheral clock */
      SCU_CLK->CLKSET |= SCU_CLK_CLKSET_ETH0CEN_Msk;

#if UC_DEVICE != XMC4500
      SCU_CLK->CGATCLR2 |= SCU_CLK_CGATCLR2_ETH0_Msk;     
#endif
      
      /* Reset ETH_MAC peripheral */
      SCU_RESET->PRSET2 |= SCU_RESET_PRSET2_ETH0RS_Msk;
      SCU_RESET->PRCLR2 |= SCU_RESET_PRCLR2_ETH0RS_Msk;
      
      /* Soft reset ETH_MAC DMA controller */
      ETH0->BUS_MODE |= ETH_BUS_MODE_SWR_Msk;
      while ((ETH0->BUS_MODE & ETH_BUS_MODE_SWR_Msk) != 0U);
            
      eth_clk = GetEthernetClockFrequency();
      if (eth_clk <= 35000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_16;
      }
      else if (eth_clk <= 60000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_26;
      }
      else if (eth_clk <= 100000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_42;
      }
      else if (eth_clk <= 150000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_62;
      }
      else if (eth_clk <= 200000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_102;
      }
      else if (eth_clk <= 250000000U) {
        ETH0->GMII_ADDRESS = ETH_MAC_MDC_DIVIDER_124;
      }
      else {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      
      /* Initialize MAC configuration */
      ETH0->MAC_CONFIGURATION = ETH_MAC_CONFIGURATION_DO_Msk; /* Disable Receive Own in half duplex*/

      /* Initialize Filter registers */
      ETH0->MAC_FRAME_FILTER = ETH_MAC_FRAME_FILTER_DBF_Msk;  /* Disable broadcast frames */
      ETH0->FLOW_CONTROL = ETH_FLOW_CONTROL_DZPQ_Msk; /* Disable Zero Quanta Pause */

      /* Initialize Address register */
      ETH0->MAC_ADDRESS0_HIGH = 0x0U;
      ETH0->MAC_ADDRESS0_LOW = 0x0U;

      ETH0->MAC_ADDRESS1_HIGH = 0x0U;
      ETH0->MAC_ADDRESS1_LOW = 0x0U;

      ETH0->MAC_ADDRESS2_HIGH = 0x0U;
      ETH0->MAC_ADDRESS2_LOW = 0x0U;

      ETH0->MAC_ADDRESS3_HIGH = 0x0U;
      ETH0->MAC_ADDRESS3_LOW = 0x0U;

      /* Disable MAC interrupts */
      ETH0->INTERRUPT_MASK = ETH_INTERRUPT_MASK_PMTIM_Msk | ETH_INTERRUPT_MASK_TSIM_Msk;

#if (ETH_MAC_TIME_STAMP != 0)
      /* Enable timestamp fine update */
      ETH0->TIMESTAMP_CONTROL = ETH_TIMESTAMP_CONTROL_TSIPV4ENA_Msk | 
                                ETH_TIMESTAMP_CONTROL_TSIPV6ENA_Msk |
                                ETH_TIMESTAMP_CONTROL_TSCTRLSSR_Msk | 
                                ETH_TIMESTAMP_CONTROL_TSADDREG_Msk |
                                ETH_TIMESTAMP_CONTROL_TSCFUPDT_Msk | 
                                ETH_TIMESTAMP_CONTROL_TSENA_Msk;
      
      handler.tx_ts_index = 0;
#endif
      ETH0->OPERATION_MODE = ETH_OPERATION_MODE_OSF_Msk;

      ETH0->BUS_MODE = ETH_BUS_MODE_AAL_Msk | /* the AHB interface generates all bursts aligned to the start address LS bits */
                       ETH_BUS_MODE_FB_Msk | /* DMA attempts to execute fixed-length Burst transfers on the AHB Master interface */
                       (0x20 << ETH_BUS_MODE_PBL_Pos); /* maximum Burst length */

#if ((ETH0_CHECKSUM_OFFLOAD != 0) || (ETH0_TIME_STAMP != 0))
      /* Enhanced descriptor enable */
      ETH0->BUS_MODE |= ETH_BUS_MODE_ATDS_Msk;
#endif
      
      /* Initialize DMA Descriptors */
      init_rx_desc();
      init_tx_desc();
    
      /* Clear and enable DMA interrupts */
      ETH0->STATUS = 0xFFFFFFFF;
      
      /* Disable MMC interrupt events */
      ETH0->MMC_TRANSMIT_INTERRUPT_MASK = 0x03ffffffU;
      ETH0->MMC_RECEIVE_INTERRUPT_MASK = 0x03ffffffU;
      ETH0->MMC_IPC_RECEIVE_INTERRUPT_MASK = 0x3fff3fffU;

      /* Enable ethernet interrupts */
      ETH0->INTERRUPT_ENABLE = ETH_INTERRUPT_ENABLE_NIE_Msk | ETH_INTERRUPT_ENABLE_RIE_Msk | ETH_INTERRUPT_ENABLE_TIE_Msk;

      NVIC_ClearPendingIRQ(ETH0_0_IRQn);
      NVIC_EnableIRQ(ETH0_0_IRQn);

      handler.frame_end = NULL;
      handler.flags |= ETH_MAC_FLAG_POWER;
               
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  
  return ARM_DRIVER_OK;
}

static int32_t ETH_MAC_GetMacAddress(ARM_ETH_MAC_ADDR *const ptr_addr)
{
  uint32_t val;

  if (ptr_addr == NULL) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  val = ETH0->MAC_ADDRESS0_HIGH;
  ptr_addr->b[5] = (uint8_t)(val >> 8);
  ptr_addr->b[4] = (uint8_t)(val);
  val = ETH0->MAC_ADDRESS0_LOW;
  ptr_addr->b[3] = (uint8_t)(val >> 24);
  ptr_addr->b[2] = (uint8_t)(val >> 16);
  ptr_addr->b[1] = (uint8_t)(val >>  8);
  ptr_addr->b[0] = (uint8_t)(val);

  return ARM_DRIVER_OK;  
}

static int32_t ETH_MAC_SetMacAddress(const ARM_ETH_MAC_ADDR *const ptr_addr)
{
  if (ptr_addr == NULL) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  /* Set Ethernet MAC Address registers */
  ETH0->MAC_ADDRESS0_HIGH = (ptr_addr->b[5] <<  8) |  ptr_addr->b[4];
  ETH0->MAC_ADDRESS0_LOW  = (ptr_addr->b[3] << 24) | (ptr_addr->b[2] << 16) |
                            (ptr_addr->b[1] <<  8) |  ptr_addr->b[0];
  return ARM_DRIVER_OK;
}

static int32_t ETH_MAC_SetAddressFilter(const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr)
{
  uint32_t crc;

  if ((ptr_addr == NULL) && (num_addr > 0)) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  /* Use unicast address filtering for first 3 MAC addresses */
  ETH0->MAC_FRAME_FILTER &= ~(ETH_MAC_FRAME_FILTER_HPF_Msk | ETH_MAC_FRAME_FILTER_HMC_Msk);
  ETH0->HASH_TABLE_HIGH = 0x00000000;
  ETH0->HASH_TABLE_LOW  = 0x00000000;

  ETH0->MAC_ADDRESS1_HIGH = 0x0U;
  ETH0->MAC_ADDRESS1_LOW = 0x0U;

  ETH0->MAC_ADDRESS2_HIGH = 0x0U;
  ETH0->MAC_ADDRESS2_LOW = 0x0U;

  ETH0->MAC_ADDRESS3_HIGH = 0x0U;
  ETH0->MAC_ADDRESS3_LOW = 0x0U;

  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }

  ETH0->MAC_ADDRESS1_HIGH = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MAC_ADDRESS1_HIGH_AE_Msk;
  ETH0->MAC_ADDRESS1_LOW = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                           ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH0->MAC_ADDRESS2_HIGH = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MAC_ADDRESS2_HIGH_AE_Msk;
  ETH0->MAC_ADDRESS2_HIGH = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                            ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH0->MAC_ADDRESS3_HIGH = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MAC_ADDRESS3_HIGH_AE_Msk;
  ETH0->MAC_ADDRESS3_HIGH = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                            ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }
  ptr_addr++;
    
  /* Calculate 64-bit Hash table for MAC addresses */
  for ( ; num_addr != 0; ptr_addr++, num_addr--) {
    crc = crc32_data(&ptr_addr->b[0], 6) >> 26;
    if (crc & 0x20) {
      ETH0->HASH_TABLE_HIGH |= (1 << (crc & 0x1F));
    }
    else {
      ETH0->HASH_TABLE_LOW  |= (1 << crc);
    }
  }
  /* Enable both, unicast perfect and hash multicast address filtering */
  ETH0->MAC_FRAME_FILTER |= ETH_MAC_FRAME_FILTER_HPF_Msk | ETH_MAC_FRAME_FILTER_HMC_Msk;

  return ARM_DRIVER_OK;
}

static int32_t ETH_MAC_SendFrame(const uint8_t *frame, uint32_t len, uint32_t flags)
{
  static uint8_t *frame_end = NULL;
  uint8_t *dst = frame_end;
  uint32_t ctrl;
    
  if ((frame == NULL) && (len != 0U)) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }
  
  if (dst == NULL) {
    /* Start of a new transmit frame */
    if (tx_desc[handler.tx_index].status & ETH_MAC_DMA_TDES0_OWN) {
      /* Transmitter is busy, wait */
      return ARM_DRIVER_ERROR_BUSY;
    }
    dst = (uint8_t *)tx_desc[handler.tx_index].buffer1;
    tx_desc[handler.tx_index].length = len;
  }
  else {
    /* Sending data fragments in progress */
    tx_desc[handler.tx_index].length += len;
  }
  /* Fast-copy data fragments to ETH_MAC DMA buffer */
  for ( ; len > 7U; dst += 8, frame += 8, len -= 8U) {
    ((__packed uint32_t *)dst)[0] = ((__packed uint32_t *)frame)[0];
    ((__packed uint32_t *)dst)[1] = ((__packed uint32_t *)frame)[1];
  }
  /* Copy remaining 7 bytes */
  for ( ; len > 1U; dst += 2, frame += 2, len -= 2U) {
    ((__packed uint16_t *)dst)[0] = ((__packed uint16_t *)frame)[0];
  }
  if (len > 0U) { dst++[0] = frame++[0]; }

  if (flags & ARM_ETH_MAC_TX_FRAME_FRAGMENT) {
    /* More data to come, remember current write position */
    handler.frame_end = dst;
    return ARM_DRIVER_OK;
  }

  /* Frame is now ready, send it to DMA */
  ctrl = tx_desc[handler.tx_index].status & ~ETH_MAC_DMA_TDES0_CIC;
#if (ETH_MAC_CHECKSUM_OFFLOAD != 0)
  if (handler.tx_checksum_offload) { 
     ctrl |= ETH_MAC_DMA_TDES0_CIC; 
  }
#endif
  ctrl &= ~(ETH_MAC_DMA_TDES0_IC | ETH_MAC_DMA_TDES0_TTSE);
  if (flags & ARM_ETH_MAC_TX_FRAME_EVENT) { 
    ctrl |= ETH_MAC_DMA_TDES0_IC; 
  }
#if (EMAC_TIME_STAMP != 0)
  if (flags & ARM_ETH_MAC_TX_FRAME_TIMESTAMP) { 
    ctrl |= ETH_MAC_DMA_TDES0_TTSE; 
  }
  handler.tx_ts_index = handler.tx_index;
#endif
  tx_desc[handler.tx_index].status = ctrl | ETH_MAC_DMA_TDES0_OWN;

  handler.tx_index++;
  if (handler.tx_index == ETH_MAC_NUM_TX_BUF) { 
    handler.tx_index = 0U; 
  }
  handler.frame_end = NULL;

  /* Start frame transmission */
  ETH0->STATUS = ETH_STATUS_TPS_Msk;
  ETH0->TRANSMIT_POLL_DEMAND = 0U;

  return ARM_DRIVER_OK;  
}

static int32_t ETH_MAC_ReadFrame(uint8_t *frame, uint32_t len)
{
  uint8_t const *src;

  if ((frame == NULL) || (len == 0U)) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  /* Fast-copy data to packet buffer */
  src = (uint8_t const *)rx_desc[handler.rx_index].buffer1;
  
  for ( ; len > 7; frame += 8, src += 8, len -= 8) {
    ((__packed uint32_t *)frame)[0] = ((uint32_t *)src)[0];
    ((__packed uint32_t *)frame)[1] = ((uint32_t *)src)[1];
  }
  /* Copy remaining 7 bytes */
  for ( ; len > 1; frame += 2, src += 2, len -= 2) {
    ((__packed uint16_t *)frame)[0] = ((uint16_t *)src)[0];
  }
  if (len > 0) frame[0] = src[0];

  /* Return this block back to EMAC-DMA */
  rx_desc[handler.rx_index].status = ETH_MAC_DMA_RDES0_OWN;

  handler.rx_index++;
  if (handler.rx_index == ETH_MAC_NUM_RX_BUF) {
      handler.rx_index = 0;
  }

  if (ETH0->STATUS & ETH_STATUS_RU_Msk) {
    /* Receive buffer unavailable, resume DMA */
    ETH0->STATUS = ETH_STATUS_RU_Msk;
    ETH0->RECEIVE_POLL_DEMAND = 0;
  }
  return (len);    
}

static uint32_t ETH_MAC_GetRxFrameSize(void)
{
  uint32_t status;

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not yet powered */
    return (0);
  }
    
  status = rx_desc[handler.rx_index].status;

  if (status & ETH_MAC_DMA_RDES0_OWN) {
    /* Owned by DMA */
    return (0U);
  }
  
  if (((status & ETH_MAC_DMA_RDES0_ES) != 0) ||
      ((status & ETH_MAC_DMA_RDES0_FS) == 0) ||
      ((status & ETH_MAC_DMA_RDES0_LS) == 0)) {
    /* Error, this block is invalid */
    return (0xFFFFFFFFU);
  }
      
  return (((status & ETH_MAC_DMA_RDES0_FL) >> 16) - 4U);
    
}

static int32_t ETH_MAC_GetRxFrameTime(ARM_ETH_MAC_TIME *time)
{
#if (ETH_MAC_TIME_STAMP)
  ETH_MAC_DMA_DESC_t *rxd;

  if (time == NULL) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }
     
  rxd = &rx_desc[handler.rx_index];

  if (rxd->status & ETH_MAC_DMA_RDES0_OWN) {
    /* Owned by DMA */
    return ARM_DRIVER_ERROR_BUSY;
  }
  time->ns = rxd->time_stamp_low;
  time->sec = rxd->time_stamp_high;

  return ARM_DRIVER_OK;
#else
  return ARM_DRIVER_ERROR;
#endif    
}

static int32_t ETH_MAC_GetTxFrameTime(ARM_ETH_MAC_TIME *time)
{
#if (ETH_MAC_TIME_STAMP)
  ETH_MAC_DMA_DESC_t *txd;

  txd = &tx_desc[handler.tx_ts_index];

  if (txd->status & ETH_MAC_DMA_TDES0_OWN) {
    /* Owned by DMA */
    return ARM_DRIVER_ERROR_BUSY;
  }
  
  if ((txd->status & ETH_MAC_DMA_TDES0_TTSS) == 0) {
    /* No transmit time stamp available */
    return ARM_DRIVER_ERROR;
  }
  time->ns  = txd->time_stamp_low;
  time->sec = txd->time_stamp_high;
  return ARM_DRIVER_OK;
#else
  return ARM_DRIVER_ERROR;
#endif   
}

static int32_t ETH_MAC_Control(uint32_t control, uint32_t arg)
{
  uint32_t maccr;
  uint32_t dmaomr;
  uint32_t macffr;
    
  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }
    
  switch (control)
  {
    case ARM_ETH_MAC_CONFIGURE:
      maccr = ETH0->MAC_CONFIGURATION & ~(ETH_MAC_CONFIGURATION_FES_Msk | 
                                          ETH_MAC_CONFIGURATION_DM_Msk   |
                                          ETH_MAC_CONFIGURATION_LM_Msk  | 
                                          ETH_MAC_CONFIGURATION_IPC_Msk);


      switch (arg & ARM_ETH_MAC_SPEED_Msk)
      {
        case ARM_ETH_MAC_SPEED_10M:
#if (RTE_ENET_RMII)
          /* RMII Half Duplex Collision detection does not work */
          maccr |= ETH_MAC_CONFIGURATION_DM_Msk;
#endif
          break;
                
        case ARM_ETH_SPEED_100M:
          maccr |= ETH_MAC_CONFIGURATION_FES_Msk;
          break;
                
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      switch (arg & ARM_ETH_MAC_DUPLEX_Msk)
      {
        case ARM_ETH_MAC_DUPLEX_FULL:
          maccr |= ETH_MAC_CONFIGURATION_DM_Msk;
          break;
      }

      if (arg & ARM_ETH_MAC_LOOPBACK)
      {
        maccr |= ETH_MAC_CONFIGURATION_LM_Msk;
      }

      dmaomr = ETH0->OPERATION_MODE & ~(ETH_OPERATION_MODE_RSF_Msk| ETH_OPERATION_MODE_TSF_Msk);
#if (ETH_MAC_CHECKSUM_OFFLOAD != 0)
      /* Enable rx checksum verification */
      if (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) {
        maccr  |= ETH_MAC_CONFIGURATION_IPC_Msk;
        dmaomr |= ETH_OPERATION_MODE_RSF_Msk;
      }

      /* Enable tx checksum generation */
      handler.tx_checksum_offload = false;
      if (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX) {
        dmaomr |= ETH_OPERATION_MODE_TSF_Msk;
        handler.tx_checksum_offload = true;
      }
#else
      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) ||
          (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX)) {
        /* Checksum offload is disabled in the driver */
        return ARM_DRIVER_ERROR;
      }
#endif
      ETH0->OPERATION_MODE = dmaomr;
      ETH0->MAC_CONFIGURATION = maccr;

      macffr = ETH0->MAC_FRAME_FILTER & ~(ETH_MAC_FRAME_FILTER_PR_Msk | 
                                          ETH_MAC_FRAME_FILTER_PM_Msk | 
                                          ETH_MAC_FRAME_FILTER_DBF_Msk);
      
      /* Enable broadcast frame receive */
      if ((arg & ARM_ETH_MAC_ADDRESS_BROADCAST) == 0) {
        macffr |= ETH_MAC_FRAME_FILTER_DBF_Msk;
      }

      /* Enable all multicast frame receive */
      if (arg & ARM_ETH_MAC_ADDRESS_MULTICAST) {
        macffr |= ETH_MAC_FRAME_FILTER_PM_Msk;
      }

      /* Enable promiscuous mode (no filtering) */
      if (arg & ARM_ETH_MAC_ADDRESS_ALL) {
        macffr |= ETH_MAC_FRAME_FILTER_PR_Msk;
      }
      ETH0->MAC_FRAME_FILTER = macffr;

      break;

    case ARM_ETH_MAC_CONTROL_TX:
      /* Enable/disable MAC transmitter */
      if (arg != 0) {
        ETH0->MAC_CONFIGURATION |= ETH_MAC_CONFIGURATION_TE_Msk;
        ETH0->OPERATION_MODE |= ETH_OPERATION_MODE_ST_Msk;
      }
      else {
        ETH0->MAC_CONFIGURATION &= ~ETH_MAC_CONFIGURATION_TE_Msk;
        ETH0->OPERATION_MODE &= ~ETH_OPERATION_MODE_ST_Msk;
      }        
      break;

    case ARM_ETH_MAC_CONTROL_RX:
      /* Enable/disable MAC receiver */
      if (arg != 0) {
        ETH0->MAC_CONFIGURATION |= ETH_MAC_CONFIGURATION_RE_Msk;
        ETH0->OPERATION_MODE |= ETH_OPERATION_MODE_SR_Msk;
      }
      else {
        ETH0->MAC_CONFIGURATION &= ~ETH_MAC_CONFIGURATION_RE_Msk;
        ETH0->OPERATION_MODE &= ~ETH_OPERATION_MODE_SR_Msk;
      }
      break;

    case ARM_ETH_MAC_FLUSH:
      /* Flush Tx and Rx buffers */
      if (arg & ARM_ETH_MAC_FLUSH_RX) {
        /* Stop/Start DMA Receive */
        ETH0->OPERATION_MODE &= ~ETH_OPERATION_MODE_SR_Msk;
        init_rx_desc();
        ETH0->OPERATION_MODE |= ETH_OPERATION_MODE_SR_Msk;
      }

      if (arg & ARM_ETH_MAC_FLUSH_TX) {
        /* Stop/Start DMA Transmit */
        ETH0->OPERATION_MODE &= ~ETH_OPERATION_MODE_ST_Msk;
        init_tx_desc();
        ETH0->OPERATION_MODE |= ETH_OPERATION_MODE_ST_Msk;
      }
      break;

    case ARM_ETH_MAC_SLEEP:
      /* Enable/disable Sleep mode */
      if (arg != 0) {
        /* Enable Power Management interrupts */
        ETH0->INTERRUPT_MASK  &= ~ETH_INTERRUPT_MASK_PMTIM_Msk;
          
        /* Enter Power-down, Magic packet enable */
        ETH0->PMT_CONTROL_STATUS = ETH_PMT_CONTROL_STATUS_MGKPKTEN_Msk | ETH_PMT_CONTROL_STATUS_PWRDWN_Msk;
      }
      else {
        /* Disable Power Management interrupts */
        ETH0->INTERRUPT_MASK |= ETH_INTERRUPT_MASK_PMTIM_Msk;
        ETH0->PMT_CONTROL_STATUS = 0x0U;
      }
      break;

    case ARM_ETH_MAC_VLAN_FILTER:
      ETH0->VLAN_TAG = arg;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  
  return ARM_DRIVER_OK;  
}

static int32_t ETH_MAC_ControlTimer(uint32_t control, ARM_ETH_MAC_TIME *time)
{
#if (ETH_MAC_TIME_STAMP != 0)
  if (!(emac.flags & EMAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }
  
  switch (control) {
    case ARM_ETH_MAC_TIMER_GET_TIME:
      /* Get current time */
      time->sec = ETH0->SYSTEM_TIME_SECONDS;
      time->ns = ETH0->SYSTEM_TIME_NANOSECONDS;
      break;

    case ARM_ETH_MAC_TIMER_SET_TIME:
      /* Set new time */
      ETH0->SYSTEM_TIME_SECONDS_UPDATE = time->sec;
      ETH0->SYSTEM_TIME_NANOSECONDS_UPDATE = time->ns;
      /* Initialize precision timer */
      ETH0->TIMESTAMP_CONTROL |= ETH_TIMESTAMP_CONTROL_TSINIT_Msk;
      break;

    case ARM_ETH_MAC_TIMER_INC_TIME:
      /* Increment current time */
      ETH0->SYSTEM_TIME_SECONDS_UPDATE = time->sec;
      ETH0->SYSTEM_TIME_NANOSECONDS_UPDATE = time->ns;
      /* Update precision timer */
      ETH0->TIMESTAMP_CONTROL |=  ETH_TIMESTAMP_CONTROL_TSUPDT_Msk;
      break;

    case ARM_ETH_MAC_TIMER_DEC_TIME:
      /* Decrement current time */
      ETH0->SYSTEM_TIME_SECONDS_UPDATE = time->sec;
      ETH0->SYSTEM_TIME_NANOSECONDS_UPDATE = time->ns | 0x80000000;
      /* Update precision timer */
      ETH0->TIMESTAMP_CONTROL |=  ETH_TIMESTAMP_CONTROL_TSUPDT_Msk;
      break;

    case ARM_ETH_MAC_TIMER_SET_ALARM:
      /* Set alarm time */
      ETH0->TARGET_TIME_SECONDS = time->sec;
      ETH0->TARGET_TIME_NANOSECONDS = time->ns;
      /* Enable timestamp interrupt trigger */
      ETH0->TIMESTAMP_CONTROL |= ETH_TIMESTAMP_CONTROL_TSTRIG_Msk;
      if (time->sec || time->ns) {
        /* Enable timestamp interrupts */
        ETH0->INTERRUPT_MASK &= ~ETH_INTERRUPT_MASK_TSIM_Msk;
        break;
      }
      /* Disable timestamp interrupts */
      ETH0->INTERRUPT_MASK |= ETH_INTERRUPT_MASK_TSIM_Msk;
      break;

    case ARM_ETH_MAC_TIMER_ADJUST_CLOCK:
      /* Adjust current time, fine correction */
      /* Correction factor is Q31 (0x80000000 = 1.000000000) */
      ETH0->TIMESTAMP_ADDEND = ((uint64_t)time->ns * ETH0->TIMESTAMP_ADDEND) >> 31;
      /* Update addend register */
      ETH0->TIMESTAMP_CONTROL |= ETH_TIMESTAMP_CONTROL_TSADDREG_Msk;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    
#else
  return ARM_DRIVER_ERROR;
#endif    
}

static int32_t ETH_MAC_PHY_Read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
  uint32_t tick;

  if (!data) {
    /* Invalid parameter */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  ETH0->GMII_ADDRESS = (ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_CR_Msk) |
                       ETH_GMII_ADDRESS_MB_Msk |
                       (phy_addr << ETH_GMII_ADDRESS_PA_Pos) |
                       (reg_addr << ETH_GMII_ADDRESS_MR_Pos);
                       
  /* Poll busy bit during max PHY_TIMEOUT time */                       
  tick = osKernelSysTick();
  do {
    if ((ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_MB_Msk) == 0U) {
      *data = (uint16_t)(ETH0->GMII_DATA & ETH_GMII_DATA_MD_Msk);
      return ARM_DRIVER_OK;
    }
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(ETH_MAC_PHY_TIMEOUT));

  return ARM_DRIVER_ERROR_TIMEOUT;
}

static int32_t ETH_MAC_PHY_Write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
  uint32_t tick;

  if (!(handler.flags & ETH_MAC_FLAG_POWER)) {
    /* Driver not powered */
    return ARM_DRIVER_ERROR;
  }

  ETH0->GMII_DATA  = data;
  ETH0->GMII_ADDRESS = (ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_CR_Msk) |
                       ETH_GMII_ADDRESS_MB_Msk |
                       ETH_GMII_ADDRESS_MW_Msk |
                       (phy_addr << ETH_GMII_ADDRESS_PA_Pos) |
                       (reg_addr << ETH_GMII_ADDRESS_MR_Pos);

   /* Poll busy bit during max PHY_TIMEOUT time */                       
  tick = osKernelSysTick();
  do {
    if ((ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_MB_Msk) == 0U) {
      return ARM_DRIVER_OK;
    }
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(ETH_MAC_PHY_TIMEOUT));

  return ARM_DRIVER_ERROR_TIMEOUT;
}

void ETH0_0_IRQHandler(void) 
{
  uint32_t status;
  uint32_t event = 0;

  status = ETH0->STATUS;
  ETH0->STATUS = status & (ETH_STATUS_NIS_Msk | ETH_STATUS_RI_Msk | ETH_STATUS_TI_Msk);
  
  if (status & ETH_STATUS_TI_Msk) {
    /* Transmit interrupt */
    event |= ARM_ETH_MAC_EVENT_TX_FRAME;
  }
  
  if (status & ETH_STATUS_RI_Msk) {
    /* Receive interrupt */
    event |= ARM_ETH_MAC_EVENT_RX_FRAME;
  }
  
  status = ETH0->INTERRUPT_STATUS;
#if (ETH_MAC_TIME_STAMP != 0)
  if (status & ETH_INTERRUPT_STATUS_TSIS_Msk) { 
    /* Timestamp interrupt */
    if (ETH0->TIMESTAMP_STATUS & ETH_TIMESTAMP_STATUS_TSTARGT_Msk) {
      /* Alarm trigger interrupt */
      event |= ARM_ETH_MAC_EVENT_TIMER_ALARM;
    }
  }
#endif
  if (status & ETH_INTERRUPT_STATUS_PMTIS_Msk) {
    /* Power management interrupt */
    if (ETH0->PMT_CONTROL_STATUS & ETH_PMT_CONTROL_STATUS_MGKPRCVD_Msk) {
      /* Magic packet received */
      event |= ARM_ETH_MAC_EVENT_WAKEUP;
    }
  }
  
  /* Callback event notification */
  if ((event != 0) && (handler.cb_event != NULL)) {
    handler.cb_event(event);
  }
}

ARM_DRIVER_ETH_MAC Driver_ETH_MAC0 =
{
    ETH_MAC_GetVersion,
    ETH_MAC_GetCapabilities,
    ETH_MAC_Initialize,
    ETH_MAC_Uninitialize,
    ETH_MAC_PowerControl,
    ETH_MAC_GetMacAddress,
    ETH_MAC_SetMacAddress,
    ETH_MAC_SetAddressFilter,
    ETH_MAC_SendFrame,
    ETH_MAC_ReadFrame,
    ETH_MAC_GetRxFrameSize,
    ETH_MAC_GetRxFrameTime,
    ETH_MAC_GetTxFrameTime,
    ETH_MAC_ControlTimer,
    ETH_MAC_Control,
    ETH_MAC_PHY_Read,
    ETH_MAC_PHY_Write
};

