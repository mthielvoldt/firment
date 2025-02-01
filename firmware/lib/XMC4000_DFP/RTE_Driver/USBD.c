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
 * @file USBD.c
 * @date 21 December, 2015
 * @version 2.5
 *
 * @brief USBD Driver for Infineon XMC4000
 *
 * History
 *
 * Version 2.5 Added powered device reconnection handling for OTG devices  and forcing to device mode<br>
 * Version 2.4 Clock handling modified for supporting XMC4800 and XMC4700 devices
 *             Added "__attribute__((section ("RW_IRAM1")))" for the proper memory location
 *             of xmc_device<br>
 *
 * Version 2.3 Initialize/Uninitialize, Power Control 
 *             guidelines related modifications. 
 *             EndpointTransfer() bug fixed<br>
 *
 * Version 1.0 Initial version<br>
 */

#include "USBD.h"
#include "dwc_otg_regs.h"

#define ARM_USBD_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,5)   /* driver version */

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_USBD_API_VERSION,
  ARM_USBD_DRV_VERSION
};

/* bmRequestType.Type */
#define USB_REQUEST_STANDARD                    (0)

/* USB Standard Request Codes */
#define USB_REQUEST_SET_ADDRESS                 (5)

/* Number of maximum endpoints */
#ifndef USB_MAX_EP
#define USB_MAX_EP                              (7)
#endif

xmc_usb_device_t xmc_device __attribute__((section ("RW_IRAM1")));

/*
 * Endpoint Out Fifo
 */
uint8_t USBD_EP_OUT_BUFFER[USB_MAX_EP][64];
/*
 * Endpoint In Fifo
 */
uint8_t USBD_EP_IN_BUFFER[USB_MAX_EP][64];

// Static Variables
static uint8_t  setup_buf[8];
volatile static uint8_t  setup_flag = 0;

static uint32_t usbd_flags = 0;
static ARM_USBD_STATE usbd_state = {0, 0, 0};

// USB Driver Flags
#define USB_INITIALIZED                        (1     <<  0)
#define USB_POWERED                            (1     <<  1)
#define USB_CONNECTED                          (1     <<  2)

/**
 * \internal
 * \ingroup USB_Device_Controller_Driver_Internals_Group
 * \defgroup USB_Device_Controller_Driver_Helpers_Group Driver Helper Functions
 *
 * \brief Set of function helping handling the device controller
 *
 * To ensure, that the code is readable and performance well, some code pieces are
 * placed inside helper functions. This includes the read, write and flush operation of
 * the fifos and the start transfer for an endpoint.
 * @{
 */
static int32_t XMC_Endpoint0_OUT_Init (uint8_t *data, uint32_t num);
/**
 * \brief Flush a tx fifo
 *
 * \param[in] fifo_num Fifo number to flush
 *
 * \note Use 0x10 as parameter to flush all tx fifos.
 */
void XMC_FlushTXFifo(uint8_t fifo_num) {
	volatile grstctl_t data = { .d32 = 0};
	uint32_t count;
	/*flush fifo */
	data.b.txfflsh = 1;
	data.b.txfnum = fifo_num;
	xmc_device.global_register->grstctl = data.d32;
	for (count=0;count<1000;count++);
	do {
		data.d32 = xmc_device.global_register->grstctl;
	} while (data.b.txfflsh);

	count = 0;
	while (count++ < 1000); /* wait 3 phy clocks */
}

/**
 * \brief Flush the rx fifo
 */
void XMC_FlushRXFifo() {
	volatile grstctl_t data = { .d32 = 0};
	uint32_t count;

	data.b.rxfflsh = 1;
	/*flush fifo */
	xmc_device.global_register->grstctl = data.d32;
	do {
		for (count=0;count<1000;count++); 
		data.d32 = xmc_device.global_register->grstctl;
	} while (data.b.rxfflsh);

	count = 0;
	while (count++ < 1000); /* wait 3 phy clocks */
}

/**
 * \brief Start a transfer for an out endpoint
 *
 * Based on the transfer values of the endpoint, the out endpoint registers will be programmed
 * to start a new out transfer.
 *
 * \note No checking of the transfer values are done in this function. Be sure,
 * that the transfer values are reasonable (e.g. buffer size is not exceeded).
 *
 * \param[in] ep Endpoint to start the transfer
 */
void XMC_StartReadXfer(xmc_usb_ep_t * ep) {
	deptsiz_data_t data = {.d32 = 0};

	if ((ep->rxTotal - ep->rxLength) > ep->maxPacketSize)
		ep->rxLength += ep->maxPacketSize;
	else
		ep->rxLength = ep->rxTotal;

	if (ep->number==0) {
		/* Setup the endpoint to recive 3 setup packages and one normal package.
		 * Cast the data pointer to use only one variable */
		((deptsiz0_data_t*)&data)->b.pktcnt = 0x1;
		((deptsiz0_data_t*)&data)->b.supcnt = 0x3;
		((deptsiz0_data_t*)&data)->b.xfersize = ep->rxTotal;
	}else {
		/* If requested length is zero, just receive one zero length packet */
		if (ep->rxLength==0) {
			data.b.xfersize = 0;
			data.b.pktcnt = 1;
		} else {
			/* setup endpoint to recive a amount of packages by given size */
			data.b.pktcnt = (((ep->rxLength - ep->rxCount) + ep->maxPacketSize -1)/ep->maxPacketSize);
			data.b.xfersize = (ep->rxLength - ep->rxCount);

		}
	}

	/* Programm dma address if needed */
	xmc_device.endpoint_out_register[ep->number]->doepdma = (uint32_t)ep->outBuffer;
	/* setup endpoint size and enable endpoint */
	xmc_device.endpoint_out_register[ep->number]->doeptsiz = data.d32;
	depctl_data_t epctl = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl };
	epctl.b.cnak = 1;
	epctl.b.epena = 1;

	xmc_device.endpoint_out_register[ep->number]->doepctl = epctl.d32;
}

