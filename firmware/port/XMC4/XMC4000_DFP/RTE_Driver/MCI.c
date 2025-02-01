/*
 * Copyright (c) 2015-2016, Infineon Technologies AG
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
 * @file MCI.c
 * @date 30 June, 2016
 * @version 1.0
 *
 * @brief MCI/SDMMC Driver for Infineon XMC devices
 *
 * History
 *
 * Version 1.0 Initial version<br>
 */

/***************************************************************************
 * HEADER FILES
 ***************************************************************************/
#include "MCI.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "xmc_sdmmc.h"

/***************************************************************************
 * MACROS
 ***************************************************************************/
/* driver version */
#define ARM_MCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR    (1, 0)

/* Macro for gpio uninitialization from SDMMC peripheral */
#define MCI_GPIO_UNINITIALIZE(pinfunction)\
  XMC_GPIO_Init(MCI_0_##pinfunction##.port, MCI_0_##pinfunction##.pin,\
               &MCI_0_PIN_CONFIG_DEFUALT);\
  XMC_GPIO_SetHardwareControl(MCI_0_##pinfunction##.port, MCI_0_##pinfunction##.pin,\
                              MCI_0_PIN_HW_CONFIG_DEFUALT)

#define MCI_DELAY_LOOP_COUNT                                100000U
#define MCI_IRQ_PREEMTION_PRIORITY                          63U
#define MCI_IRQ_SUB_PRIORITY                                0
#define MCI_MASK_BYTE_ZERO                                  0x000000FFU
/* Macros used to find value of time out counter value in SDMMC_TIMEOUT_CTRL register */
#define MCI_POW_VAL_OF_TWO_IN_TIMOUT_REG                    13U
#define MCI_MAX_VAL_IN_TIMOUT_REG                           14U
#define MCI_CMD_INDX_12                                     12U

/***************************************************************************
 * GLOBAL VARIABLES/DATA STRUCTURES
 ***************************************************************************/

/* MCI Transfer Information, Master structure */
static MCI_TRANSFER_INFO_t   MCI;

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_MCI_API_VERSION,
  ARM_MCI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_MCI_CAPABILITIES DriverCapabilities = {
  MCI_CD_PIN,                                     /* cd_state          */
  MCI_CD_EVENT,                                   /* cd_event          */
  MCI_WP_PIN,                                     /* wp_state          */
  MCI_PWR_PIN,                                    /* vdd               */
  0U,                                             /* vdd_1v8           */
  0U,                                             /* vccq              */
  0U,                                             /* vccq_1v8          */
  0U,                                             /* vccq_1v2          */
  MCI_BUS_WIDTH_4,                                /* data_width_4      */
  0U,                                             /* data_width_8      */
  0U,                                             /* data_width_4_ddr  */
  0U,                                             /* data_width_8_ddr  */
  1U,                                             /* high_speed        */
  0U,                                             /* uhs_signaling     */
  0U,                                             /* uhs_tuning        */
  0U,                                             /* uhs_sdr50         */
  0U,                                             /* uhs_sdr104        */
  0U,                                             /* uhs_ddr50         */
  0U,                                             /* uhs_driver_type_a */
  0U,                                             /* uhs_driver_type_c */
  0U,                                             /* uhs_driver_type_d */
  0U,                                             /* sdio_interrupt    */
  0U,                                             /* read_wait         */
  0U,                                             /* suspend_resume    */
  0U,                                             /* mmc_interrupt     */
  0U,                                             /* mmc_boot          */
  0U,                                             /* rst_n             */
  0U,                                             /* ccs               */
  0U                                              /* ccs_timeout       */
};

/***************MCI - GPIO PIN configuration Structure**********************/

/* MCI pin config in default for un-initialization*/
const XMC_GPIO_CONFIG_t MCI_0_PIN_CONFIG_DEFUALT = {
  .mode = XMC_GPIO_MODE_INPUT_TRISTATE,
};
const XMC_GPIO_HWCTRL_t MCI_0_PIN_HW_CONFIG_DEFUALT = XMC_GPIO_HWCTRL_DISABLED;

#if (MCI_CD_PIN != 0U)
/* MCI card detect pin config */
const MCI_GPIO_CONFIG_t MCI_0_CARD_DETECT_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_INPUT_TRISTATE,
  }
};
const MCI_PIN_CONFIG_t MCI_0_CARD_DETECT = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_CARD_DETECT_PORT,
  .pin  = (uint8_t)RTE_SDMMC_CARD_DETECT_PIN,
  .config = &MCI_0_CARD_DETECT_PIN_CONFIG
};
#endif

#if (MCI_WP_PIN != 0U)
/* MCI write protect pin config */
const MCI_GPIO_CONFIG_t MCI_0_WRITE_PROTECT_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_INPUT_TRISTATE,
  }
};
const MCI_PIN_CONFIG_t MCI_0_WRITE_PROTECT = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_WRITE_PROTECT_PORT,
  .pin  = (uint8_t)RTE_SDMMC_WRITE_PROTECT_PIN,
  .config = &MCI_0_WRITE_PROTECT_PIN_CONFIG
};
#endif

#if (MCI_RESET_PIN != 0U)
/* MCI Reset pin config */
const MCI_GPIO_CONFIG_t MCI_0_RESET_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_RESET = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_RESET_PORT,
  .pin  = (uint8_t)RTE_SDMMC_RESET_PIN,
  .config = &MCI_0_RESET_PIN_CONFIG
};
#endif

#if (MCI_PWR_PIN != 0U)
/* MCI Bus Power pin config */
const MCI_GPIO_CONFIG_t MCI_0_BUS_POWER_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_BUS_POWER = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_BUS_POWER_PORT,
  .pin  = (uint8_t)RTE_SDMMC_BUS_POWER_PIN,
  .config = &MCI_0_BUS_POWER_PIN_CONFIG
};
#endif

/* MCI Data 0 pin config */
const MCI_GPIO_CONFIG_t MCI_0_DATA_0_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_DATA_0 = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_DATA_0_PORT,
  .pin  = (uint8_t)RTE_SDMMC_DATA_0_PIN,
  .config = &MCI_0_DATA_0_PIN_CONFIG
};

#if (MCI_BUS_WIDTH_4 != 0U)
/* MCI Data 1 pin config */
const MCI_GPIO_CONFIG_t MCI_0_DATA_1_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_DATA_1 = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_DATA_1_PORT,
  .pin  = (uint8_t)RTE_SDMMC_DATA_1_PIN,
  .config = &MCI_0_DATA_1_PIN_CONFIG
};

