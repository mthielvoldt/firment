#ifndef fmt_sysInit_H
#define fmt_sysInit_H

/**
 * An MCU-specific function (to be implemented in port/<fam>/fmt_sysInit_port)
 * that does things like configure clocks, init HAL's etc.  Essentially anything
 * that the specific architecture requires that isn't initialized in module 
 * driver code.
 */
void fmt_initSys(void);

#endif