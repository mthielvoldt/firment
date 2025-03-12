/** Provides project-specific config for CMSIS (ARM) CAN Driver.
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

 #ifndef can_pcbDetails_h
 #define can_pcbDetails_h

#define RTE_CAN0 0
#define RTE_CAN1 0
#define RTE_CAN2 0
#define RTE_CAN3 0
#define RTE_CAN4 0
#define RTE_CAN5 0

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

#endif // include once ifdef guard