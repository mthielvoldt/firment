/** Firment Interrupt on change module
 * Provides a interface to configure gpio edge-driven interrupts.
 */
#pragma once

#include <RTE_Device.h>
#include <xmc_eru.h>
#include <stdint.h>

typedef enum
{
  EDGE_TYPE_NONE,
  EDGE_TYPE_FALLING,
  EDGE_TYPE_RISING,
  EDGE_TYPE_BOTH,
} edgeType_t;

void fmt_initIoc(
    RTE_IOC_t inputConfig,
    edgeType_t activeEdges,
    uint8_t outputChannel,
    uint8_t IRQn,
    uint32_t priority);
