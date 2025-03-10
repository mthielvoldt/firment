/**
 * This MCU-specific file fulfills the requirements of fmt_ioc_test.c in the
 * manner that Device_IOC.h files for different targets fulfill the requirements
 * of the applicable fmt_ioc_<port>.c
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
#ifndef Device_IOC_H
#define Device_IOC_H

/* Which IOCs are enabled and HW-tested.*/
// Next 2 tested on Firment devboard v0.1 MGT
#define IOC_4  0
#define IOC_14 0

/* Which IOCs may be routed to which ISRs.*/
// Next 2 tested on Firment devboard v0.1 MGT
#define IOC_14_to_ISR_3 3
#define IOC_4_to_ISR_2 2

#endif // Device_IOC_H