/** Firment Interrupt on change module 
 * Provides a standard interface to pin level-change driven interrupts.
*/
#include <stdint.h>

typedef enum {
  EDGE_TYPE_FALLING, 
  EDGE_TYPE_RISING,
  EDGE_TYPE_BOTH,
} edgeType_t; 

typedef void (*isr_t)(void);

void fmt_initIoc(uint32_t port, uint8_t pin, edgeType_t activeEdges, isr_t isr);