/* MCI Data 2 pin config */
const MCI_GPIO_CONFIG_t MCI_0_DATA_2_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_DATA_2 = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_DATA_2_PORT,
  .pin  = (uint8_t)RTE_SDMMC_DATA_2_PIN,
  .config = &MCI_0_DATA_2_PIN_CONFIG
};

/* MCI Data 3 pin config */
const MCI_GPIO_CONFIG_t MCI_0_DATA_3_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_DATA_3 = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_DATA_3_PORT,
  .pin  = (uint8_t)RTE_SDMMC_DATA_3_PIN,
  .config = &MCI_0_DATA_3_PIN_CONFIG
};

#endif

/* MCI Command pin config */
const MCI_GPIO_CONFIG_t MCI_0_COMMAND_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_COMMAND = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_COMMAND_PORT,
  .pin  = (uint8_t)RTE_SDMMC_COMMAND_PIN,
  .config = &MCI_0_COMMAND_PIN_CONFIG
};

/* MCI Clock pin config */
const MCI_GPIO_CONFIG_t MCI_0_CLK_PIN_CONFIG = {
  .config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  },
  .hw_control = XMC_GPIO_HWCTRL_PERIPHERAL1,
};
const MCI_PIN_CONFIG_t MCI_0_CLK = {
  .port = (XMC_GPIO_PORT_t *)RTE_SDMMC_CLK_PORT,
  .pin  = (uint8_t)RTE_SDMMC_CLK_PIN,
  .config = &MCI_0_CLK_PIN_CONFIG
};

/***************************************************************************
 * API DEFINITIONS
 ***************************************************************************/

/**
  \fn          ARM_DRV_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}

/**
  \fn          ARM_MCI_CAPABILITIES GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_MCI_CAPABILITIES
*/
static ARM_MCI_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}

/**
  \fn            int32_t Initialize (ARM_MCI_SignalEvent_t cb_event)
  \brief         Initialize the Memory Card Interface
  \param[in]     cb_event  Pointer to \ref ARM_MCI_SignalEvent
  \return        \ref execution_status
*/
static int32_t Initialize (ARM_MCI_SignalEvent_t cb_event) {
  /* Configure SDMMC source clock as 48MHz from PLL*/
  XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
  XMC_SCU_CLOCK_SetUsbClockDivider(4U);
  
  /* Start USB/SDMMC PLL */
  XMC_SCU_CLOCK_StartUsbPll(1U, 32U);

  /* Configure all GPIOs related to SDMMC peripheral */
  /* Command(CMD) Pin configuration */
  XMC_GPIO_Init(MCI_0_COMMAND.port, MCI_0_COMMAND.pin,
                &MCI_0_COMMAND.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_COMMAND.port, MCI_0_COMMAND.pin,
                              MCI_0_COMMAND.config->hw_control);

  /* Clock Pin configuration*/
  XMC_GPIO_Init(MCI_0_CLK.port, MCI_0_CLK.pin,
                &MCI_0_CLK.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_CLK.port, MCI_0_CLK.pin,
                              MCI_0_CLK.config->hw_control);
#if (MCI_RESET_PIN != 0U)
  /* Reset Pin configuration*/
  XMC_GPIO_Init(MCI_0_RESET.port, MCI_0_RESET.pin,
                &MCI_0_RESET.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_RESET.port, MCI_0_RESET.pin,
                              MCI_0_RESET.config->hw_control);
#endif

  /* DATA0 Pin configuration*/
  XMC_GPIO_Init(MCI_0_DATA_0.port, MCI_0_DATA_0.pin,
                &MCI_0_DATA_0.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_DATA_0.port, MCI_0_DATA_0.pin,
                              MCI_0_DATA_0.config->hw_control);

#if (MCI_BUS_WIDTH_4 != 0U)
  /* DATA1 Pin configuration*/
  XMC_GPIO_Init(MCI_0_DATA_1.port, MCI_0_DATA_1.pin,
                &MCI_0_DATA_1.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_DATA_1.port, MCI_0_DATA_1.pin,
                              MCI_0_DATA_1.config->hw_control);

  /* DATA2 Pin configuration*/
  XMC_GPIO_Init(MCI_0_DATA_2.port, MCI_0_DATA_2.pin,
                &MCI_0_DATA_2.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_DATA_2.port, MCI_0_DATA_2.pin,
                              MCI_0_DATA_2.config->hw_control);

  /* DATA3 Pin configuration*/
  XMC_GPIO_Init(MCI_0_DATA_3.port, MCI_0_DATA_3.pin,
                &MCI_0_DATA_3.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_DATA_3.port, MCI_0_DATA_3.pin,
                              MCI_0_DATA_3.config->hw_control);
#endif

#if (MCI_CD_PIN != 0U)
  /* Card Detect Pin configuration*/
  XMC_GPIO_Init(MCI_0_CARD_DETECT.port, MCI_0_CARD_DETECT.pin,
                &MCI_0_CARD_DETECT.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_CARD_DETECT.port, MCI_0_CARD_DETECT.pin,
                              MCI_0_CARD_DETECT.config->hw_control);
#endif

#if (MCI_PWR_PIN != 0U)
  /* Bus power Pin configuration*/
  XMC_GPIO_Init(MCI_0_BUS_POWER.port, MCI_0_BUS_POWER.pin,
                &MCI_0_BUS_POWER.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_BUS_POWER.port, MCI_0_BUS_POWER.pin,
                              MCI_0_BUS_POWER.config->hw_control);
#endif

#if (MCI_WP_PIN != 0U)
  /* Bus power Pin configuration*/
  XMC_GPIO_Init(MCI_0_WRITE_PROTECT.port, MCI_0_WRITE_PROTECT.pin,
                &MCI_0_WRITE_PROTECT.config->config);
  XMC_GPIO_SetHardwareControl(MCI_0_WRITE_PROTECT.port, MCI_0_WRITE_PROTECT.pin,
                              MCI_0_WRITE_PROTECT.config->hw_control);
#endif
  
  /* Enable SDMMC peripheral */
  XMC_SDMMC_Enable(XMC_SDMMC);

  /* Enable SCU clock for SDMMC */
  XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_MMC);

  /* SD clock disable */
  XMC_SDMMC_SDClockDisable(XMC_SDMMC);

  /* Resets the Host Controller's register */
  XMC_SDMMC_SetSWReset(XMC_SDMMC, XMC_SDMMC_SW_RESET_ALL);

  /* A stub of delay for reset to finish */
  for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
    __NOP();
  }

  /* Stop internal clock to change prescalar*/
  XMC_SDMMC_Stop(XMC_SDMMC);

  /* Select SD clock frequency, Select Div2(24MHz) as defulat frequncy */
  XMC_SDMMC_SDClockFreqSelect(XMC_SDMMC, XMC_SDMMC_CLK_DIV_2);

  /* Enable delay on command and data lines */
  XMC_SDMMC_EnableDelayCmdDatLines();

  /* Set the delay value chosen in the APP user interface */
  XMC_SDMMC_SetDelay(8);

  /* Internal clock enable */
  XMC_SDMMC_Start(XMC_SDMMC);

  /* A stub of delay for initialization */
  for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
    __NOP();
  }

  /* Set data-line timeout to a defualt value of maximum */
  XMC_SDMMC_SetDataLineTimeout(XMC_SDMMC, XMC_SDMMC_DAT_TIMEOUT_COUNTER_2_POW_27);

  /* Set bus voltage in the power control register, Set as 3.3 volts */
  XMC_SDMMC_SetBusVoltage(XMC_SDMMC, XMC_SDMMC_BUS_VOLTAGE_3_3_VOLTS);

  /* Clear MCI driver status */
  MCI.status.command_active   = 0U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_active  = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.sdio_interrupt   = 0U;
  MCI.status.ccs              = 0U;

  /* Store call back function pointer */
  MCI.cb_event = cb_event;
  /* Set driver state as intialized */
  MCI.flags = MCI_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn            int32_t Uninitialize (void)
  \brief         De-initialize Memory Card Interface.
  \return        \ref execution_status