/**
 * \brief Start a new in transfer
 *
 * Based on the transfer values of the endpoint the in endpoint registers will be programmed
 * to start a new in transfer
 *
 * \param[in] ep Endpoint to start the transfer
 */

void XMC_StartWriteXfer(xmc_usb_ep_t * ep) {
	deptsiz_data_t size = { .d32 = 0 };
	depctl_data_t ctl = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl };

  if (ep->txTotal - ep->txLength  < ep->maxPacketSize)
		ep->txLength += ep->txTotal - ep->txLength;
	else
		ep->txLength += ep->maxPacketSize;

	if (ep->txLength==0) {
		size.b.xfersize = 0;
		size.b.pktcnt = 1;
	}
	else {
		if (ep->number==0) {
			size.b.pktcnt = 1;
			size.b.xfersize = ep->txLength - ep->txCount; /* ep->maxXferSize equals maxPacketSize */
		}
		else {
			size.b.xfersize = ep->txLength - ep->txCount;
			size.b.pktcnt =((ep->txLength - ep->txCount + ep->maxPacketSize -1)/
									ep->maxPacketSize);
		}
		/* programm dma*/
    memcpy(ep->inBuffer,ep->txBuffer,size.b.xfersize);
		xmc_device.endpoint_in_register[ep->number]->diepdma = (uint32_t)ep->inBuffer;
	}

	/* programm size of transfer and enable endpoint */
	xmc_device.endpoint_in_register[ep->number]->dieptsiz = size.d32;

	ctl.b.epena = 1;
	ctl.b.cnak = 1;
	xmc_device.endpoint_in_register[ep->number]->diepctl = ctl.d32;

}

/**
 * @}
 */

/**
 * \internal
 * \defgroup USB_Device_Controller_Driver_Interrupt_Group Device Controller Interrupts
 * \ingroup USB_Device_Controller_Driver_Internals_Group
 * \brief Interrupt Functions
 *
 * Interrupt handler for the various kind of needed interrupts.
 * @{
 */

/**
 * \brief Handles the USB reset interrupt
 *
 * When ever the host sets the bus into reset condition the usb otg_core generates
 * an interrupt, which is handled by this function. It resets the complete otg_core
 * into the default state.
 */
void XMC_HandleUSBReset() {
	int i;
	depctl_data_t epctl;
	dctl_data_t dctl;
 //XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0);
 // XMC_SCU_POWER_DisableUsb();
	
	/* Clear the Remote Wakeup Signalling */
	dctl.d32 = xmc_device.device_register->dctl;
	dctl.b.rmtwkupsig = 1;
	xmc_device.device_register->dctl = dctl.d32;

	/* enable naks for all eps */
	for (i=0;i<NUM_EPS;i++) {
		epctl.d32 = xmc_device.endpoint_out_register[i]->doepctl;
		epctl.b.snak = 1;
		epctl.b.stall = 0;
		xmc_device.endpoint_out_register[i]->doepctl = epctl.d32;
	} 
	/* Configure fifos */
	/* Calculate the size of the rx fifo */
	xmc_device.global_register->grxfsiz = 64;
	/* Calculate the size of the tx fifo for ep 0 */
	fifosize_data_t gnptxfsiz = { .d32 = 0 };
	gnptxfsiz.b.depth = 16;
	gnptxfsiz.b.startaddr = 64;
	xmc_device.global_register->gnptxfsiz = gnptxfsiz.d32;
	/* calculate the size for the rest */
	for (i=1;i<USB_NUM_TX_FIFOS;i++) {
		xmc_device.global_register->dtxfsiz[i-1] = ((256 + i*(64))/4) |(16 << 16);
	}

	/* flush the fifos for proper operation */
	XMC_FlushTXFifo(0x10); /* 0x10 == all fifos, see doc */
	XMC_FlushTXFifo(0x0);
	XMC_FlushRXFifo();
	/* Flush learning queue not needed due to fifo config */

	/* enable ep0 interrupts */
	daint_data_t daint = { .d32 = 0 };
	daint.b.inep0 = 1;
	daint.b.outep0 = 1;
	xmc_device.device_register->daintmsk = daint.d32;

	/* enable endpoint interrupts */
	/* out ep interrupts */
	doepint_data_t doepint = { .d32 = 0 };
	doepint.b.setup = 1;
	doepint.b.xfercompl = 1;
	doepint.b.ahberr = 1;
	doepint.b.epdisabled = 1;
	xmc_device.device_register->doepmsk = doepint.d32;
	/*in ep interrupts */
	diepint_data_t diepint = { .d32 = 0 };
	diepint.b.timeout = 1;
	diepint.b.xfercompl = 1;
	diepint.b.ahberr = 1;
	diepint.b.epdisabled = 1;
	xmc_device.device_register->diepmsk = diepint.d32;

	/* Clear device Address */
	dcfg_data_t dcfg;
	dcfg.d32 = xmc_device.device_register->dcfg;
	dcfg.b.devaddr = 0;
	xmc_device.device_register->dcfg = dcfg.d32;
	
	/* Not in use any more */
	xmc_device.ep[0].isInUse = 0;
    xmc_device.ep[0].outInUse = 0;

	xmc_device.DeviceEvent_cb(ARM_USBD_EVENT_RESET);
		
	usbd_flags &= ~USB_INITIALIZED;	
		usbd_flags &= ~USB_POWERED;
		usbd_flags &= ~USB_CONNECTED;
		XMC_Initialize (xmc_device.DeviceEvent_cb ,xmc_device.EndpountEvent_cb);
		XMC_PowerControl(ARM_POWER_FULL);
		XMC_DeviceConnect ();
		xmc_device.IsPowered = 1;
		
	/* clear reset intr */
	gintsts_data_t gintsts = { .d32 = 0 };
	gintsts.b.usbreset = 1;
	xmc_device.global_register->gintsts = gintsts.d32;
  XMC_Endpoint0_OUT_Init(setup_buf, 8);

	
}

