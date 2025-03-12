/**
 * This MCU-specific file stands in for a real ioc_mcuDetails for an actual MCU.
 * It must provide definitions for the macros that are used by ioc_pcbDetails.
 * 
 * These macros represent those configurations that have been tested working on
 * hardware.  Any config change in the example that uses an untested routing
 * will trigger test build failures until that new routing is added to these
 * lists.  This extra step is a reminder to HW-test new routings.
 * 
 * To be explicit that HW tests happened, a date and initials are added before
 * each define indicating who performed the test, and when.  Un-initialed lines
 * should be considered provisional and not mergeable.
 */
#ifndef ioc_mcuDetails_H
#define ioc_mcuDetails_H

/* Which IOCs are enabled and HW-tested.*/
// Next 2 tested on Firment devboard v0.1 MGT
#define IOC_4  0
#define IOC_14 0

/* Which IOCs may be routed to which ISRs.*/
// Next 2 tested on Firment devboard v0.1 MGT
#define IOC_14_to_ISR_3 3
#define IOC_4_to_ISR_2 2

#endif // ioc_mcuDetails_H