*/
static int32_t Uninitialize (void) {
  /* Clear driver status as not initialized */
  MCI.flags = 0U;

  /* Configure GPIO of SDMMC to default state and hence release the port pins */
  MCI_GPIO_UNINITIALIZE(COMMAND);

  MCI_GPIO_UNINITIALIZE(CLK);

  MCI_GPIO_UNINITIALIZE(DATA_0);

#if (MCI_BUS_WIDTH_4 != 0U)
  MCI_GPIO_UNINITIALIZE(DATA_1);
  MCI_GPIO_UNINITIALIZE(DATA_2);
  MCI_GPIO_UNINITIALIZE(DATA_3);
#endif

#if (MCI_CD_PIN != 0U)
  MCI_GPIO_UNINITIALIZE(CARD_DETECT);
#endif

#if (MCI_RESET_PIN != 0U)
  MCI_GPIO_UNINITIALIZE(RESET);
#endif

#if (MCI_PWR_PIN != 0U)
  MCI_GPIO_UNINITIALIZE(BUS_POWER);
#endif

#if (MCI_WP_PIN != 0U)
  MCI_GPIO_UNINITIALIZE(WRITE_PROTECT);
#endif

return ARM_DRIVER_OK;
}

/**
  \fn            int32_t PowerControl (ARM_POWER_STATE state)
  \brief         Control Memory Card Interface Power.
  \param[in]     state   Power state \ref ARM_POWER_STATE
  \return        \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  switch (state) {
    case ARM_POWER_OFF:
      /* Set driver state as powered off */
      MCI.flags &= ~MCI_POWER;
      /* Clear MCI driver status */
      MCI.status.command_active   = 0U;
      MCI.status.command_timeout  = 0U;
      MCI.status.command_error    = 0U;
      MCI.status.transfer_active  = 0U;
      MCI.status.transfer_timeout = 0U;
      MCI.status.transfer_error   = 0U;
      MCI.status.sdio_interrupt   = 0U;
      MCI.status.ccs              = 0U;

      /* Resets the Host Controller's register */
      XMC_SDMMC_SetSWReset(XMC_SDMMC, XMC_SDMMC_SW_RESET_ALL);
      /* A stub of delay for reset to finish */
      for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
      }

      /* Disable delay on command and data lines */
      XMC_SDMMC_DisableDelayCmdDatLines();
      /* SD Bus Power off */
      XMC_SDMMC_BusPowerOff(XMC_SDMMC);
      /* SD clock disable */
      XMC_SDMMC_SDClockDisable(XMC_SDMMC);
      /* Stop internal clock */
      XMC_SDMMC_Stop(XMC_SDMMC);
      /* Disable SCU clock for SDMMC */
      XMC_SCU_CLOCK_DisableClock(XMC_SCU_CLOCK_MMC);
      /* Disable SDMMC peripheral */
      XMC_SDMMC_Disable(XMC_SDMMC);
      /* Disable SDMMC IRQ in NVIC */
      NVIC_DisableIRQ(SDMMC0_0_IRQn);
      break;

    case ARM_POWER_FULL:
      /* Return Driver error , if MCI is not initialized before this */
      if ((MCI.flags & MCI_INIT)  == 0) {
        return ARM_DRIVER_ERROR;
      }

      /* Return Driver error , if MCI is not already powered on before- Do nothing */
      if ((MCI.flags & MCI_POWER) != 0) {
        return ARM_DRIVER_OK;
      }

      /* Enable normal interrupt status bits */
      XMC_SDMMC_EnableEventStatus(XMC_SDMMC, (XMC_SDMMC_CMD_COMPLETE | XMC_SDMMC_TX_COMPLETE |
                                  XMC_SDMMC_BUFFER_WRITE_READY | XMC_SDMMC_BUFFER_READ_READY |
                                  XMC_SDMMC_CARD_INS | XMC_SDMMC_CARD_REMOVAL));

      /* Enable error interrupt status bits */
      XMC_SDMMC_EnableEventStatus(XMC_SDMMC, ( XMC_SDMMC_CMD_TIMEOUT_ERR | XMC_SDMMC_CMD_CRC_ERR |
                                  XMC_SDMMC_CMD_END_BIT_ERR | XMC_SDMMC_CMD_IND_ERR |
                                  XMC_SDMMC_DATA_TIMEOUT_ERR | XMC_SDMMC_DATA_CRC_ERR |
                                  XMC_SDMMC_DATA_END_BIT_ERR | XMC_SDMMC_CURRENT_LIMIT_ERR |
                                  XMC_SDMMC_ACMD_ERR));

      /* Enable normal interrupt signal for registering IRQ to NVIC */
      XMC_SDMMC_EnableEvent(XMC_SDMMC, (XMC_SDMMC_CMD_COMPLETE | XMC_SDMMC_TX_COMPLETE |
                            XMC_SDMMC_BUFFER_WRITE_READY | XMC_SDMMC_BUFFER_READ_READY));

#if (MCI_CD_EVENT != 0U)
      XMC_SDMMC_EnableEvent(XMC_SDMMC, ((uint32_t)XMC_SDMMC_CARD_REMOVAL) |
                           ((uint32_t)XMC_SDMMC_CARD_INS));