/**
 * \brief Interrupt handler for device enumeration done.
 *
 * Handles the enumeration done from dwc_otg, when the host has enumerated the device.
 */
void XMC_HandleEnumDone() {
	/* Normaly we need to check dctl
	 * We are always fullspeed, so max it up. */
	depctl_data_t epctl = { .d32 = xmc_device.endpoint_in_register[0]->diepctl };
	epctl.b.mps = 0x0; /* 64 Byte, this is also automatically set for out ep */
	xmc_device.endpoint_in_register[0]->diepctl = epctl.d32;
	
	/* update device connected flag */
	xmc_device.IsConnected = 1;
	xmc_device.IsPowered = 1;
	
	/* Set Trim */
	gusbcfg_data_t gusbcfg = { .d32 = xmc_device.global_register->gusbcfg };
	gusbcfg.b.usbtrdtim = 9; /* default value for LS/FS */
	xmc_device.global_register->gusbcfg = gusbcfg.d32;
	
	/* clear interrupt */
	gintsts_data_t gintsts = { .d32 = 0  };
	gintsts.b.enumdone = 1;
	xmc_device.global_register->gintsts = gintsts.d32;
}

/**
 * \brief Handles all interrupts for all out endpoints
 *
 * The interrupt handler first checks, which endpoint has caused the interrupt and then
 * determines, which interrupt should be handled.
 */
void XMC_HandleOEPInt() {
	daint_data_t daint = { .d32 = xmc_device.device_register->daint };
	daint_data_t daintmsk = { .d32 = xmc_device.device_register->daintmsk };
	doepmsk_data_t doepmsk = { .d32 = xmc_device.device_register->doepmsk };
	doepint_data_t doepint;
	doepint_data_t clear;
	deptsiz_data_t doeptsiz;
	xmc_usb_ep_t *ep;
	uint16_t mask = daint.ep.out & daintmsk.ep.out;
	uint8_t ep_num = 0;
	uint8_t receive_setup = 0;	
	while (mask >> ep_num) {
		if ((mask >> ep_num) & 1) {		
			/* load register data for endpoint */
			ep = &xmc_device.ep[ep_num];
			doepint.d32 = xmc_device.endpoint_out_register[ep_num]->doepint & doepmsk.d32;
			doeptsiz.d32 = xmc_device.endpoint_out_register[ep_num]->doeptsiz;

			/* Setup Phase Complete */
			if (doepint.b.setup) {
				/* ep0 not stalled any more */
				ep->isStalled = 0;
				/* calculate size for setup packet */
				ep->outBytesAvailable = (USB_SETUP_COUNT - ((deptsiz0_data_t*)&doeptsiz)->b.supcnt)*USB_SETUP_SIZE;
        // Analyze Setup packet for SetAddress
        if (ep->rxBuffer[0] == USB_REQUEST_STANDARD) {
          if (ep->rxBuffer[1] == USB_REQUEST_SET_ADDRESS)
            XMC_DeviceSetAddress(ep->rxBuffer[2]);
        }
				ep->outInUse = 0;
        setup_flag = 1;
  			xmc_device.EndpountEvent_cb(0,ARM_USBD_EVENT_SETUP); /* signal endpoint event */
				/* clear the interrupt */
				clear.d32 = 0;
				clear.b.setup = 1;
				xmc_device.endpoint_out_register[ep_num]->doepint = clear.d32;
			}
			if (doepint.b.xfercompl) {
				uint32_t bytes = (ep->rxLength - ep->rxCount - doeptsiz.b.xfersize);
				ep->rxCount += bytes;
				memcpy(ep->rxBuffer,ep->outBuffer,bytes); 
				ep->rxBuffer += bytes;

				if (ep->rxTotal==ep->rxLength) {				
					ep->outBytesAvailable = ep->rxCount;
					ep->outInUse = 0;
					xmc_device.EndpountEvent_cb(ep_num,ARM_USBD_EVENT_OUT);

					if ((ep_num == 0) && (ep->rxCount == 0)) receive_setup = 1;
				} else {
					XMC_StartReadXfer(ep);
				}

				/* clear the interrupt */
				clear.d32 = 0;
				clear.b.xfercompl = 1;
				xmc_device.endpoint_out_register[ep_num]->doepint = clear.d32;
			}
			if (doepint.b.epdisabled == 1) {
				clear.d32 = 0;
				clear.b.epdisabled = 1;
				xmc_device.endpoint_out_register[ep_num]->doepint = clear.d32;
			}
		}
		ep_num++;
	}
	/* clear interrupt */
	gintsts_data_t gintsts = { .d32 = 0 };
	gintsts.b.outepintr = 1;
	xmc_device.global_register->gintsts = gintsts.d32;
    if (receive_setup) XMC_Endpoint0_OUT_Init(setup_buf, 8);
}

/**
 * \internal
 * \brief Handles all interrupts for all in endpoints
 *
 * The interrupt handler first checks, which endpoint has caused the interrupt and then
 * determines, which interrupt should be handled.
 */
