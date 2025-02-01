/*
 * Copyright (C) 2014 Infineon Technologies AG. All rights reserved.
 *
 * Infineon Technologies AG (Infineon) is supplying this software for use with Infineon's microcontrollers.
 * This file can be freely distributed within development tools that are supporting such microcontrollers.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,OR CONSEQUENTIAL DAMAGES, FOR ANY REASON
 * WHATSOEVER.
 *
 */

/**
 * @file ETH_PHY_KSZ8031.c
 * @date 16 Jan, 2015
 * @version 1.0
 *
 * @brief Ethernet Physical Layer Transceiver (PHY) Driver for KSZ8081
 *
 * History
 * Version 1.0 
 *  Initial version
 *
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_PHY# = n (default: 0)
 * -------------------------------------------------------------------- */

/*******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include "Driver_ETH_PHY.h"

/*******************************************************************************
 * MACROS
 *******************************************************************************/
#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,00) /* driver version */

#ifndef ETH_PHY_NUM
#define ETH_PHY_NUM     0        /* Default driver number */
#endif

#ifndef ETH_PHY_ADDR
#define ETH_PHY_ADDR    0x0      /* Default device address */
#endif

/* Basic Registers */
#define REG_BMCR            (0x00U)        /* Basic Mode Control Register       */
#define REG_BMSR            (0x01U)        /* Basic Mode Status Register        */
#define REG_PHYIDR1         (0x02U)        /* PHY Identifier 1                  */
#define REG_PHYIDR2         (0x03U)        /* PHY Identifier 2                  */
#define REG_ANAR            (0x04U)        /* Auto-Negotiation Advertisement    */
#define REG_ANLPAR          (0x05U)        /* Auto-Neg. Link Partner Abitily    */
#define REG_ANER            (0x06U)        /* Auto-Neg. Expansion Register      */
#define REG_ANNPTR          (0x07U)        /* Auto-Neg. Next Page TX            */

/* Extended Registers */
#define REG_PHYCTRL1        (0x1eU)        /* PHY control 1 Register            */

/* Basic Mode Control Register */
#define BMCR_RESET          (0x8000U)      /* Software Reset                    */
#define BMCR_LOOPBACK       (0x4000U)      /* Loopback mode                     */
#define BMCR_SPEED_SEL      (0x2000U)      /* Speed Select (1=100Mb/s)          */
#define BMCR_ANEG_EN        (0x1000U)      /* Auto Negotiation Enable           */
#define BMCR_POWER_DOWN     (0x0800U)      /* Power Down                        */
#define BMCR_ISOLATE        (0x0400U)      /* Isolate Media interface           */
#define BMCR_REST_ANEG      (0x0200U)      /* Restart Auto Negotiation          */
#define BMCR_DUPLEX         (0x0100U)      /* Duplex Mode (1=Full duplex)       */
#define BMCR_COL_TEST       (0x0080U)      /* Collision Test                    */

/* Basic Mode Status Register */
#define BMSR_100B_T4        (0x8000U)      /* 100BASE-T4 Capable                */
#define BMSR_100B_TX_FD     (0x4000U)      /* 100BASE-TX Full Duplex Capable    */
#define BMSR_100B_TX_HD     (0x2000U)      /* 100BASE-TX Half Duplex Capable    */
#define BMSR_10B_T_FD       (0x1000U)      /* 10BASE-T Full Duplex Capable      */
#define BMSR_10B_T_HD       (0x0800U)      /* 10BASE-T Half Duplex Capable      */
#define BMSR_MF_PRE_SUP     (0x0040U)      /* Preamble suppression Capable      */
#define BMSR_ANEG_COMPL     (0x0020U)      /* Auto Negotiation Complete         */
#define BMSR_REM_FAULT      (0x0010U)      /* Remote Fault                      */
#define BMSR_ANEG_ABIL      (0x0008U)      /* Auto Negotiation Ability          */
#define BMSR_LINK_STAT      (0x0004U)      /* Link Status (1=established)       */
#define BMSR_JABBER_DET     (0x0002U)      /* Jaber Detect                      */
#define BMSR_EXT_CAPAB      (0x0001U)      /* Extended Capability               */

/* PHY control 1 Register */
#define PHYCTRL1_OPMODE_SPEED      (0x0003U)
#define PHYCTRL1_OPMODE_DUPLEX     (0x0004U)

/* PHY Identifier Registers */
#define PHY_ID1             (0x0022U)      /* KSZ8081 Device Identifier MSB     */
#define PHY_ID2             (0x1560U)      /* KSZ8081 Device Identifier LSB     */