#endif

      /* Enable error interrupt signal for registering IRQ to NVIC */
      XMC_SDMMC_EnableEvent(XMC_SDMMC, (XMC_SDMMC_CMD_TIMEOUT_ERR | XMC_SDMMC_CMD_CRC_ERR |
                                        XMC_SDMMC_CMD_END_BIT_ERR | XMC_SDMMC_CMD_IND_ERR |
                                        XMC_SDMMC_DATA_TIMEOUT_ERR | XMC_SDMMC_DATA_CRC_ERR |
                                        XMC_SDMMC_DATA_END_BIT_ERR | XMC_SDMMC_CURRENT_LIMIT_ERR |
                                        XMC_SDMMC_ACMD_ERR));
      #if (MCI_PWR_PIN != 0U)
      /* Turn the bus power on */
      XMC_SDMMC_BusPowerOn(XMC_SDMMC);
      #endif
      /* Enable the SD clock */
      XMC_SDMMC_SDClockEnable(XMC_SDMMC);

      /* Enable Interrupt */
      NVIC_SetPriority(SDMMC0_0_IRQn,
                       NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                       MCI_IRQ_PREEMTION_PRIORITY, MCI_IRQ_SUB_PRIORITY));

      NVIC_ClearPendingIRQ(SDMMC0_0_IRQn);
      NVIC_EnableIRQ(SDMMC0_0_IRQn);

      /* Set driver state as Powered On */
      MCI.flags |= MCI_POWER;
      break;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn            int32_t CardPower (uint32_t voltage)
  \brief         Set Memory Card supply voltage.
  \param[in]     voltage  Memory Card supply voltage
  \return        \ref execution_status
*/
static int32_t CardPower (uint32_t voltage) {
  /* Return Driver error , if MCI is not powered on to full before this */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

#if (MCI_PWR_PIN != 0U)
  /* Power on/off is supported */
  switch (voltage & ARM_MCI_POWER_VDD_Msk) {
    case ARM_MCI_POWER_VDD_OFF:
      /* Switch off Bus power */
      XMC_SDMMC_BusPowerOff(XMC_SDMMC);
      return ARM_DRIVER_OK;

    case ARM_MCI_POWER_VDD_3V3:
      /* Switch on Bus power , 3.3V */
      XMC_SDMMC_BusPowerOn(XMC_SDMMC);
      return ARM_DRIVER_OK;

    default:
      break;
  }
#endif
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn            int32_t ReadCD (void)
  \brief         Read Card Detect (CD) state.
  \return        1:card detected, 0:card not detected, or error
*/
static int32_t ReadCD (void) {
  /* Return Driver error , if MCI is not powered on to full before this */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
#if (MCI_CD_PIN != 0U)
  XMC_SDMMC_PRESENT_STATE_t present_state;
  present_state.b32 = 0U;

  /* Read present state register of SDMMC host controller */
  present_state = XMC_SDMMC_GetPresentState(XMC_SDMMC);
  /* Check if card detect pin level is high  */
  if ( present_state.card_detect_pin_level ){
    /* Card is present */
    return (1);
  }
  else{
  /* Card is not present  */
  // Do Nothing;
  }
#endif
  /* If not suported also, in defulat return, card is not present */
  return (0);
}

/**
  \fn            int32_t ReadWP (void)
  \brief         Read Write Protect (WP) state.
  \return        1:write protected, 0:not write protected, or error
*/
static int32_t ReadWP (void) {
  /* Return Driver error , if MCI is not powered on to full before this */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
#if (MCI_WP_PIN != 0U)
  XMC_SDMMC_PRESENT_STATE_t present_state;
  present_state.b32 = 0U;

  /* Read present state register of SDMMC host controller */
  present_state = XMC_SDMMC_GetPresentState(XMC_SDMMC);
  /* Check if write protect pin level is high  */
  if ( present_state.write_protect_pin_level ){
    /* Write enabled */
    // Do Nothing;
  }
  else{
    /* Write protected  */
    return (1);
  }
#endif
  /* If not suported also, in defulat return, card is Write enabled */
  return (0);
}

/**
  \fn            int32_t SendCommand (uint32_t  cmd,
                                      uint32_t  arg,
                                      uint32_t  flags,
                                      uint32_t *response)
  \brief         Send Command to card and get the response.
  \param[in]     cmd       Memory Card command
  \param[in]     arg       Command argument
  \param[in]     flags     Command flags
  \param[out]    response  Pointer to buffer for response
  \return        \ref execution_status
*/
static int32_t SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response) {
  /* Check if response pointer is not NULL, if response expexted for the cmd */
  if (((flags & MCI_RESPONSE_EXPECTED_Msk) != 0U) && (response == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Check driver state is in powered on state, otherwise return error */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Check if previous command is still active, if so, return error */
  if (MCI.status.command_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  /* Check if cmd and data line in hardware is busy, if so, return error */
  if ((XMC_SDMMC_IsCommandLineBusy(XMC_SDMMC)) ||
     ((XMC_SDMMC_IsDataLineBusy(XMC_SDMMC))&& (flags & ARM_MCI_TRANSFER_DATA))){
    return ARM_DRIVER_ERROR_BUSY;
  }

  /*
   * Workaround: MCI spec does not support ACMD12. Hence ,Keil middle will sent CMD12
   * at the end of multi block transfer. Then it is expected to have command complete
   * event. ButXMC IP support ACMD12 and it enabled too. As a workaround for MCI spec,
   * ARM support team suggested to add workaround ARM Support case number 663089.
   */
  if (cmd == MCI_CMD_INDX_12) {
    flags = ARM_MCI_RESPONSE_NONE;
  }

  /* Set command active status */
  MCI.status.command_active   = 1U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.ccs              = 0U;

  /* Fill cmd register variable, with command index value, recieved in cmd parameter */
  cmd = (cmd & MCI_MASK_BYTE_ZERO) << SDMMC_COMMAND_CMD_IND_Pos;

  /* Store the response pointer , to be filled later in IRQ */
  MCI.response = response;

  /* Clear driver state flags (all flags) from any previous set */
  MCI.flags &= ~( MCI_RESP_LONG | MCI_DATA_MODE | MCI_DATA_READ | MCI_DATA_WRITE );

  /* Check expected response of command from flag parameter */
  switch (flags & ARM_MCI_RESPONSE_Msk) {
    case ARM_MCI_RESPONSE_NONE:
      /* Clear RESP_TYPE_SELECT bit in cmd register variable. No response expected */
      cmd &= ~(SDMMC_COMMAND_RESP_TYPE_SELECT_Msk);
      break;

    case ARM_MCI_RESPONSE_SHORT:
      /* Short response (48-bit) expected */
      cmd |= (((XMC_SDMMC_COMMAND_RESPONSE_t)XMC_SDMMC_COMMAND_RESPONSE_SHORT)<< SDMMC_COMMAND_RESP_TYPE_SELECT_Pos);
      break;

    case ARM_MCI_RESPONSE_SHORT_BUSY:
      /* Short response with busy signal (48-bit) expected */
      cmd |= (((XMC_SDMMC_COMMAND_RESPONSE_t)XMC_SDMMC_COMMAND_RESPONSE_SHORT_BUSY)<< SDMMC_COMMAND_RESP_TYPE_SELECT_Pos);
      break;

    case ARM_MCI_RESPONSE_LONG:
      MCI.flags |= MCI_RESP_LONG;
      /*  Long response (136-bit) expected */
      cmd |= (((XMC_SDMMC_COMMAND_RESPONSE_t)XMC_SDMMC_COMMAND_RESPONSE_LONG)<< SDMMC_COMMAND_RESP_TYPE_SELECT_Pos);
      break;

    default:
      return ARM_DRIVER_ERROR;
   }

  if (flags & ARM_MCI_RESPONSE_CRC) {
    /* CRC is to be checked in response */
    cmd |= SDMMC_COMMAND_CMD_CRC_CHECK_EN_Msk;
  }

  if (flags & ARM_MCI_RESPONSE_INDEX) {
    /* Command Index is to be checked in response */
    cmd |= SDMMC_COMMAND_CMD_IND_CHECK_EN_Msk;
  }

  if (flags & ARM_MCI_TRANSFER_DATA) {
    /* Data will be present in the command, Activate Data transfer */
    cmd |= SDMMC_COMMAND_DATA_PRESENT_SELECT_Msk;
    /* Set driver state as data transfer mode, for this command */
    MCI.flags |= MCI_DATA_MODE;
  }

  /* Clear all error interrupt flags */
  XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)(XMC_SDMMC_CMD_TIMEOUT_ERR |
                       XMC_SDMMC_CMD_CRC_ERR | XMC_SDMMC_CMD_END_BIT_ERR |
                       XMC_SDMMC_CMD_IND_ERR | XMC_SDMMC_DATA_TIMEOUT_ERR |
                       XMC_SDMMC_DATA_CRC_ERR | XMC_SDMMC_DATA_END_BIT_ERR |
                       XMC_SDMMC_ACMD_ERR));

  /* Clear all normal interrupt flags */
  XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)(XMC_SDMMC_CMD_COMPLETE |
                       XMC_SDMMC_BUFFER_READ_READY | XMC_SDMMC_BUFFER_WRITE_READY |
                       XMC_SDMMC_TX_COMPLETE ));

  /* Send the command to bus */
  XMC_SDMMC_SendCommand(XMC_SDMMC,(XMC_SDMMC_COMMAND_t*)&cmd, arg);

  return ARM_DRIVER_OK;
}