void XMC_HandleIEPInt() {
	daint_data_t daint = { .d32 = xmc_device.device_register->daint };
	diepmsk_data_t diepmsk = { .d32 = xmc_device.device_register->diepmsk };
	diepmsk_data_t clear;
	uint16_t mask = daint.ep.in;
	uint8_t ep_num = 0;
	uint8_t receive_setup = 0;

	while (mask >> ep_num) {
		if ((mask >> ep_num) & 0x1) {
			xmc_usb_ep_t *ep = &xmc_device.ep[ep_num];
			diepint_data_t diepint = { .d32 = xmc_device.endpoint_in_register[ep_num]->diepint & 
			((((xmc_device.device_register->dtknqr4_fifoemptymsk >> ep->number) & 0x1)<<7) | diepmsk.d32)};
			deptsiz_data_t dieptsiz = { .d32 = xmc_device.endpoint_in_register[ep_num]->dieptsiz };
      

			if (diepint.b.xfercompl) {

				/* update xfer values */
				if ((dieptsiz.b.pktcnt == 0) && (dieptsiz.b.xfersize == 0)) {
					uint32_t Bytes = ep->txLength - ep->txCount;
					ep->txCount += Bytes;
					ep->txBuffer += Bytes;
				}
				if (ep->txTotal==ep->txLength) {
					ep->isInUse = 0;
					xmc_device.EndpountEvent_cb(0x80 | ep_num,ARM_USBD_EVENT_IN);
					if ((ep_num == 0) && (ep->txCount == 0)) receive_setup = 1;
				} 
				else {
					/* start next step of transfer */
					XMC_StartWriteXfer(ep);
				}
				clear.d32 = 0;
				clear.b.xfercompl = 1;
				xmc_device.endpoint_in_register[ep_num]->diepint = clear.d32;
			}
			if (diepint.b.timeout == 1 ) {
				clear.d32 = 0;
				clear.b.timeout = 1;
				xmc_device.endpoint_in_register[ep_num]->diepint = clear.d32;
			}
			if (diepint.b.epdisabled == 1) {
				clear.d32 = 0;
				clear.b.epdisabled = 1;
				xmc_device.endpoint_in_register[ep_num]->diepint = clear.d32;
			}
		}
		ep_num++;
	}
	gintsts_data_t gintsts = { .d32 = 0 };
	gintsts.b.inepint = 1;
	xmc_device.global_register->gintsts = gintsts.d32;
	if (receive_setup) XMC_Endpoint0_OUT_Init(setup_buf, 8);
}

/**
 * \brief Handle OTG Interrupt
 *
 * It detects especially connect and disconnect events.
 */