/*******************************************************************************
 * LOCAL VARIABLES
 *******************************************************************************/

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_PHY_API_VERSION,
  ARM_ETH_PHY_DRV_VERSION
};

static ARM_ETH_PHY_Read_t  reg_rd;
static ARM_ETH_PHY_Write_t reg_wr;
static uint16_t            reg_bmcr;

/*******************************************************************************
 * DRIVER IMPLEMENTATION
 *******************************************************************************/

/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) 
{
  return DriverVersion;
}


/**
  \fn          int32_t Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                   ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) 
{
  uint16_t val;

  /* Register PHY read/write functions. */
  if ((fn_read == NULL) || (fn_write == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  
  reg_rd = fn_read;
  reg_wr = fn_write;

  /* Check Device Identification. */
  reg_rd(ETH_PHY_ADDR, REG_PHYIDR1, &val);
  if (val != PHY_ID1) {
    /* Wrong PHY device. */
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  
  reg_rd(ETH_PHY_ADDR, REG_PHYIDR2, &val);
  if ((val & 0xFFF0) != PHY_ID2) {
    /* Wrong PHY device. */
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  reg_bmcr = BMCR_POWER_DOWN;
  return (reg_wr(ETH_PHY_ADDR, REG_BMCR, reg_bmcr));
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) 
{
  return (reg_wr(ETH_PHY_ADDR, REG_BMCR, BMCR_POWER_DOWN));
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) 
{

  switch (state) {
    case ARM_POWER_OFF:
      reg_bmcr |=  BMCR_POWER_DOWN;
      break;
      
    case ARM_POWER_FULL:
      if(!(reg_bmcr & BMCR_POWER_DOWN)){
          return ARM_DRIVER_OK;
      }

      reg_bmcr &= ~BMCR_POWER_DOWN;
      break;
      
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return (reg_wr(ETH_PHY_ADDR, REG_BMCR, reg_bmcr));
}

/**
  \fn          int32_t SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t SetInterface (uint32_t interface) 
{
  switch (interface) {
    case ARM_ETH_INTERFACE_RMII:
      break;
      
    case ARM_ETH_INTERFACE_MII:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return(0);
}

/**
  \fn          int32_t SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t SetMode (uint32_t mode) 
{
  uint16_t val;

  val = reg_bmcr & BMCR_POWER_DOWN;

  switch (mode & ARM_ETH_PHY_SPEED_Msk) {
    case ARM_ETH_PHY_SPEED_10M:
      break;
    case ARM_ETH_PHY_SPEED_100M:
      val |=  BMCR_SPEED_SEL;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
    case ARM_ETH_PHY_DUPLEX_HALF:
      break;
    case ARM_ETH_PHY_DUPLEX_FULL:
      val |=  BMCR_DUPLEX;
      break;
  }

  if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
    val |= BMCR_ANEG_EN;
  }

  if (mode & ARM_ETH_PHY_LOOPBACK) {
    val |= BMCR_LOOPBACK;
  }

  if (mode & ARM_ETH_PHY_ISOLATE) {
    val |= BMCR_ISOLATE;
  }

  reg_bmcr = val;

  return (reg_wr(ETH_PHY_ADDR, REG_BMCR, reg_bmcr));
}

/**
  \fn          ARM_ETH_LINK_STATE GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE GetLinkState (void) 
{
  ARM_ETH_LINK_STATE state;
  uint16_t           val;

  reg_rd(ETH_PHY_ADDR, REG_BMSR, &val);
  state = (val & BMSR_LINK_STAT) ? ARM_ETH_LINK_UP : ARM_ETH_LINK_DOWN;

  return (state);
}

/**
  \fn          ARM_ETH_LINK_INFO GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO GetLinkInfo (void) 
{
  ARM_ETH_LINK_INFO info;
  uint16_t          val;

  reg_rd(ETH_PHY_ADDR, REG_PHYCTRL1, &val);
  info.speed  = ((val & PHYCTRL1_OPMODE_SPEED) - 1U) ? ARM_ETH_SPEED_100M   : ARM_ETH_SPEED_10M;
  info.duplex = (val & PHYCTRL1_OPMODE_DUPLEX) ? ARM_ETH_DUPLEX_FULL : ARM_ETH_DUPLEX_HALF;

  return (info);
}


/* PHY Driver Control Block */
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM) = {
  GetVersion,
  Initialize,
  Uninitialize,
  PowerControl,
  SetInterface,
  SetMode,
  GetLinkState,
  GetLinkInfo
};
