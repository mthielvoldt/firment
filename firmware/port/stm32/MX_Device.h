/**
 * @file MX_Device.h
 * @attention This is usually a generated file, that Firment treats as hand-
 * editable in order to avoid needing to use generators that don't pass a cost-
 * benefit analysis: 
 * Benefit: 
 * - Developer avoids looking up GPIO_AFx_SPIy when pins change position.
 * Cost: 
 * - Significant learning curve to understand the tool's proper use.
 * - Significant setup (downloads, .yml configs, run sequence) could be scripted
 * - Significant lengthening of CI workflow (slows build)
 * - Imposes directory structure that must be reconformed to Firment's
 * 
 * @brief
 * Included by Drivers found in CMSIS-Driver_STM32/Drivers/
 * 
 * A starting-point can be found here: 
 * https://github.com/Open-CMSIS-Pack/csolution-examples/blob/main/CubeMX/STM32CubeMX/MyBoard_ROM/MX_Device/MX_Device.h
 * 
 */

#ifndef MX_DEVICE_H
#define MX_DEVICE_H

#define MX_DEVICE_VERSION 0x01000000U

#if defined(stm32g4)
#define CMSIS_device_header <stm32g4xx.h>
#elif defined(stm32l4)
#define CMSIS_device_header <stm32l4xx.h>
#endif

#include <comm_pcbDetails.h>  // FMT_USES_<transport>

#endif // MX_DEVICE_H