void XMC_HandleOTGInt() {
	gotgint_data_t data = { .d32 = xmc_device.global_register->gotgint }, clear = { .d32 = 0};
#ifdef ARM_USBD_VBUS_DETECT
	if (data.b.sesenddet) {
		xmc_device.IsPowered = 0;
		xmc_device.DeviceEvent_cb(ARM_USBD_EVENT_VBUS_OFF);
		clear.d32 = 0;
		clear.b.sesenddet = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
#endif
	if (data.b.sesreqsucstschng) {
		clear.d32 = 0;
		clear.b.sesreqsucstschng = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
	if (data.b.hstnegsucstschng) {
		clear.d32 = 0;
		clear.b.hstnegsucstschng = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
	if (data.b.hstnegdet) {
		clear.d32 = 0;
		clear.b.hstnegdet = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
	if (data.b.adevtoutchng) {
		clear.d32 = 0;
		clear.b.adevtoutchng = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
	if (data.b.debdone) {
		clear.d32 = 0;
		clear.b.debdone = 1;
		xmc_device.global_register->gotgint = clear.d32;
	}
}

/**
 * \brief Global interrupt handler
 *
 * The handler first checks, which global interrupt has caused the interrupt
 * and then dispatches interrupt to the corresponding sub-handler.
 */
void XMC_HandleIrq() {
	gintmsk_data_t gintmsk = { .d32 = xmc_device.global_register->gintmsk };
	gintsts_data_t data = { .d32 = xmc_device.global_register->gintsts & gintmsk.d32  };
	gintsts_data_t clear;
	if (data.b.sofintr) {
		clear.d32 = 0;
		clear.b.sofintr = 1;
		xmc_device.global_register->gintsts = clear.d32;
	}
	if (data.b.erlysuspend) {
		clear.d32 = 0;
		clear.b.erlysuspend = 1;
		xmc_device.global_register->gintsts = clear.d32;
	}
	if (data.b.usbsuspend) {
        usbd_state.active = false;
		xmc_device.DeviceEvent_cb(ARM_USBD_EVENT_SUSPEND);
		clear.d32 = 0;
		clear.b.usbsuspend = 1;
		xmc_device.global_register->gintsts = clear.d32;
	}
	if (data.b.wkupintr) {
		xmc_device.DeviceEvent_cb(ARM_USBD_EVENT_RESUME);
		clear.d32 = 0;
		clear.b.wkupintr = 1;
		xmc_device.global_register->gintsts = clear.d32;
	}
#ifdef ARM_USBD_VBUS_DETECT
	if (data.b.sessreqintr) {

		usbd_flags &= ~USB_INITIALIZED;	
		usbd_flags &= ~USB_POWERED;
		usbd_flags &= ~USB_CONNECTED;
		XMC_Initialize (xmc_device.DeviceEvent_cb ,xmc_device.EndpountEvent_cb);
		XMC_PowerControl(ARM_POWER_FULL);
		XMC_DeviceConnect ();
		xmc_device.IsPowered = 1;
		clear.d32 = 0;
		clear.b.sessreqintr = 1;
		xmc_device.global_register->gintsts = clear.d32;
		xmc_device.DeviceEvent_cb(ARM_USBD_EVENT_VBUS_ON);
	}
#endif
	if (data.b.usbreset)
		XMC_HandleUSBReset();
	if (data.b.enumdone)
		XMC_HandleEnumDone();
	if (data.b.inepint)
		XMC_HandleIEPInt();
	if (data.b.outepintr)
		XMC_HandleOEPInt();
	if (data.b.otgintr)
		XMC_HandleOTGInt();
}

/**
 * \brief Entry point for exception vector table
 *
 * This function gets called, if an usb exception (interrupt) was thrown and
 * dispatches it to \ref XMC_HandleIrq.
 */
void USB0_0_IRQHandler() {
	XMC_HandleIrq();
}

/**
 * \brief  Enables and sets the priority of USB Interrupt
 *
 * First the interrupt priority is set and then the interrupt is enabled in the NVIC.
 */
static void XMC_EnableUSBInterrupt(void)
{
  NVIC_SetPriority(USB0_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),\
                                           XMC_PREEMPTION_PRIORITY,\
                                           XMC_SUB_PRIORITY));
  NVIC_ClearPendingIRQ(USB0_0_IRQn);
  NVIC_EnableIRQ(USB0_0_IRQn);
}

/**
 * \brief Disables the usb interrupt in the NVIC.
 *
 * Befor the interrupt gets disabled, it will also be cleared.
 */
static void XMC_DisableUSBInterrupt(void)
{
  NVIC_ClearPendingIRQ(USB0_0_IRQn);
  NVIC_DisableIRQ(USB0_0_IRQn);
}


/**
 * @}
 */

/**
  \fn          ARM_DRIVER_VERSION ARM_USBD_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
ARM_DRIVER_VERSION XMC_GetVersion() {
  return DriverVersion;
}

ARM_USBD_CAPABILITIES XMC_GetCapabilities() {
	ARM_USBD_CAPABILITIES cap;
#ifdef ARM_USBD_VBUS_DETECT
  cap.vbus_detection  = 1;         ///< VBUS detection
  cap.event_vbus_on   = 1;         ///< Signal VBUS On event
  cap.event_vbus_off  = 1;         ///< Signal VBUS Off event
#else
  cap.vbus_detection  = 0;         ///< VBUS detection
  cap.event_vbus_on   = 0;         ///< Signal VBUS On event
  cap.event_vbus_off  = 0;         ///< Signal VBUS Off event
#endif
	return cap;
}

int32_t XMC_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) {
	int i;
																	
  if (usbd_flags & USB_INITIALIZED){
    // Driver is already initialized
    return ARM_DRIVER_OK;
  }
	/* clear device status */
	memset(&xmc_device,0x0,sizeof(xmc_usb_device_t));
	
	/* assign callbacks */
	xmc_device.DeviceEvent_cb = cb_device_event;
	xmc_device.EndpountEvent_cb = cb_endpoint_event;
	
/* assign register address */
	xmc_device.global_register = (dwc_otg_core_global_regs_t*)(XMC_USB_BASE_ADDRESS);
	xmc_device.device_register = ((dwc_otg_device_global_regs_t*)(XMC_USB_BASE_ADDRESS + DWC_DEV_GLOBAL_REG_OFFSET));
	for (i=0;i< NUM_EPS;i++) {
		xmc_device.endpoint_in_register[i] = (dwc_otg_dev_in_ep_regs_t*)(XMC_USB_BASE_ADDRESS + DWC_DEV_IN_EP_REG_OFFSET +
				(DWC_EP_REG_OFFSET*i));
	}
	for (i=0;i< NUM_EPS;i++) {
		xmc_device.endpoint_out_register[i] = (dwc_otg_dev_out_ep_regs_t*)(XMC_USB_BASE_ADDRESS + DWC_DEV_OUT_EP_REG_OFFSET +
				(DWC_EP_REG_OFFSET*i));
	}
	for (i=0;i<USB_NUM_TX_FIFOS;i++)
		xmc_device.fifo[i] = (uint32_t*)(XMC_USB_BASE_ADDRESS + XMC_USB_TX_FIFO_REG_OFFSET +
			(i*XMC_USB_TX_FIFO_OFFSET));
    usbd_flags = USB_INITIALIZED;		
	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t XMC_Uninitialize (void)
  \brief       De-initialize USB Device Interface.
  \return      \ref execution_status
*/
static int32_t XMC_Uninitialize (void)
{
	XMC_DisableUSBInterrupt();

  /* Clear Reset and power up */
  XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0);
#if defined(CLOCK_GATING_SUPPORTED)
  XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USB0);
#endif
  XMC_SCU_POWER_DisableUsb();
	
	/* clean up */
	memset(&xmc_device,0,sizeof(xmc_device));
	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_USBD_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Device Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t  XMC_PowerControl(ARM_POWER_STATE state) {
 
  gahbcfg_data_t gahbcfg = { .d32 = xmc_device.global_register->gahbcfg };
  gusbcfg_data_t gusbcfg =  { .d32 = xmc_device.global_register->gusbcfg };
  dcfg_data_t dcfg = { .d32 = xmc_device.device_register->dcfg };
  dctl_data_t dctl = { .d32 = xmc_device.device_register->dctl };
	gintmsk_data_t gintmsk = { .d32 = 0 };
  switch (state)
  {
    case ARM_POWER_OFF:
    /* Clear Reset and power up */
    XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0);
#if defined(CLOCK_GATING_SUPPORTED)
    XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USB0);
#endif
    XMC_SCU_POWER_DisableUsb();
	    break;
		
    case ARM_POWER_LOW:	
      return ARM_DRIVER_ERROR_UNSUPPORTED;
		
		case ARM_POWER_FULL:
      if (usbd_flags & USB_POWERED)
			{	
        return ARM_DRIVER_OK;	
			}
#if defined(CLOCK_GATING_SUPPORTED)
		XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USB0);
#endif
		 /* Reset and power up */
		 XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USB0);
		 XMC_SCU_POWER_EnableUsb();	

	   /* configure ahb details */
	   gahbcfg.b.glblintrmsk = 1; /* enable interrupts ( global mask ) */
	   gahbcfg.b.nptxfemplvl_txfemplvl = 1;
	   /* enable dma */
	   gahbcfg.b.dmaenable = 1;
	   xmc_device.global_register->gahbcfg = gahbcfg.d32;
		 
	   /* configure usb details */
	   gusbcfg.b.srpcap = 1; /* enable session request protocoll */
	  gusbcfg.b.force_dev_mode= 1; /* force MAC to device */
	   xmc_device.global_register->gusbcfg = gusbcfg.d32;

	  /* Device init */
	  /* configure device speed */
	  dcfg.b.devspd = USB_DCFG_DevSpd_FS;
	  dcfg.b.descdma = 0;
	  xmc_device.device_register->dcfg = dcfg.d32;

	  /* configure device functions */
	  dctl.b.sftdiscon = 1; /* disconnect the device until its connected by the user */
	  /* all other config is done by default register value */
	  xmc_device.device_register->dctl = dctl.d32;
	  /* flush the fifos for proper operation */
	  XMC_FlushTXFifo(0x10); /* 0x10 == all fifos, see doc */
	  XMC_FlushRXFifo();
	
	  /* Enable Global Interrupts */
	  /* clear interrupt status bits prior to unmasking */
	  xmc_device.global_register->gintmsk = 0; /* diable all interrupts */
	  xmc_device.global_register->gintsts = 0xFFFFFFFF; /* clear all interrupts */
	  /* enable common interrupts */
	  gintmsk.b.modemismatch = 1;
	  gintmsk.b.otgintr = 1;
	  gintmsk.b.sessreqintr = 1;
	  /* enable device interrupts */
	  gintmsk.b.usbreset = 1;
	  gintmsk.b.enumdone = 1;
	  gintmsk.b.erlysuspend = 1;
	  gintmsk.b.usbsuspend = 1;
	  gintmsk.b.wkupintr = 1;
	  gintmsk.b.sofintr = 1;
	  gintmsk.b.outepintr = 1;
	  gintmsk.b.inepintr = 1;
	  xmc_device.global_register->gintmsk = gintmsk.d32;
	
	  /* Enable Interrupts in NVIC */
	  XMC_EnableUSBInterrupt();
	  usbd_flags = USB_INITIALIZED | USB_POWERED;
    break;

  default:
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_USBD_DeviceConnect (void)
  \brief       Connect USB Device.
  \return      \ref execution_status
