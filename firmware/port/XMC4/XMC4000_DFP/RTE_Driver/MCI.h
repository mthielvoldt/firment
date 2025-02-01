/*
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 *
 */

 /**
 * @file MCI.h
 * @date 30 June, 2016
 * @version 1.0
 *
 * @brief MCI/SDMMC Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

#ifndef MCI_H
#define MCI_H

/***************************************************************************
 * HEADER FILES
 ***************************************************************************/
#include "Driver_MCI.h"
#include "xmc_scu.h"
#include "xmc_gpio.h"
#include "xmc_sdmmc.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

/***************************************************************************
 *  MACROS
 ***************************************************************************/
#if (defined(RTE_Drivers_MCI) && \
    (RTE_SDMMC0 == 0))
#error "SDMMC not configured in RTE_Device.h!"
#endif

/* Card detect change event enable */
#if (RTE_SDMMC_CD_EVENT_EN == 0)
#define MCI_CD_EVENT           0U
#else
#define MCI_CD_EVENT           1U
#endif

/* Card detect pin enable */
#if (RTE_SDMMC_CD_PIN_EN == 0)
#define MCI_CD_PIN             0U
#else
#define MCI_CD_PIN             1U
#endif

/* write Protect detect pin enable */
#if (RTE_SDMMC_WP_PIN_EN == 0)
#define MCI_WP_PIN             0U
#else
#define MCI_WP_PIN             1U
#endif

/* SDMMC Power pin enable */
#if (RTE_SDMMC_PWR_PIN_EN == 0)
#define MCI_PWR_PIN            0U
#else
#define MCI_PWR_PIN            1U
#endif

/* SDMMC Power pin enable */
#if (RTE_SDMMC_RESET_PIN_EN == 0)
#define MCI_RESET_PIN          0U
#else
#define MCI_RESET_PIN          1U
#endif

/* Enable Bus width of four line */
#if (RTE_SDMMC_BUS_WIDTH_4 == 0)
#define MCI_BUS_WIDTH_4        0U
#else
#define MCI_BUS_WIDTH_4        1U
#endif

/* Transfer event mask */
#define MCI_TRANSFER_EVENT_Msk    (ARM_MCI_EVENT_TRANSFER_ERROR   |\
                                   ARM_MCI_EVENT_TRANSFER_TIMEOUT |\
                                   ARM_MCI_EVENT_TRANSFER_COMPLETE)

/* Command event mask */
#define MCI_COMMAND_EVENT_Msk     (ARM_MCI_EVENT_COMMAND_ERROR   |\
                                   ARM_MCI_EVENT_COMMAND_TIMEOUT |\
                                   ARM_MCI_EVENT_COMMAND_COMPLETE)
/* Expected response flag mask */
#define MCI_RESPONSE_EXPECTED_Msk (ARM_MCI_RESPONSE_SHORT      |\
                                   ARM_MCI_RESPONSE_SHORT_BUSY |\
                                   ARM_MCI_RESPONSE_LONG)

/* SDMMC Default peripheral clock frequncy*/
#define MCI_SDMMC_INT_DEFAULT_CLOCK_IN_HZ  (48000000UL)

/* Normal interrupt status bits*/
#define MCI_NORMAL_INT_STATUS_BITS         (0x7FFFU)

/* Data Buffer ready status- Read or write ready*/
#define MCI_DATA_BUFFER_READY   (((uint32_t)XMC_SDMMC_BUFFER_READ_READY) |\
                                ((uint32_t)XMC_SDMMC_BUFFER_WRITE_READY))

/* Driver flag definitions */
#define MCI_INIT                ((uint8_t)0x01)   /* MCI initialized            */
#define MCI_POWER               ((uint8_t)0x02)   /* MCI powered on& configured */
#define MCI_RESP_LONG           ((uint8_t)0x04)   /* Long response expected     */
#define MCI_DATA_MODE           ((uint8_t)0x08)   /* Data Transfer mode         */
#define MCI_DATA_READ           ((uint8_t)0x10)   /* Read transfer              */
#define MCI_DATA_WRITE          ((uint8_t)0x20)   /* Writetransfer              */

/***************************************************************************
 * ENUMERATIONS
 ***************************************************************************/
/**
 * @brief Data transfer direction
 */
typedef enum MCI_DATA_TRANSFER_DIR {
  MCI_DATA_TRANSFER_READ_BUFFER = 1U,     /**< Read from card */
  MCI_DATA_TRANSFER_WRITE_BUFFER          /**< Write to card */
} MCI_DATA_TRANSFER_DIR_t;

/***************************************************************************
 * DATA STRUCTURES
 ***************************************************************************/
/**
 * @brief MCI GPIO configuration
 */
typedef struct MCI_GPIO_CONFIG {
  XMC_GPIO_CONFIG_t config;               /**< Pin configuration structure*/
  XMC_GPIO_HWCTRL_t hw_control;           /**< HW control setting for pins*/
} MCI_GPIO_CONFIG_t;

/**
 * @brief MCI pin configuration
 */
typedef struct MCI_PIN_CONFIG {
  XMC_GPIO_PORT_t *const port;            /**< Pointer to the GPIO port base address*/
  const uint8_t pin;                      /**< Pin number in the port*/
  const MCI_GPIO_CONFIG_t *config;        /**< Reference to pin configuration structure*/
} MCI_PIN_CONFIG_t;

/**
 * @brief MCI Data Transfer Information
 */
typedef struct MCI_DATAXFER {
  uint32_t *buf;                          /* Data buffer*/
  MCI_DATA_TRANSFER_DIR_t transfer_mode;  /* Data Transfer mode*/
  uint32_t block_cnt;                     /* No of data blocks to transfer*/
  uint32_t block_size;                    /* Blocks size in transfer*/
  uint32_t block_no_quadlets;             /* Block size in quad bytes, total bytes diveded by 4( word size)*/
} MCI_DATAXFER_t;

/**
 * @brief MCI Transfer Information, Master structure
 */
typedef struct MCI_TRANSFER_INFO {
  ARM_MCI_STATUS         status;          /* MCI Driver status*/
  ARM_MCI_SignalEvent_t  cb_event;        /* Driver event callback function*/
  uint32_t               *response;       /* Pointer to response buffer*/
  MCI_DATAXFER_t         dataxfer;        /* Data Transfer Information*/
  uint8_t volatile       flags;           /* Driver state flags*/
} MCI_TRANSFER_INFO_t;

#endif /* MCI_H */