/**
  \fn            int32_t SetupTransfer (uint8_t *data,
                                        uint32_t block_count,
                                        uint32_t block_size,
                                        uint32_t mode)
  \brief         Setup read or write transfer operation.
  \param[in,out] data         Pointer to data block(s) to be written or read
  \param[in]     block_count  Number of blocks
  \param[in]     block_size   Size of a block in bytes
  \param[in]     mode         Transfer mode
  \return        \ref execution_status
*/
static int32_t SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode) {

  XMC_SDMMC_TRANSFER_MODE_t response;

  /* Check if data pointer is not NULL and block details are valid */
  if ((data == NULL) || (block_count == 0U) || (block_size == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  /* Check driver state is in powered on state, otherwise return error */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  /* Check if previous transfer request is still active, if so, return error */
  if (MCI.status.transfer_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  /* Store buffer pointer and block details in to MCI structure */
  MCI.dataxfer.buf = (uint32_t*)data;
  MCI.dataxfer.block_cnt = block_count;
  MCI.dataxfer.block_size = block_size;
  /* SDMMC transfer happens in word size through FIFO. word size is 4 bytes */
  MCI.dataxfer.block_no_quadlets =  block_size >> 2U;

  /* Clear MCI flag status for DATA Transfer and direction, Read or Write*/
  MCI.flags &= ~(MCI_DATA_READ | MCI_DATA_WRITE);

  if (mode & ARM_MCI_TRANSFER_STREAM) {
    /* Stream Data transfer (MMC only), MMC - not supported in driver*/
    return (ARM_DRIVER_ERROR_UNSUPPORTED);
  }

  if ((mode & ARM_MCI_TRANSFER_WRITE) == 0) {
    /* Direction: From card to controller, read operation */
    MCI.flags |= MCI_DATA_READ;
    MCI.dataxfer.transfer_mode = MCI_DATA_TRANSFER_READ_BUFFER;
    XMC_SDMMC_SetDataTransferDirection(XMC_SDMMC, XMC_SDMMC_DATA_TRANSFER_CARD_TO_HOST);
  }
  else {
    /* Direction: From controller to card , write operation */
    MCI.flags |= MCI_DATA_WRITE;
    MCI.dataxfer.transfer_mode = MCI_DATA_TRANSFER_WRITE_BUFFER;
    XMC_SDMMC_SetDataTransferDirection(XMC_SDMMC, XMC_SDMMC_DATA_TRANSFER_HOST_TO_CARD);
  }

  /* Set Data transfer mode */
  response.block_size = block_size;
  response.num_blocks = block_count;
/* Check if single block transfer or multi block transfer, for multi block enable ACMD12 */
  if(block_count == 1 ){
    response.type = XMC_SDMMC_TRANSFER_MODE_TYPE_SINGLE;
    response.auto_cmd = XMC_SDMMC_TRANSFER_MODE_AUTO_CMD_DISABLED;
  }
  else {
    response.type = XMC_SDMMC_TRANSFER_MODE_TYPE_MULTIPLE;
    response.auto_cmd = XMC_SDMMC_TRANSFER_MODE_AUTO_CMD_12;
    /* Enable ACMD 12 interrupt signal */
    XMC_SDMMC_EnableEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_ACMD_ERR);
  }

  /* Set Block size */
  XMC_SDMMC->BLOCK_SIZE = (uint16_t)(response.block_size);

  /* Set Number of blocks */
  XMC_SDMMC->BLOCK_COUNT = (uint16_t)(response.num_blocks);

  /* Set Type of data transfer: single, infinite, multiple or stop multiple */
  XMC_SDMMC->TRANSFER_MODE = (uint16_t)((XMC_SDMMC->TRANSFER_MODE & (uint16_t)~SDMMC_TRANSFER_MODE_MULTI_BLOCK_SELECT_Msk) |
                                       ((uint16_t)response.type));

  return (ARM_DRIVER_OK);
}

/**
  \fn            int32_t AbortTransfer (void)
  \brief         Abort current read/write data transfer.
  \return        \ref execution_status
*/
static int32_t AbortTransfer (void) {
  /* Check driver state is in powered on state, otherwise return error */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Disable normal interrupt signal for registering IRQ to NVIC */
  XMC_SDMMC_DisableEvent(XMC_SDMMC, (XMC_SDMMC_CMD_COMPLETE | XMC_SDMMC_TX_COMPLETE |
                         XMC_SDMMC_BUFFER_WRITE_READY | XMC_SDMMC_BUFFER_READ_READY));
#if (MCI_CD_EVENT != 0U)
  XMC_SDMMC_DisableEvent(XMC_SDMMC, ((uint32_t)XMC_SDMMC_CARD_REMOVAL) |
                        ((uint32_t)XMC_SDMMC_CARD_INS));
#endif
  /* Disable error interrupt signal for registering IRQ to NVIC */
  XMC_SDMMC_DisableEvent(XMC_SDMMC, ( XMC_SDMMC_CMD_TIMEOUT_ERR | XMC_SDMMC_CMD_CRC_ERR |
                         XMC_SDMMC_CMD_END_BIT_ERR | XMC_SDMMC_CMD_IND_ERR |
                         XMC_SDMMC_DATA_TIMEOUT_ERR | XMC_SDMMC_DATA_CRC_ERR |
                         XMC_SDMMC_DATA_END_BIT_ERR | XMC_SDMMC_CURRENT_LIMIT_ERR |
                         XMC_SDMMC_ACMD_ERR));
  /* Clear Normal Events*/
  XMC_SDMMC_ClearEvent(XMC_SDMMC, (XMC_SDMMC_CMD_COMPLETE | XMC_SDMMC_TX_COMPLETE |
                       XMC_SDMMC_BUFFER_WRITE_READY | XMC_SDMMC_BUFFER_READ_READY));
#if (MCI_CD_EVENT != 0U)
  XMC_SDMMC_ClearEvent(XMC_SDMMC, ((uint32_t)XMC_SDMMC_CARD_REMOVAL) |
                      ((uint32_t)XMC_SDMMC_CARD_INS) |
                      ((uint32_t)XMC_SDMMC_CARD_INT));
#endif
  /* Clear Error Events*/
  XMC_SDMMC_ClearEvent(XMC_SDMMC, ( XMC_SDMMC_CMD_TIMEOUT_ERR | XMC_SDMMC_CMD_CRC_ERR |
                       XMC_SDMMC_CMD_END_BIT_ERR | XMC_SDMMC_CMD_IND_ERR |
                       XMC_SDMMC_DATA_TIMEOUT_ERR | XMC_SDMMC_DATA_CRC_ERR |
                       XMC_SDMMC_DATA_END_BIT_ERR | XMC_SDMMC_CURRENT_LIMIT_ERR |
                       XMC_SDMMC_ACMD_ERR));

  /* Resets the Host Controller's register */
  XMC_SDMMC_SetSWReset(XMC_SDMMC, XMC_SDMMC_SW_RST_CMD_LINE | XMC_SDMMC_SW_RST_DAT_LINE );

  /* A stub of delay to finish reset */
  for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
  }

  /* Clear MCI driver status */
  MCI.status.command_active   = 0U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_active  = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.sdio_interrupt   = 0U;
  MCI.status.ccs              = 0U;

  /* Clear driver state flags (all flags) from any previous set */
  MCI.flags &= ~( MCI_RESP_LONG | MCI_DATA_MODE | MCI_DATA_READ | MCI_DATA_WRITE );

  /* Enable normal interrupt signal for registering IRQ to NVIC */
  XMC_SDMMC_EnableEvent(XMC_SDMMC, (XMC_SDMMC_CMD_COMPLETE | XMC_SDMMC_TX_COMPLETE |
                        XMC_SDMMC_BUFFER_WRITE_READY | XMC_SDMMC_BUFFER_READ_READY));
#if (MCI_CD_EVENT != 0U)
  XMC_SDMMC_EnableEvent(XMC_SDMMC, ((uint32_t)XMC_SDMMC_CARD_REMOVAL) | ((uint32_t)XMC_SDMMC_CARD_INS));
#endif
  /* Enable error interrupt signal for registering IRQ to NVIC */
  XMC_SDMMC_EnableEvent(XMC_SDMMC, ( XMC_SDMMC_CMD_TIMEOUT_ERR | XMC_SDMMC_CMD_CRC_ERR |
                        XMC_SDMMC_CMD_END_BIT_ERR | XMC_SDMMC_CMD_IND_ERR |
                        XMC_SDMMC_DATA_TIMEOUT_ERR | XMC_SDMMC_DATA_CRC_ERR |
                        XMC_SDMMC_DATA_END_BIT_ERR | XMC_SDMMC_CURRENT_LIMIT_ERR |
                        XMC_SDMMC_ACMD_ERR));
  return ARM_DRIVER_OK;
}

/**
  \fn            int32_t Control (uint32_t control, uint32_t arg)
  \brief         Control MCI Interface.
  \param[in]     control  Operation
  \param[in]     arg      Argument of operation (optional)
  \return        \ref execution_status
*/
  static int32_t Control (uint32_t control, uint32_t arg) {
  /* Check driver state is in powered on state, otherwise return error */
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Local variables used for deriving prescalar value and time out value */
  XMC_SDMMC_SDCLK_FREQ_SEL_t prescalar = XMC_SDMMC_CLK_DIV_1;
  uint8_t shift_count = 0;
  uint8_t timeout_powval = 0;
  uint8_t timeout_regval = 0;
  uint32_t f_sdmmc_clock_val = 0;
  uint32_t fsd_clock_val = 0;

  switch (control) {
    case ARM_MCI_BUS_SPEED:
      /* Get frequency of SDMMC peripheral configured in init */
      f_sdmmc_clock_val = MCI_SDMMC_INT_DEFAULT_CLOCK_IN_HZ;
      /* Check if bus speed is max speed of SDMMC peripheral */
      if ( arg >= f_sdmmc_clock_val){
        fsd_clock_val = f_sdmmc_clock_val;
      }
      else{
        /* Find the next prescalar value to be loaded to CLK presclar register */
        prescalar = (XMC_SDMMC_SDCLK_FREQ_SEL_t) (1U << shift_count);
        /* Check prescalar value matches with requested bus speed in argument */
        while ( prescalar < 0x80U && prescalar != 0) {
          if ( (f_sdmmc_clock_val/ (prescalar+prescalar)) <= arg ) {
          break;
          }
          else {
            /* Find the next prescalar value to be loaded to CLK presclar register */
            shift_count ++;
            prescalar = (XMC_SDMMC_SDCLK_FREQ_SEL_t)(1U << shift_count);
          }
        }
        /* Actual sd bus speed, used to return */
        fsd_clock_val = f_sdmmc_clock_val/(prescalar+prescalar);
      }

      /* Power off SD Bus */
      XMC_SDMMC_BusPowerOff(XMC_SDMMC);
      /* SD clock disable */
      XMC_SDMMC_SDClockDisable(XMC_SDMMC);
      /* Stop internal clock to change prescalar*/
      XMC_SDMMC_Stop(XMC_SDMMC);
      /* Select SD clock frequency */
      XMC_SDMMC->CLOCK_CTRL = (uint16_t)(((uint16_t)SDMMC_CLOCK_CTRL_SDCLK_FREQ_SEL_Msk) &
                              ((uint16_t)(prescalar << SDMMC_CLOCK_CTRL_SDCLK_FREQ_SEL_Pos)));
      /* Internal clock enable */
      XMC_SDMMC_Start(XMC_SDMMC);
      /* A stub of delay for initialization */
      for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
      }
      /* Enable the SD clock */
      XMC_SDMMC_SDClockEnable(XMC_SDMMC);
      #if (MCI_PWR_PIN != 0U)
      /* Turn the bus power on */
      XMC_SDMMC_BusPowerOn(XMC_SDMMC);
      #endif
      /* A stub of delay for initialization */
      for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
      }

      return (fsd_clock_val);

    case ARM_MCI_BUS_SPEED_MODE:
      switch (arg) {
        case ARM_MCI_BUS_DEFAULT_SPEED:
          /* Speed mode up to 25MHz */
          XMC_SDMMC_DisableHighSpeed(XMC_SDMMC);
          break;
        case ARM_MCI_BUS_HIGH_SPEED:
          /* Speed mode up to 50MHz */
          XMC_SDMMC_EnableHighSpeed(XMC_SDMMC);
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_BUS_DATA_WIDTH:
      switch (arg) {
        case ARM_MCI_BUS_DATA_WIDTH_1:
          /* Configure 1-bit data bus width */
          XMC_SDMMC_SetDataTransferWidth(XMC_SDMMC, XMC_SDMMC_DATA_LINES_1);
          break;
        case ARM_MCI_BUS_DATA_WIDTH_4:
          /* Configure 4-bit data bus width */
          XMC_SDMMC_SetDataTransferWidth(XMC_SDMMC, XMC_SDMMC_DATA_LINES_4);
          break;
        case ARM_MCI_BUS_DATA_WIDTH_8:
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

#if (MCI_RESET_PIN)
    case ARM_MCI_CONTROL_RESET:
      if (arg) {
        /* Resets the Host Controller's register */
        XMC_SDMMC_SetSWReset(XMC_SDMMC, XMC_SDMMC_SW_RESET_ALL);
        /* A stub of delay to finish reset */
        for (volatile uint32_t i = 0; (uint32_t)i < (uint32_t)MCI_DELAY_LOOP_COUNT; i++) {
        }
      }
      else {
        /* Do Nothing */
      }
      break;
#endif

    case ARM_MCI_DATA_TIMEOUT:
      /* Set data-line timeout */
      /* Find register value for SDMMC_TIMEOUT_CTRL.DAT_TIMEOUT_CNT_VAL to match the argument */
      timeout_powval = MCI_POW_VAL_OF_TWO_IN_TIMOUT_REG;
      timeout_regval = 0;
      /* Find the register value for specified time out */
      while (timeout_regval < MCI_MAX_VAL_IN_TIMOUT_REG){
        if (arg <= (1U << timeout_powval)) {
          break;
        }
        else {
          timeout_powval++;
          timeout_regval++;
        }
      }
      /* Set the time out in register */
      /* Set the time out in register */
      XMC_SDMMC->TIMEOUT_CTRL = (uint8_t)(((uint32_t)timeout_regval << SDMMC_TIMEOUT_CTRL_DAT_TIMEOUT_CNT_VAL_Pos) &
                                (uint32_t)SDMMC_TIMEOUT_CTRL_DAT_TIMEOUT_CNT_VAL_Msk);
      break;

    case ARM_MCI_BUS_CMD_MODE:
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn            ARM_MCI_STATUS GetStatus (void)
  \brief         Get MCI status.
  \return        MCI status \ref ARM_MCI_STATUS
*/
static ARM_MCI_STATUS GetStatus (void) {
  return MCI.status;
}

/* SDMMC0_0_IRQHandler */
void SDMMC0_0_IRQHandler(void) {
  /* Local variables to store events and count variable for loop*/
  uint32_t event = 0U;
  uint32_t count = 0U;
  /* Local variable to store Normal interrupt status from IRQ */
  uint16_t normal_int_status = 0;
  /* Local variable to store Error interrupt status from IRQ */
  uint16_t err_int_status = 0;

  /* Read Normal and Error interrrupt status */
  normal_int_status = XMC_SDMMC->INT_STATUS_NORM;
  err_int_status = XMC_SDMMC->INT_STATUS_ERR;

  /* To mask high byte from response register */
  uint32_t resp_high_mask = 0;

  /*...Handling Error Interrupts and updating MCI status...*/
  /*********************************************************/
  /* Check any error happend by checking last bit of INT_STATUS_NORM register */
  if (normal_int_status & SDMMC_INT_STATUS_NORM_ERR_INT_Msk) {
    /* Check for Command timeout error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_CMD_TIMEOUT_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CMD_TIMEOUT_ERR);
      /* Update status as Command response timeout */
      MCI.status.command_timeout = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_COMMAND_TIMEOUT;
    }
    /* Check for Command CRC error or index error or end bit error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_CMD_CRC_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CMD_CRC_ERR);
      /* Update status as Command error */
      MCI.status.command_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_CMD_END_BIT_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CMD_END_BIT_ERR);
      /* Update status as Command error  */
      MCI.status.command_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_CMD_IND_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CMD_IND_ERR);
      /* Update status as Command error */
      MCI.status.command_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_COMMAND_ERROR;
    }
    /* Check for Data timeout error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_DATA_TIMEOUT_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_DATA_TIMEOUT_ERR);
      /* Update status as Data timeout */
      MCI.status.transfer_timeout = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_TRANSFER_TIMEOUT;
    }
    /* Check for Data CRC error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_DATA_CRC_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_DATA_CRC_ERR);
      /* Update status as Transfer Error  */
      MCI.status.transfer_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    /* Check for Data end bit error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_DATA_END_BIT_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_DATA_END_BIT_ERR);
      /* Update status as Transfer Error  */
      MCI.status.transfer_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    /* ACMD12 error interrupt */
    if (((uint32_t)err_int_status & ((uint32_t)XMC_SDMMC_ACMD_ERR >> 16U)) != (uint32_t)0) {
      /* Clear the error status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_ACMD_ERR);
      /* Update status as Transfer Error  */
      MCI.status.transfer_error = 1;
      /* Store the event */
      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
   }
  }
  /*...Handling Normal Interrupts and updating MCI status...*/
  /**********************************************************/
  /* Check normal interrrupt status bits */
  if (normal_int_status & MCI_NORMAL_INT_STATUS_BITS) {
#if (MCI_CD_EVENT != 0U)
    /* Check for Card insertion interrupt */
    if (((uint32_t)normal_int_status & (uint32_t)XMC_SDMMC_CARD_INS) != 0U) {
      /* Clear the interrupt status bit */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CARD_INS);
      /* Store the event */
      event |= ARM_MCI_EVENT_CARD_INSERTED;
    }
    /* Check for Card removal interrupt */
    if (((uint32_t)normal_int_status & (uint32_t)XMC_SDMMC_CARD_REMOVAL) != 0U) {
      /* Clear the interrupt status bit */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CARD_REMOVAL);
       /* Store the event */
      event |= ARM_MCI_EVENT_CARD_REMOVED;
    }
