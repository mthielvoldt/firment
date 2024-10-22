/** Interface for a generic CRC engine
 * Some MCUs provide a hardware peripheral for accelerated CRC calculation.
 * For example, the XMC4000 MCU has a "Flexible CRC Engine" (FCE) module.
 * This file provides a standard CRC-engine interface to a concrete implentation
 * that will depend on what hardware is in-use.
 *
 * Note: use "port/soft-crc.c" if your hardware lacks a CRC accelator.
 * Note: This driver wrapper currenlty only supports 16-bit CRCs.
 */

#pragma once
#include <Driver_Common.h>

#define FMT_CRC_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(0, 0) /* API version */

#ifdef __cplusplus
extern "C"
{
#endif

#define FMT_CRC_SET_RESULT_INVERSION 0
#define FMT_CRC_SET_RESULT_REFLECTION 1
#define FMT_CRC_SET_INPUT_REFLECTION 2
#define FMT_CRC_SET_CRC_ORDER 3
#define FMT_CRC_CLEAR_ERROR 4

typedef enum _CRC_Error {
  FMT_CRC_ERR_UNSPECIFIED,
  FMT_CRC_BUS_ERR,
  FMT_CRC_CONFIG_ERR,
} CRC_Error_t;

  typedef struct _FMT_CRC_CAPABILITIES
  {
    uint32_t crc8_sae_j1850 : 1;  // supports CRC8  polynomial 0x1D
    uint32_t crc16_ccitt : 1;     // supports CRC16 polynomial 0x1021
    uint32_t crc32_ieee802p3 : 1; // supports CRC32 polynomial 0x04C11DB7
    uint32_t reflect_input : 1;   // supports bitwise mirroring the input words
    uint32_t reflect_result : 1;  // supports bitwise mirroring the crc result
    uint32_t invert_result : 1;   // supports bitwise inversion of result
    uint32_t config_err : 1;
    uint32_t bus_err : 1;
  } FMT_CRC_CAPABILITIES_t;

  typedef struct _FMT_CRC_STATUS
  {
    uint32_t config_err : 1; // redundant registers CFG / CHECK  do not match
    uint32_t bus_err : 1;    // SW wrote a word smaller than the CRC width
  } FMT_CRC_STATUS_t;

  // The _t suffix fixes an autoformatter issue.
  typedef ARM_DRIVER_VERSION ARM_DRIVER_VERSION_t;

  typedef struct _FMT_DRIVER_CRC
  {
    ARM_DRIVER_VERSION_t (*GetVersion)(void);
    FMT_CRC_CAPABILITIES_t (*GetCapabilities)(void);
    int32_t (*Initialize)(void); // Should this take an event handler callback?
    int32_t (*Uninitialize)(void);
    int32_t (*PowerControl)(ARM_POWER_STATE state);
    int32_t (*ComputeCRC)(const void *data, uint32_t num, uint16_t *crc);
    int32_t (*Control)(uint32_t control, uint32_t arg);
    FMT_CRC_STATUS_t (*GetStatus)(void);
  } const FMT_DRIVER_CRC;

#ifdef __cplusplus
}
#endif