*/
static int32_t XMC_DeviceConnect (void) {
  if ((usbd_flags & USB_POWERED) == 0) {
    // Device not powered
    return ARM_DRIVER_ERROR;
  }

  if (usbd_flags & USB_CONNECTED) {
    // Device is already connected
    return ARM_DRIVER_OK;
  }
	/* Just disable softdisconnect */
	dctl_data_t dctl = { .d32 = xmc_device.device_register->dctl };
	dctl.b.sftdiscon = 0;
	xmc_device.device_register->dctl = dctl.d32;
  usbd_flags |= USB_CONNECTED;
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_USBD_DeviceDisconnect (void)
  \brief       Disconnect USB Device.
  \return      \ref execution_status
*/
static int32_t XMC_DeviceDisconnect (void) {
	dctl_data_t dctl = { .d32 = xmc_device.device_register->dctl };
	  if ((usbd_flags & USB_POWERED) == 0) {
    // Device not powered
    return ARM_DRIVER_ERROR;
  }

  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is already disconnected
    return ARM_DRIVER_OK;
  }
	dctl.b.sftdiscon = 1;
	xmc_device.device_register->dctl = dctl.d32;

  usbd_state.active   =  false;
  usbd_flags         &= ~USB_CONNECTED;

	return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USBD_STATE ARM_USBD_DeviceGetState (void)
  \brief       Get current USB Device State.
  \return      Device State \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE XMC_DeviceGetState (void) {
  return usbd_state;
}

/**
  \fn          int32_t ARM_USBD_DeviceRemoteWakeup (void)
  \brief       Trigger USB Remote Wakeup.
  \return      \ref execution_status
*/
static int32_t XMC_DeviceRemoteWakeup (void) {
    if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
	return ARM_DRIVER_OK;
}
/**
  \fn          int32_t ARM_USBD_DeviceSetAddress (uint8_t dev_addr)
  \brief       Set USB Device Address.
  \param[in]   dev_addr  Device Address
  \return      \ref execution_status
*/
static int32_t XMC_DeviceSetAddress (uint8_t dev_addr) {
	dcfg_data_t data = { .d32 = xmc_device.device_register->dcfg };
	
   if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
	data.b.devaddr = dev_addr;
	xmc_device.device_register->dcfg = data.d32;
  
	return ARM_DRIVER_OK;
}

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
static int32_t XMC_EndpointStall (uint8_t ep_addr, bool stall)
{
	xmc_usb_ep_t *ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];
	
  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
	
	if (stall) {
		ep->isStalled = 1;
		if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
			/* if ep is enabled, disable ep and set stall bit */
			depctl_data_t data = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl };
			if (data.b.epena) {
				data.b.epdis = 1;
			}
			data.b.stall = 1;
			xmc_device.endpoint_in_register[ep->number]->diepctl = data.d32;
		} else {
			/* just set stall bit */
			depctl_data_t data = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl };
			data.b.stall = 1;
			xmc_device.endpoint_out_register[ep->number]->doepctl = data.d32;
		}
    XMC_Endpoint0_OUT_Init(setup_buf, 8);
	} else {
		/* just clear stall bit */
		ep->isStalled = 0;
		if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
			depctl_data_t data = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl };
			data.b.stall = 0;
			data.b.setd0pid = 1; /* reset pid to 0 */
			xmc_device.endpoint_in_register[ep->number]->diepctl = data.d32;
		} else {
			depctl_data_t data = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl };
			data.b.stall = 0;
			data.b.setd0pid = 1; /* reset pid to 0 */
			xmc_device.endpoint_out_register[ep->number]->doepctl = data.d32;
		}
	}
	return ARM_DRIVER_OK;
}

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
static int32_t XMC_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num) {
 xmc_usb_ep_t *ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];

	
  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
	if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {

  	if (!ep->isConfigured)
  		return ARM_DRIVER_ERROR;
  	if (ep->isInUse == 1)
  		return 0;
		
  	/* copy data into input buffer for dma mode */
  	ep->txBuffer = data;
  	ep->txTotal  = num;
  	/* set transfer values */
  	ep->txLength = 0;
  	ep->txCount = 0;
  	ep->isInUse = 1;

  	/* start the transfer */
  	XMC_StartWriteXfer(ep);
  } 
	else
		{		
  	if (ep->outInUse || !ep->isConfigured)
  		return ARM_DRIVER_ERROR;	
			/* set ep values */	
    ep->rxBuffer = data;
		ep->rxTotal = num;
		
  	ep->rxCount =0;
  	ep->rxLength =0;
		ep->outInUse=1;
	
  XMC_StartReadXfer(ep);
  
		}
  return ARM_DRIVER_OK;
}

