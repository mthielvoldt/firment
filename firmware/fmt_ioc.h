/** Firment Interrupt on change module
 * Provides a interface to configure gpio edge-driven interrupts.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  EDGE_TYPE_NONE,
  EDGE_TYPE_FALLING,
  EDGE_TYPE_RISING,
  EDGE_TYPE_BOTH,
} edgeType_t;

bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void));

void fmt_enableIoc(uint8_t iocId);

void fmt_disableIoc(uint8_t iocId);

bool fmt_getIocPinState(uint8_t iocId);
