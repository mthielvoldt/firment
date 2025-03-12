/**
 * This fake MCU header supplies stand-in "port addresses" to fmt_gpio_stub.c.
 * It works like mcu_gpio.h files for real MCUs, providing macros that
 * can be used to create structs needed for feature initialization.
 * 
 * Client(s): config/gpio_pcb.h
 * Selected by: project CMakeLists.txt variable PORT_DIR, -> 
 *   port/<family>/CMakeLists - target_include_directories(PUBLIC .)
 *
 * These macros represent those configurations that have been tested working on
 * hardware.  Any config change in the example that uses an untested routing
 * will trigger test build failures until that new routing is added to these
 * lists.  This extra step is a reminder to HW-test new routings.
 *
 * To be explicit that HW tests happened, initials are added before each define 
 * indicating who performed the test, and on what pcb.  Un-initialed
 * lines should be considered provisional and not mergeable.
 */

typedef struct portPin_s
{
  void *const port;
  uint8_t pin;
} portPin_t;

// Next 2 lines MGT, PCBv0.1.0
#define P5_8 {(void *)5, 8}
#define P5_9 {(void *)5, 9}
