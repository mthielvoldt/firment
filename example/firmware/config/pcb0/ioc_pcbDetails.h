/**
 * To prepare a new Interrupt-on-Change pin (IOC) for use:
 * 1. associate the new IOC to its purpose with a descriptive name.
 * 2. insert this new IOC into AVAILABLE_IOCs
 * 3. #define IOCn_INDEX corresponding to the position of the new IOC in
 *    AVAILABLE_IOCs.
 */
#ifndef ioc_pcbDetails_h
#define ioc_pcbDetails_h

#include <ioc_mcuDetails.h>

#define MSG_WAIT_IOC_ID 0
#define MSG_WAIT_IOC_OUT IOC_14_to_ISR_3

#define CTS_IOC_ID 1
#define CTS_IOC_OUT IOC_4_to_ISR_2

/* Interrupt-on-Change pins.  See ioc_mcuDetails.h for your target hardware.*/
/* Note: IOC_n_RTE_INDEX should match position of IOC_n in AVAILABLE_IOCs */
#define AVAILABLE_IOCs {       \
    [MSG_WAIT_IOC_ID] = IOC_4, \
    [CTS_IOC_ID] = IOC_14,     \
}

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

#endif // include once