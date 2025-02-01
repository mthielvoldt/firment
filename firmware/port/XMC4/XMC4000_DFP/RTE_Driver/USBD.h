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
 * @file USBD.h
 * @date 21 December, 2015
 * @version 2.5
 *
 * @brief USBD Driver for Infineon XMC4000
 *
 * History
 *
 * Version 2.5 Added  macro for powered device reconnection hadling for OTG devices <br>
 * Version 2.4 Clock handling modified for supporting XMC4800 and XMC4700 devices
 *             Added "__attribute__((section ("RW_IRAM1")))" for the proper memory location
 *             of xmc_device <br>
 * Version 2.3 Initialize/Uninitialize, Power Control 
 *               guidelines related modifications. 
 *               EndpointTransfer() bug fixed<br>
 *
 * Version 1.0 Initial version<br>
 */

#ifndef USBD_H_
#define USBD_H_
#include "xmc_scu.h"
#include "driver_usbd.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

/** Register Address */ 
#define XMC_USB_BASE_ADDRESS ((uint8_t*)USB0_BASE)

/**
 * \brief Driver interface function table
 *
 * To access the XMC device controller driver interface use this table of functions.
 */
extern const ARM_DRIVER_USBD Driver_USBD0;
/**
 * \defgroup USB_Device_Controller_Driver_Internals_Group Driver Internals
 * \brief Functions, defines, typedefs used internal for handling tasks inside the driver
 *
 * In order to work efficient some special internal functions and typedefs are needed. These
 * are only used inside the driver and should not be used by the end users code.
 * @{
 */

/**
 * \defgroup USB_Device_Controller_Driver_Register_Group Register
 * @{
 * \brief Structures, unions, typedefs and defines for handling the registers of the device controller.
 *
 * For more information see the \ref dwc_otg_regs.h file documentation. All values in this group are
 * only for the register interface and are not used by the actuall driver.
 */

/*******************************************************
 * Register Definitions
 *******************************************************/
/** Maximum endpoint channel */
#define MAX_EPS_CHANNELS 7
/** Maximum periodic fifos in usb core */
#define MAX_PERIO_FIFOS 1
/** Maximum tx fifos */
#define MAX_TX_FIFOS 14
typedef void* dwc_dma_t;
#include "dwc_otg_regs.h"

/**
 * @}
 */
#pragma anon_unions

/* VBUS MACRO */
//#if (UC_SERIES ==XMC42) | (UC_SERIES ==XMC41) 
//#else
#define ARM_USBD_VBUS_DETECT
//#endif

/* Fifo definitions */
/** First fifo register offset from base address */
#define XMC_USB_TX_FIFO_REG_OFFSET 0x1000
/** Offset for each fifo register */
#define XMC_USB_TX_FIFO_OFFSET 0x1000

/* Interrupt priority */
/** Defines USB interrupt priority */
#define XMC_PREEMPTION_PRIORITY          63

/** Defines USB interrupt sub priority */
#define XMC_SUB_PRIORITY                 0

/** Number of hardware tx endpoint fifos */
#define USB_NUM_TX_FIFOS 7
/** Overall FIFO size in bytes */
#define MAX_FIFO_SIZE 2048

/* Number Definitions */
/** Number of hardware endpoints */
#define NUM_EPS 7

/* ep0 definitions */
/** Amount of setup packets */
#define USB_SETUP_COUNT 3
/** Size of setup data */
#define USB_SETUP_SIZE 8

/* Register Flags */
/** FS device speed flag in DCFG register */
#define USB_DCFG_DevSpd_FS 0x3

/** GRXSTSP Register PktSts options */
enum GRXSTS_PktSts {
	USB_GRXSTS_PktSts_GOutNAK = 0x1, /**< Global out nak send ( triggers an interrupt ) */
	USB_GRXSTS_PktSts_OutData = 0x2, /**< OUT data packet received */
	USB_GRXSTS_PktSts_OutCmpl = 0x3, /**< OUT transfer completed (triggers an interrupt) */
	USB_GRXSTS_PktSts_SetupCmpl = 0x4, /**< SETUP transaction completed (triggers an interrupt) */
	USB_GRXSTS_PktSts_Setup = 0x6 /**< SETUP data packet received */
};

/** Endpoint type options */
enum EP_TYPE {
	EP_TYPE_Control = 0, /**< Control endpoint */
	EP_TYPE_Iso = 1, /**< Isochronous endpoint */
	EP_TYPE_Intr = 2, /**< Interrupt endpint */
	EP_TYPE_Bulk = 3 /**< Bulk endpoint */
};

/**
 * \brief Describes an usb endpoint of a XMC device
 *
 * This struct describes an endpoint of a XMC device. It contains
 * all necessary data for operation and control of that endpoint.
 */