#endif
        /*..Command completed and Get CMD Response..*/
        /********************************************/
    /* Check for Command complete interrupt */
    if (((uint32_t)normal_int_status & (uint32_t)XMC_SDMMC_CMD_COMPLETE) != (uint32_t)0) {
      /* Clear the interrupt status bit */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_CMD_COMPLETE);
      /* Store the event */
      event |= ARM_MCI_EVENT_COMMAND_COMPLETE;

      /* Command successfully completed, hence fill the response data in MCI data structure */
      if (MCI.response) {
        /* Read response registers, to check for long or short response expected */
        if (MCI.flags & MCI_RESP_LONG) {
          /* For long response read all four response registors. Response[127:8] filled to R0,R2,R4,R6[23:0]*/
          MCI.response[0] = (uint32_t)((uint32_t)XMC_SDMMC->RESPONSE[0] << 8U);
          resp_high_mask = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[0] & 0xFF000000) >> 24U);
          MCI.response[1] = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[1] << 8U) | resp_high_mask);
          resp_high_mask = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[1] & 0xFF000000) >> 24U);
          MCI.response[2] = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[2] << 8U) | resp_high_mask);
          resp_high_mask = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[2] & 0xFF000000) >> 24U);
          MCI.response[3] = (uint32_t)(((uint32_t)XMC_SDMMC->RESPONSE[3] << 8U) | resp_high_mask);
        }
        else {
          /* For short response read only Response-0 registor */
          MCI.response[0] = (uint32_t)XMC_SDMMC->RESPONSE[0];
        }
      }
      /* Check that, if data transfer is associated with the current completed CMD*/
      if (MCI.flags & MCI_DATA_MODE) {
        /* Clear the the data mode in flag*/
        MCI.flags &= ~MCI_DATA_MODE;
        /* Set command active status */
        MCI.status.transfer_active = 1U;
      }
    }
        /*......... Handle Data Transfer..........*/
        /******************************************/
    /* Check for data transfer is active */
    if (MCI.status.transfer_active) {
      /* Check for Data Buffer(FIFO) ready status, either for read ready or write ready */
      if (((uint32_t)normal_int_status & (uint32_t)MCI_DATA_BUFFER_READY) != (uint32_t)0) {
        /* Clear Buffer Read/Write ready interrupt */
        XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_BUFFER_READ_READY);
        XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_BUFFER_WRITE_READY);
        /* Read/write block data, until total blocks are transferred */
        if ((uint32_t)MCI.dataxfer.block_cnt> 0U) {
          /* 4 bytes data transmission in each iteration */
          for (count = 0U; count < MCI.dataxfer.block_no_quadlets; count++) {
            /* Perform Transfer */
            if (MCI.dataxfer.transfer_mode == MCI_DATA_TRANSFER_READ_BUFFER) {
              /* Read from FIFO buffer*/
              *MCI.dataxfer.buf = (uint32_t)XMC_SDMMC_ReadFIFO(XMC_SDMMC);
            }
            if (MCI.dataxfer.transfer_mode == MCI_DATA_TRANSFER_WRITE_BUFFER) {
              /* Write to FIFO buffer*/
              XMC_SDMMC_WriteFIFO(XMC_SDMMC, MCI.dataxfer.buf);
            }
            /* Point for next word in data buffer */
            MCI.dataxfer.buf++;
          }
          /* Decrement number of blocks */
          MCI.dataxfer.block_cnt--;
        }
      }
      if ((uint32_t)MCI.dataxfer.block_cnt == 0U) {
        MCI.status.transfer_active = 0;
      }
    }

    /* Transfer complete interrupt */
    if (((uint32_t)normal_int_status & (uint32_t)XMC_SDMMC_TX_COMPLETE) != (uint32_t)0) {
      /* Clear interrupt status */
      XMC_SDMMC_ClearEvent(XMC_SDMMC, (uint32_t)XMC_SDMMC_TX_COMPLETE);
      /* Store the event */
      event |= ARM_MCI_EVENT_TRANSFER_COMPLETE;
    }
  }

  /* Check if any transfer related event happend */
  if (event & MCI_TRANSFER_EVENT_Msk) {
    /* Clear transfer active status */
    MCI.status.transfer_active = 0U;
  }
  /* Check if any command related event happend */
  if (event & MCI_COMMAND_EVENT_Msk) {
    /* Clear command active status */
    MCI.status.command_active = 0U;
  }

  /* If call back function is registered, call the same */
  if (MCI.cb_event) {
    /* Call the function with stored the events */
    (MCI.cb_event)(event);
  }
}

/* MCI Driver Control Block */
ARM_DRIVER_MCI Driver_MCI0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  CardPower,
  ReadCD,
  ReadWP,
  SendCommand,
  SetupTransfer,
  AbortTransfer,
  Control,
  GetStatus
};

