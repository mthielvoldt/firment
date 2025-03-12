/** Provides project-specific config for CMSIS (ARM) USB Driver.
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

#ifndef usb_pcbDetails_h
#define usb_pcbDetails_h


#define RTE_USB_HOST_ID 0 // valid range: 0-1


#endif // RTE_DeviceConfig_H