static int32_t XMC_Endpoint0_OUT_Init (uint8_t *buf, uint32_t num) {
  xmc_usb_ep_t *ep = &xmc_device.ep[0];

  /* set ep values */
  ep->rxTotal = num;
  ep->rxCount = 0;
  ep->rxLength = 0;
  ep->rxBuffer = ep->outBuffer = buf;
  ep->outBytesAvailable = 0;
  
	deptsiz_data_t data = {.d32 = 0};

	if ((ep->rxTotal - ep->rxLength) > ep->maxPacketSize)
		ep->rxLength += 8;
	else
		ep->rxLength = ep->rxTotal;
		/* Setup the endpoint to recive 3 setup packages and one normal package.
		 * Cast the data pointer to use only one variable */
		((deptsiz0_data_t*)&data)->b.pktcnt = 0x1;
		((deptsiz0_data_t*)&data)->b.supcnt = 0x3;
		((deptsiz0_data_t*)&data)->b.xfersize = ep->rxTotal;
	/* Programm dma address if needed */
	xmc_device.endpoint_out_register[ep->number]->doepdma = (uint32_t)ep->rxBuffer;
	/* setup endpoint size and enable endpoint */
	xmc_device.endpoint_out_register[ep->number]->doeptsiz = data.d32;
	depctl_data_t epctl = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl };
	epctl.b.cnak = 1;
	epctl.b.epena = 1;

	xmc_device.endpoint_out_register[ep->number]->doepctl = epctl.d32;

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t ARM_USBD_EndpointTransferGetResult (uint8_t ep_addr)
  \brief       Get result of USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      number of successfully transfered data bytes
*/
static uint32_t XMC_EndpointTransferGetResult (uint8_t ep_addr) {
	xmc_usb_ep_t * ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];
  if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
    return ep->txLength;
  } else {
    return ep->rxCount;
  }
}