typedef struct {
	/** \name Configuration */
	union {
		uint32_t address:8; /**< the endpoint address including the direction */
		struct {
			uint32_t number:4; /**< the endpoint number */
			uint32_t :3; /**< Pading between number and direction */
			uint32_t direction:1; /**< the endpoint direction */
		};
	};
	uint32_t type:2; /**< the endpoint type */
	uint32_t isConfigured:1; /**< a flag showing, if the endpoint is configured */
	volatile uint32_t isInUse:1; /**< a flag showing, if the endpoint is currently in use */
  volatile uint32_t outInUse:1;
	uint32_t isStalled:1; /**< a flag showing, if the endpoint is stalled. */
	uint32_t txFifoNum:4; /**< TX Fifo Number */
	uint32_t sendZeroLengthPacket:1; /**< a flag showing, if a zero length packet has to be send at the end of the transfer */
	uint32_t maxPacketSize:7; /**< the maximum size of packages for this endpoint ( due to FS Speed device only 64 Byte )*/
	uint32_t maxTransferSize:19; /**< the maximum amount of data the core can send at once. (Depends on the field width in the register) */

	/** \name Endpoint buffers */
	uint8_t *outBuffer; /**< the buffer for operation as out endpoint */
	uint32_t outBytesAvailable; /**< bytes available in the out buffer */
	uint32_t outBufferSize; /**< the size of the out buffer */
	uint32_t outOffset; /**< the read offset of the out buffer */
	uint8_t *inBuffer; /**< the buffer for operation as in endpoint */
	uint32_t inBufferSize; /**< the size of the in buffer */

	/* \name TX transfer controls */
	uint8_t *txBuffer; /**< the buffer of the current TX transfer */
	uint32_t txLength; /**< the length of the current TX transfer */
	uint32_t txCount; /**< bytes transfered of the current TX transfer */
	uint32_t txTotal; /**< the length of data in TX buffer */

	/* \name RX transfer controls */
	uint8_t *rxBuffer; /**< the buffer of the current RX transfer */
	uint8_t *rxDataPtr; /**< the destination data pointer of the current RX transfer */  
	uint32_t rxLength; /**< the length of the current RX transfer */
	uint32_t rxCount; /**< bytes transfered of the current RX transfer */
	uint32_t rxTotal; /**< the length of all data in RX buffer */
} xmc_usb_ep_t;

/**
 * \brief A struct describing a XMC usb device
 *
 * All information to control an xmc usb device is stored in
 * this struct. It contains register, callbacks and information
 * about the endpoints and the status of the device.
 */
typedef struct xmc_usb_device {
	/** \name Endpoints */
	xmc_usb_ep_t ep[NUM_EPS]; /**< Array of endpoints of the device \ref xmc_usb_ep_t */

	/** \name Registers */
	dwc_otg_core_global_regs_t *global_register; /**< Global register interface */
	dwc_otg_device_global_regs_t *device_register; /**< Device register interface */
	dwc_otg_dev_in_ep_regs_t *endpoint_in_register[NUM_EPS]; /**< IN Endpoint register interface */
	dwc_otg_dev_out_ep_regs_t *endpoint_out_register[NUM_EPS]; /**< OUT Endpoint register interface */
	volatile uint32_t *fifo[USB_NUM_TX_FIFOS]; /**< TX Fifo interface */

	/** \name Configuration */
	uint16_t txfifomsk; /**< Mask of used tx fifos */
	/** \name Device Status */
	uint8_t IsConnected:1; /**< Flag if device is connected */
	uint8_t IsActive:1; /**< Flag if device is currently active ( in transfer ) */
	uint8_t IsPowered; /**< Flag if device is powered by Vbus */

	/** \name Callbacks */
	ARM_USBD_SignalDeviceEvent_t DeviceEvent_cb; /**< The device event callback */
	ARM_USBD_SignalEndpointEvent_t EndpountEvent_cb; /**< The endpoint event callback */
} xmc_usb_device_t;

/** \brief XMC device data
 *
 * The instance of structure describing the xmc device.
 *
 * \see xmc_usb_device_t
 */
extern xmc_usb_device_t xmc_device;

/**
 * @}
 */

/*
 * Public Functions
 */


/**
 * \defgroup USB_Device_Controller_Driver_Interface_Group Driver Interface
 *
 * \brief Interface to communicate with the device controller (USB MAC)
 *
 * The complete functionality of the device driver is abstracted to set of functions in this module. The functions do no protocol handling.
 * This has to be done in upper layers.
 *
 * @{
 */

/**
 * \brief Get the version of the driver
 *
 * \return Driver version
 * \see DRV_VERSION
 */
ARM_DRIVER_VERSION XMC_GetVersion(void);

/**
 * \brief Get the capabilities of the driver
 *
 * \return Driver capabilities
 * \see USBD_CAPABILITIES
 */
ARM_USBD_CAPABILITIES XMC_GetCapabilities(void);

/**
  \fn          int32_t ARM_USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                            ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
  \brief       Initialize USB Device Interface.
  \param[in]   cb_device_event    Pointer to \ref ARM_USBD_SignalDeviceEvent
  \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBD_SignalEndpointEvent
  \return      \ref execution_status
*/
static int32_t XMC_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                ARM_USBD_SignalEndpointEvent_t cb_endpoint_event);

