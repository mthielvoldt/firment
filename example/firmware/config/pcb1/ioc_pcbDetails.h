/**
 * To prepare a new Interrupt-on-Change pin (IOC) for use: 
 * 1. associate the new IOC to its purpose with a descriptive name.
 * 2. insert this new IOC into AVAILABLE_IOCs
 * 3. #define IOCn_INDEX corresponding to the position of the new IOC in 
 *    AVAILABLE_IOCs.
 */
#ifndef ioc_pcbDetails_h
#define ioc_pcbDetails_h


/* Interrupt-on-Change pins.  See ioc_mcuDetails.h for your target hardware.*/
/* Note: IOC_n_RTE_INDEX should match position of IOC_n in AVAILABLE_IOCs */
#define AVAILABLE_IOCs {IOC_B1, IOC_B2}
#define IOC_B1_RTE_INDEX 0
#define IOC_B2_RTE_INDEX 1

// clearToSend OUTPUT 2
// msgWaiting OUTPUT 3
#define IOC_USE_EXTI0 0
#define IOC_USE_EXTI1 0
#define IOC_USE_EXTI2 1
#define IOC_USE_EXTI3 1
#define IOC_USE_EXTI4 0
#define IOC_USE_EXTI9_5 0
#define IOC_USE_EXTI15_10 0

#endif // include once