/**
  \fn          int32_t XMC_ReadSetupPacket (uint8_t *setup)
  \brief       Read setup packet received over Control Endpoint.
  \param[out]  setup  Pointer to buffer for setup packet
  \return      \ref execution_status
*/
uint8_t setup_cnt = 0;
static int32_t XMC_ReadSetupPacket (uint8_t *setup) {
  xmc_usb_ep_t *ep = &xmc_device.ep[0];
  if (setup_flag == 0) {
    // No setup packet waitting
    return ARM_DRIVER_ERROR;
  }
  
  if (++setup_cnt > 1) {
    __nop();
  }
  setup_flag = 0;
  memcpy(setup, ep->rxBuffer, 8);

  if (setup_flag) {
    // Interrupted with new setup packet
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t ARM_USBD_EndpointTransferAbort (uint8_t ep_addr)
  \brief       Abort current USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t XMC_EndpointTransferAbort (uint8_t ep_addr) {

	xmc_usb_ep_t *ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];
  uint8_t dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) ? 1 : 0; 
  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
  if (dir) {
  	depctl_data_t data = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl};
  	data.b.stall = 0;
  	data.b.setd0pid = 0;
    //if (ep->number != 0)
    if (data.b.epena)
  	  data.b.epdis = 1;
  	data.b.snak = 1;
  	xmc_device.endpoint_in_register[ep->number]->diepctl = data.d32;
	  XMC_FlushTXFifo(ep->number);

  }
  else {
  	depctl_data_t data = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl};
  	data.b.stall = 0;
  	data.b.setd0pid = 0;
    //if (ep->number != 0)
    if (data.b.epena)
  	  data.b.epdis = 1;
		data.b.snak = 1;
  	xmc_device.endpoint_out_register[ep->number]->doepctl = data.d32;
  }
  ep->isStalled = 0;
  if (dir) {
	  ep->isInUse = 0;
    ep->txLength = 0;
    ep->txCount  = 0;
    ep->txTotal  = 0;
  }
  else {
    ep->outInUse = 0;
    ep->outBytesAvailable = 0;
    ep->outOffset = 0;
    ep->rxLength = 0;
    ep->rxCount  = 0;
    ep->rxTotal  = 0;
  }
	return ARM_DRIVER_OK;
}

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
                                       uint16_t ep_max_packet_size) 
{
  daint_data_t daintmsk = { .d32 = xmc_device.device_register->daintmsk };
	xmc_usb_ep_t *ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];
  int max_transfer_size = (((1<<10)-1)* ep_max_packet_size);
  uint8_t dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) ? 1 : 0; 

  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
  
	/* do ep configuration */
	ep->address = ep_addr;
	ep->isConfigured = 1;
	ep->maxPacketSize = ep_max_packet_size;
	ep->maxTransferSize = max_transfer_size;

	/* is in */
  if (dir) {
	  /* transfer buffer */
    ep->inBuffer = USBD_EP_IN_BUFFER[ep->number];

		depctl_data_t data = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl };
		/*enable endpoint */
		data.b.usbactep = 1;
		/* set ep type */
		data.b.eptype = ep_type;
		/* set mps */
    if (ep_type == ARM_USB_ENDPOINT_CONTROL) {
			
      switch(ep_max_packet_size) {
        case(64):
          data.b.mps = 0x0;
          break;
        case(32):
          data.b.mps = 0x1;
          break;
        case(16):
          data.b.mps = 0x2;
          break;
        case(8):
          data.b.mps = 0x3;
          break;
      }
    }
    else {
      data.b.mps = ep_max_packet_size;
    }
		/* set first data0 pid */
		data.b.setd0pid = 1;
		data.b.snak = 0;
		data.b.txfnum = ep->number; //ep->txFifoNum;
		xmc_device.endpoint_in_register[ep->number]->diepctl = data.d32; /* configure endpoint */
		daintmsk.ep.in |= (1<<ep->number); /* enable interrupts for endpoint */
	}
  else {
	/* transfer buffer */
	  ep->outBuffer = USBD_EP_OUT_BUFFER[ep->number];
		/* is out */
		depctl_data_t data = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl };
		/*enable endpoint */
		data.b.usbactep = 1;
		/* set ep type */
		data.b.eptype = ep_type;
		/* set mps */
    if (ep_type == ARM_USB_ENDPOINT_CONTROL) {
      switch(ep_max_packet_size) {
        case(64):
          data.b.mps = 0x0;
          break;
        case(32):
          data.b.mps = 0x1;
          break;
        case(16):
          data.b.mps = 0x2;
          break;
        case(8):
          data.b.mps = 0x3;
          break;
      }
    }
    else {
      data.b.mps = ep_max_packet_size;
    }
		/* set first data0 pid */
    data.b.setd0pid = 1;
    data.b.snak     = 1;
		xmc_device.endpoint_out_register[ep->number]->doepctl = data.d32; /* configure endpoint */
		daintmsk.ep.out |= (1<<ep->number); /* enable interrupts */
    
	}
	xmc_device.device_register->daintmsk = daintmsk.d32;
	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_USBD_EndpointUnconfigure (uint8_t ep_addr)
  \brief       Unconfigure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t XMC_EndpointUnconfigure (uint8_t ep_addr)
{
	xmc_usb_ep_t *ep = &xmc_device.ep[ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK];
	daint_data_t daintmsk = { .d32 = xmc_device.device_register->daintmsk };
  uint8_t dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) ? 1 : 0; 
  
  if ((usbd_flags & USB_CONNECTED) == 0) {
    // Device is not connected
    return ARM_DRIVER_ERROR;
  }
	/* chose register based on the direction. Control Endpoint need both */
  if (dir) {
  	depctl_data_t data = { .d32 = xmc_device.endpoint_in_register[ep->number]->diepctl};
    /* disable the endpoint, deactivate it and only send naks */
    if (data.b.epena)
  	  data.b.epdis = 1;
    data.b.snak = 1;
    xmc_device.endpoint_in_register[ep->number]->diepctl = data.d32; /* disable endpoint configuration */
    daintmsk.ep.in &= ~(1 << ep->number);                            /* disable interrupts */
	}
  else {
  	depctl_data_t data = { .d32 = xmc_device.endpoint_out_register[ep->number]->doepctl};
    /* disable the endpoint, deactivate it and only send naks */
    if (data.b.epena)
  	  data.b.epdis = 1;
    data.b.snak = 1;
    xmc_device.endpoint_out_register[ep->number]->doepctl = data.d32;
    daintmsk.ep.out &= ~(1 << ep->number);
	}
	xmc_device.device_register->daintmsk = daintmsk.d32;
	
	return ARM_DRIVER_OK;
}

uint16_t XMC_GetFrameNumber(void) {
	dsts_data_t dsts = { .d32 = xmc_device.device_register->dsts };
	return dsts.b.soffn;
}

/* the xmc_device driver */
const ARM_DRIVER_USBD Driver_USBD0 = {
		.GetVersion = XMC_GetVersion,
		.GetCapabilities = XMC_GetCapabilities,
		.Initialize = XMC_Initialize,
		.Uninitialize = XMC_Uninitialize,
		.PowerControl = XMC_PowerControl,
		.DeviceConnect = XMC_DeviceConnect,
		.DeviceDisconnect = XMC_DeviceDisconnect,
		.DeviceGetState = XMC_DeviceGetState,
		.DeviceRemoteWakeup = XMC_DeviceRemoteWakeup,
  	.DeviceSetAddress = XMC_DeviceSetAddress,
    .ReadSetupPacket     = XMC_ReadSetupPacket,
  	.EndpointConfigure = XMC_EndpointConfigure,
		.EndpointUnconfigure = XMC_EndpointUnconfigure,
    .EndpointStall = XMC_EndpointStall,
	  .EndpointTransfer     = XMC_EndpointTransfer,
	  .EndpointTransferGetResult     = XMC_EndpointTransferGetResult,
		.EndpointTransferAbort = XMC_EndpointTransferAbort,
    .GetFrameNumber = XMC_GetFrameNumber
};