/**
  \fn          int32_t ARM_USBD_Uninitialize (void)
  \brief       De-initialize USB Device Interface.
  \return      \ref execution_status
*/
static int32_t XMC_Uninitialize (void);

/**
  \fn          int32_t ARM_USBD_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Device Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t XMC_PowerControl(ARM_POWER_STATE state);

/**
  \fn          int32_t ARM_USBD_DeviceConnect (void)
  \brief       Connect USB Device.
  \return      \ref execution_status
*/
static int32_t XMC_DeviceConnect(void);


/**
  \fn          ARM_USBD_STATE ARM_USBD_DeviceGetState (void)
  \brief       Get current USB Device State.
  \return      Device State \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE XMC_DeviceGetState (void);

/**
 * \brief Get the state of the device
 *
 * Power, activity and device state data will be collected and return.
 *
 * \return Device state (See \ref USBD_STATE)
 */
ARM_USBD_STATE XMC_DeviceGetState(void);

/**
  \fn          int32_t ARM_USBD_DeviceRemoteWakeup (void)
  \brief       Trigger USB Remote Wakeup.
  \return      \ref execution_status
*/
static int32_t XMC_DeviceRemoteWakeup(void);

/**
  \fn          int32_t ARM_USBD_DeviceSetAddress (uint8_t dev_addr)
  \brief       Set USB Device Address.
  \param[in]   dev_addr  Device Address
  \return      \ref execution_status
*/
static int32_t XMC_DeviceSetAddress (uint8_t dev_addr);

/**
  \fn          int32_t ARM_USBD_EndpointConfigure (uint8_t  ep_addr,
                                                   uint8_t  ep_type,
                                                   uint16_t ep_max_packet_size)
  \brief       Configure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type  Endpoint Type
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \return      \ref execution_status
*/
static int32_t XMC_EndpointConfigure (uint8_t ep_addr,
                                       uint8_t  ep_type,
                                       uint16_t ep_max_packet_size);

/**
  \fn          int32_t ARM_USBD_EndpointUnconfigure (uint8_t ep_addr)
  \brief       Unconfigure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t XMC_EndpointUnconfigure (uint8_t ep_addr);

/**
  \fn          int32_t ARM_USBD_EndpointStall (uint8_t ep_addr, bool stall)
  \brief       Set/Clear Stall for USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   stall  Operation
                - \b false Clear
                - \b true Set
  \return      \ref execution_status
*/
static int32_t XMC_EndpointStall (uint8_t ep_addr, bool stall);

/**
  \fn          int32_t ARM_USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)
  \brief       Read data from or Write data to USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[out]  data Pointer to buffer for data to read or with data to write
  \param[in]   num  Number of data bytes to transfer
  \return      \ref execution_status
*/
static int32_t XMC_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num);
/**
  \fn          uint32_t ARM_USBD_EndpointTransferGetResult (uint8_t ep_addr)
  \brief       Get result of USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      number of successfully transfered data bytes
*/
static uint32_t XMC_EndpointTransferGetResult (uint8_t ep_addr);

#if 0
/**
 * \brief Prepare an endpoint to receive out tokens
 *
 * The selected endpoint gets configured, so that it receives the specified amount of data from the host.
 *
 * \param[in] ep_addr Endpoint address
 * \param[in] buffer Buffer address to store data at
 * \param[in] size Size of the buffer
 * \return Status of the function (See \ref ARM_USBD_STATUS)
 */
ARM_USBD_STATUS XMC_EndpointReadStart(uint8_t ep_addr,uint8_t * buffer,uint32_t size);
#endif
/**
 * \brief Read data from an endpoint
 *
 * If data has been received for this endpoint, it gets copied into the user buffer until its full
 * or no data is left in the driver buffer.
 *
 * \param[in] ep_addr Endpoint address
 * \param[in] buffer User buffer
 * \param[in] length Length of data to read
 * \return Amount of data read from the endpoint buffer (>=0) or a negative value if an error happened
 */
int32_t XMC_EndpointRead(uint8_t ep_addr,uint8_t * buffer,uint32_t length);

/**
 * \brief Write data to an endpoint
 *
 * The User data gets copied into the driver buffer or will be send directly based on the buffer concept
 * choosen in the configuration. After that the endpoint will be set up to transfer the data to the host.
 *
 * \param[in] ep_addr Endpoint address
 * \param[in] buffer Data buffer to write to the endpoint
 * \param[in] length Length of the data buffer
 *
 * \return Amount of data written to the endpoint buffer (>=0) or a negative value if an error happened
 */
int32_t XMC_EndpointWrite(uint8_t ep_addr,const uint8_t * buffer,uint32_t length);

/**
  \fn          int32_t ARM_USBD_EndpointTransferAbort (uint8_t ep_addr)
  \brief       Abort current USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t XMC_EndpointTransferAbort (uint8_t ep_addr);

/**
 * \brief Get the current USB frame number
 *
 * \return Frame number
 */
uint16_t XMC_GetFrameNumber(void);

/**
 * @}
 */

#endif /* USBD_H_ */

/**
 * @}
 */
