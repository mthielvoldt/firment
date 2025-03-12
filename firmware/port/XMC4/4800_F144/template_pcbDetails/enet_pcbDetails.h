/** Provides project-specific config for CMSIS (ARM) Ethernet Driver.
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

#ifndef enet_pcbDetails_h
#define enet_pcbDetails_h


#define RTE_ENET 0

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

#endif // include once ifdef guard
