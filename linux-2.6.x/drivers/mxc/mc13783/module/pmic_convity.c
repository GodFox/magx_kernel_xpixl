/*
 * Copyright (c) 2006 Freescale Semiconductor, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 *
 */

/*!
 * @file pmic_convity.c
 * @brief Implementation of the PMIC Connectivity driver APIs.
 *
 * The PMIC connectivity device driver and this API were developed to support
 * the external connectivity capabilities of several power management ICs that
 * are available from Freescale Semiconductor, Inc. In particular, both the
 * mc13783 and Phoenix sc55112 power management ICs are currently supported.
 * All of the common features of each power management IC are available as
 * well as all device-specific features. However, attempting to use a device-
 * specific feature on a platform on which it is not supported will simply
 * return an error status.
 *
 * The following operating modes, in terms of external connectivity, are
 * supported:
 *
 * @verbatim
       Operating Mode     mc13783   sc55112
       ---------------    -----   ----------
       USB (incl. OTG)     Yes       Yes
       RS-232              Yes       Yes
       CEA-936             Yes       No

   @endverbatim
 *
 * The USB, including USB On-the-Go (OTG), and RS-232 operating modes are
 * quite similar between the mc13783 and sc55112 power management ICs and
 * it is possible to configure and operate them in an identical manner.
 * However, the mc13783 hardware does support some additional USB and RS-232
 * configuration options as well as the CE-936 operating mode.
 *
 * This file implements only the mc13783-specific features.
 *
 * For complete technical information concerning the Freescale power
 * management ICs, please refer to the following documents:
 *
 * [1] MC13783 Detailed Technical Specification (Level 3), Rev 1.3 -
 *     04/09/17. Freescale Semiconductor, Inc.
 * [2] iDEN Phoenix Platform Level 3 Detailed Technical Specification,
 *     Rev 2.1 11-15-2004. Freescale Semiconductor, Inc.
 *
 * For information about the USB and USB-on-the-Go protocol, please refer to
 * the following documents:
 *
 * [3] Universal Serial Bus Specification, Revision 2.0, April 27, 2000,
 *     USB Implementers Forum, Inc.
 * [4] On-The-Go Supplement to the USB 2.0 Specification, Revision 1.0a,
 *     June 24, 2003. USB Implementers Forum, Inc.
 *
 * @ingroup PMIC_MC13783_CONNECTIVITY
 */

#include <asm/arch/pmic_convity.h>	/* For PMIC Connectivity driver interface. */
#include <asm/arch/pmic_external.h>	/* For PMIC Protocol driver interface.     */
#include <asm/arch/pmic_adc.h>	/* For PMIC ADC driver interface.          */

#include <linux/interrupt.h>	/* For tasklet interface.                  */
#include <linux/device.h>	/* For kernel module interface.            */
#include <linux/spinlock.h>	/* For spinlock interface.                 */
#include "../core/pmic_config.h"
/*
 * mc13783 Connectivity API
 */
/* EXPORTED FUNCTIONS */
EXPORT_SYMBOL(pmic_convity_open);
EXPORT_SYMBOL(pmic_convity_close);
EXPORT_SYMBOL(pmic_convity_set_mode);
EXPORT_SYMBOL(pmic_convity_get_mode);
EXPORT_SYMBOL(pmic_convity_reset);
EXPORT_SYMBOL(pmic_convity_set_callback);
EXPORT_SYMBOL(pmic_convity_clear_callback);
EXPORT_SYMBOL(pmic_convity_get_callback);
EXPORT_SYMBOL(pmic_convity_usb_set_speed);
EXPORT_SYMBOL(pmic_convity_usb_get_speed);
EXPORT_SYMBOL(pmic_convity_usb_set_power_source);
EXPORT_SYMBOL(pmic_convity_usb_get_power_source);
EXPORT_SYMBOL(pmic_convity_usb_set_xcvr);
EXPORT_SYMBOL(pmic_convity_usb_get_xcvr);
EXPORT_SYMBOL(pmic_convity_usb_otg_set_dlp_duration);
EXPORT_SYMBOL(pmic_convity_usb_otg_get_dlp_duration);
EXPORT_SYMBOL(pmic_convity_usb_otg_set_config);
EXPORT_SYMBOL(pmic_convity_usb_otg_clear_config);
EXPORT_SYMBOL(pmic_convity_usb_otg_get_config);
EXPORT_SYMBOL(pmic_convity_rs232_set_config);
EXPORT_SYMBOL(pmic_convity_rs232_get_config);
EXPORT_SYMBOL(pmic_convity_cea936_exit_signal);

#ifdef DEBUG_CONVITY
#define TRACEMSG_CONVITY(fmt,args...)  printk(fmt,##args)
#else				/* DEBUG */
#define TRACEMSG(fmt,args...)
#endif				/* DEBUG */

/*! @def SET_BITS
 * Set a register field to a given value.
 */

#define SET_BITS(reg, field, value)    (((value) << reg.field.offset) & \
                                        reg.field.mask)

/*! @def GET_BITS
 * Get the current value of a given register field.
 */
#define GET_BITS(reg, value)    (((value) & reg.mask) >> \
                                 reg.offset)

/*!
 * @brief Define the possible states for a device handle.
 *
 * This enumeration is used to track the current state of each device handle.
 */
typedef enum {
	HANDLE_FREE,		/*!< Handle is available for use. */
	HANDLE_IN_USE		/*!< Handle is currently in use.  */
} HANDLE_STATE;

/*
 * This structure is used to define a specific hardware register field.
 *
 * All hardware register fields are defined using an offset to the LSB
 * and a mask. The offset is used to right shift a register value before
 * applying the mask to actually obtain the value of the field.
 */
typedef struct {
	const unsigned char offset;	/* Offset of LSB of register field.           */
	const unsigned int mask;	/* Mask value used to isolate register field. */
} REGFIELD;

/*!
 * @brief This structure is used to identify the fields in the USBCNTRL_REG_0 hardware register.
 *
 * This structure lists all of the fields within the USBCNTRL_REG_0 hardware
 * register.
 */
typedef struct {
	REGFIELD FSENB;		/*!< USB Full Speed Enable                            */
	REGFIELD USB_SUSPEND;	/*!< USB Suspend Mode Enable                          */
	REGFIELD USB_PU;	/*!< USB Pullup Enable                                */
	REGFIELD UDP_PD;	/*!< USB Data Plus Pulldown Enable                    */
	REGFIELD UDM_PD;	/*!< USB 150K UDP Pullup Enable                       */
	REGFIELD DP150K_PU;	/*!< USB Pullup/Pulldown Override Enable              */
	REGFIELD VBUSPDENB;	/*!< USB VBUS Pulldown NMOS Switch Enable             */
	REGFIELD CURRENT_LIMIT;	/*!< USB Regulator Current Limit Setting-3 bits       */
	REGFIELD DLP_SRP;	/*!< USB Data Line Pulsing Timer Enable               */
	REGFIELD SE0_CONN;	/*!< USB Pullup Connect When SE0 Detected             */
	REGFIELD USBXCVREN;	/*!< USB Transceiver Enabled When INTERFACE_MODE[2:0]=000 and RESETB=high */
	REGFIELD PULLOVR;	/*!< 1K5 Pullup and UDP/UDM Pulldown Disable When UTXENB=Low             */
	REGFIELD INTERFACE_MODE;	/*!< Connectivity Interface Mode Select-3 Bits        */
	REGFIELD DATSE0;	/*!< USB Single or Differential Mode Select           */
	REGFIELD BIDIR;		/*!< USB Unidirectional/Bidirectional Transmission    */
	REGFIELD USBCNTRL;	/*!< USB Mode of Operation controlled By USBEN/SPI Pin */
	REGFIELD IDPD;		/*!< USB UID Pulldown Enable                          */
	REGFIELD IDPULSE;	/*!< USB Pulse to Gnd on UID Line Generated           */
	REGFIELD IDPUCNTRL;	/*!< USB UID Pin pulled high By 5ua Curr Source       */
	REGFIELD DMPULSE;	/*!< USB Positive pulse on the UDM Line Generated     */
} USBCNTRL_REG_0;

/*!
 * @brief This variable is used to access the USBCNTRL_REG_0 hardware register.
 *
 * This variable defines how to access all of the fields within the
 * USBCNTRL_REG_0  hardware register. The initial values consist of the offset
 * and mask values needed to access each of the register fields.
 */
static const USBCNTRL_REG_0 regUSB0 = {
	{0, 0x000001},		/*!< FSENB        */
	{1, 0x000002},		/*!< USB_SUSPEND  */
	{2, 0x000004},		/*!< USB_PU       */
	{3, 0x000008},		/*!< UDP_PD       */
	{4, 0x000010},		/*!< UDM_PD       */
	{5, 0x000020},		/*!< DP150K_PU    */
	{6, 0x000040},		/*!< VBUSPDENB    */
	{7, 0x000380},		/*!< CURRENT_LIMIT */
	{10, 0x000400},		/*!< DLP_SRP      */
	{11, 0x000800},		/*!< SE0_CONN     */
	{12, 0x001000},		/*!< USBXCVREN    */
	{13, 0x002000},		/*!< PULLOVR      */
	{14, 0x01c000},		/*!< INTERFACE_MODE */
	{17, 0x020000},		/*!< DATSE0       */
	{18, 0x040000},		/*!< BIDIR        */
	{19, 0x080000},		/*!< USBCNTRL     */
	{20, 0x100000},		/*!< IDPD         */
	{21, 0x200000},		/*!< IDPULSE      */
	{22, 0x400000},		/*!< IDPUCNTRL    */
	{23, 0x800000}		/*!< DMPULSE      */

};

/*!
 * @brief This structure is used to identify the fields in the USBCNTRL_REG_1 hardware register.
 *
 * This structure lists all of the fields within the USBCNTRL_REG_1 hardware
 * register.
 */
typedef struct {
	REGFIELD VUSBIN;	/*!< Controls The Input Source For VUSB             */
	REGFIELD VUSB;		/*!< VUSB Output Voltage Select-High=3.3V Low=2.775V */
	REGFIELD VUSBEN;	/*!< VUSB Output Enable-                            */
	REGFIELD VBUSEN;	/*!< VBUS Output Enable-                            */
	REGFIELD RSPOL;		/*!< Low=RS232 TX on UDM, RX on UDP
				   High= RS232 TX on UDP, RX on UDM               */
	REGFIELD RSTRI;		/*!< TX Forced To Tristate in RS232 Mode Only       */
	REGFIELD ID100kPU;	/*!< 100k UID Pullup Enabled                        */
} USBCNTRL_REG_1;

/*!
 * @brief This variable is used to access the USBCNTRL_REG_1 hardware register.
 *
 * This variable defines how to access all of the fields within the
 * USBCNTRL_REG_1  hardware register. The initial values consist of the offset
 * and mask values needed to access each of the register fields.
 */
static const USBCNTRL_REG_1 regUSB1 = {
	{0, 0x000003},		/*!< VUSBIN-2 Bits  */
	{2, 0x000004},		/*!< VUSB           */
	{3, 0x000008},		/*!< VUSBEN         */
	/*{4, 0x000010} *//*!< Reserved       */
	{5, 0x000020},		/*!< VBUSEN         */
	{6, 0x000040},		/*!< RSPOL          */
	{7, 0x000080},		/*!< RSTRI          */
	{8, 0x000100}		/*!< ID100kPU       */
	/*!< 9-23 Unused    */
};

/*! Define a mask to access the entire hardware register. */
static const unsigned int REG_FULLMASK = 0xffffff;

/*! Define the mc13783 USBCNTRL_REG_0 register power on reset state. */
static const unsigned int RESET_USBCNTRL_REG_0 = 0x080060;

/*! Define the mc13783 USBCNTRL_REG_1 register power on reset state. */
static const unsigned int RESET_USBCNTRL_REG_1 = 0x000006;

static type_event_notification eventNotify;

/*!
 * @brief This structure is used to maintain the current device driver state.
 *
 * This structure maintains the current state of the connectivity driver. This
 * includes both the PMIC hardware state as well as the device handle and
 * callback states.
 */

typedef struct {
	PMIC_CONVITY_HANDLE handle;	/*!< Device handle.   */
	HANDLE_STATE handleState;	/*!< Device handle
					   state.           */
	PMIC_CONVITY_MODE mode;	/*!< Device mode.     */
	PMIC_CONVITY_CALLBACK callback;	/*!< Event callback function pointer. */
	PMIC_CONVITY_EVENTS eventMask;	/*!< Event mask.      */
	PMIC_CONVITY_USB_SPEED usbSpeed;	/*!< USB connection
						   speed.           */
	PMIC_CONVITY_USB_MODE usbMode;	/*!< USB connection
					   mode.            */
	PMIC_CONVITY_USB_POWER_IN usbPowerIn;	/*!< USB transceiver
						   power source.    */
	PMIC_CONVITY_USB_POWER_OUT usbPowerOut;	/*!< USB transceiver
						   power output
						   level.           */
	PMIC_CONVITY_USB_TRANSCEIVER_MODE usbXcvrMode;	/*!< USB transceiver
							   mode.            */
	unsigned int usbDlpDuration;	/*!< USB Data Line
					   Pulsing duration. */
	PMIC_CONVITY_USB_OTG_CONFIG usbOtgCfg;	/*!< USB OTG
						   configuration
						   options.         */
	PMIC_CONVITY_RS232_INTERNAL rs232CfgInternal;	/*!< RS-232 internal
							   connections.     */
	PMIC_CONVITY_RS232_EXTERNAL rs232CfgExternal;	/*!< RS-232 external
							   connections.     */
} PMIC_CONVITY_STATE_STRUCT;

/*!
 * @brief Identifies the hardware interrupt source.
 *
 * This enumeration identifies which of the possible hardware interrupt
 * sources actually caused the current interrupt handler to be called.
 */
typedef enum {
	CORE_EVENT_4V4,		/*!< Detected USB 4.4 V event.              */
	CORE_EVENT_2V0,		/*!< Detected USB 2.0 V event.              */
	CORE_EVENT_0V8,		/*!< Detected USB 0.8 V event.              */
	CORE_EVENT_ABDET	/*!< Detected USB mini A-B connector event. */
} PMIC_CORE_EVENT;

/*! Define the maximum Data Line Pulse duration in milliseconds. */
//static const unsigned int MAX_DLP_DURATION_MS = 7;

/*!
 * @brief This structure defines the reset/power on state for the Connectivity driver.
 */
static const PMIC_CONVITY_STATE_STRUCT reset = {
	0,
	HANDLE_FREE,
	USB,
	NULL,
	0,
	USB_FULL_SPEED,
	USB_HOST,
	USB_POWER_INTERNAL,
	USB_POWER_3V3,
	USB_TRANSCEIVER_OFF,
	0,
	USB_PULL_OVERRIDE | USB_VBUS_CURRENT_LIMIT_HIGH,
	RS232_TX_USE0VM_RX_UDATVP,
	RS232_TX_UDM_RX_UDP
};

/*!
 * @brief This structure maintains the current state of the Connectivity driver.
 *
 * The initial values must be identical to the reset state defined by the
 * #reset variable.
 */
static PMIC_CONVITY_STATE_STRUCT convity = {
	0,
	HANDLE_FREE,
	USB,
	NULL,
	0,
	USB_FULL_SPEED,
	USB_HOST,
	USB_POWER_INTERNAL,
	USB_POWER_3V3,
	USB_TRANSCEIVER_OFF,
	0,
	USB_PULL_OVERRIDE | USB_VBUS_CURRENT_LIMIT_HIGH,
	RS232_TX_USE0VM_RX_UDATVP,
	RS232_TX_UDM_RX_UDP
};

/*!
 * @brief This spinlock is used to provide mutual exclusion.
 *
 * Create a spinlock that can be used to provide mutually exclusive
 * read/write access to the globally accessible "convity" data structure
 * that was defined above. Mutually exclusive access is required to
 * ensure that the convity data structure is consistent at all times
 * when possibly accessed by multiple threads of execution (for example,
 * while simultaneously handling a user request and an interrupt event).
 *
 * We need to use a spinlock sometimes because we need to provide mutual
 * exclusion while handling a hardware interrupt.
 */
static spinlock_t lock = SPIN_LOCK_UNLOCKED;

/*!
 * @brief This mutex is used to provide mutual exclusion.
 *
 * Create a mutex that can be used to provide mutually exclusive
 * read/write access to the globally accessible data structures
 * that were defined above. Mutually exclusive access is required to
 * ensure that the Connectivity data structures are consistent at all
 * times when possibly accessed by multiple threads of execution.
 *
 * Note that we use a mutex instead of the spinlock whenever disabling
 * interrupts while in the critical section is not required. This helps
 * to minimize kernel interrupt handling latency.
 */
static DECLARE_MUTEX(mutex);

/* Prototype for the connectivity driver tasklet function. */
static void pmic_convity_tasklet(unsigned long arg);

/*!
 * @brief Tasklet handler for the connectivity driver.
 *
 * Declare a tasklet that will do most of the processing for all of the
 * connectivity-related interrupt events (USB4.4VI, USB2.0VI, USB0.8VI,
 * and AB_DETI). Note that we cannot do all of the required processing
 * within the interrupt handler itself because we may need to call the
 * ADC driver to measure voltages as well as calling any user-registered
 * callback functions.
 */
DECLARE_TASKLET(convityTasklet, pmic_convity_tasklet, 0);

/*!
 * @brief Global variable to track currently active interrupt events.
 *
 * This global variable is used to keep track of all of the currently
 * active interrupt events for the connectivity driver. Note that access
 * to this variable may occur while within an interrupt context and,
 * therefore, must be guarded by using a spinlock.
 */
static PMIC_CORE_EVENT eventID = 0;

/* Prototypes for all static connectivity driver functions. */
static PMIC_STATUS pmic_convity_set_mode_internal(const PMIC_CONVITY_MODE mode);
static PMIC_STATUS pmic_convity_deregister_all(void);
static void pmic_convity_event_handler(void *param);

/**************************************************************************
 * General setup and configuration functions.
 **************************************************************************
 */

/*!
 * @name General Setup and Configuration Connectivity APIs
 * Functions for setting up and configuring the connectivity hardware.
 */
/*@{*/

/*!
 * Attempt to open and gain exclusive access to the PMIC connectivity
 * hardware. An initial operating mode must also be specified.
 *
 * If the open request is successful, then a numeric handle is returned
 * and this handle must be used in all subsequent function calls. The
 * same handle must also be used in the pmic_convity_close() call when use
 * of the PMIC connectivity hardware is no longer required.
 *
 * @param       handle          device handle from open() call
 * @param       mode            initial connectivity operating mode
 *
 * @return      PMIC_SUCCESS    if the open request was successful
 */
PMIC_STATUS pmic_convity_open(PMIC_CONVITY_HANDLE * const handle,
			      const PMIC_CONVITY_MODE mode)
{
	PMIC_STATUS rc = PMIC_ERROR;

	if (handle == (PMIC_CONVITY_HANDLE *) NULL) {
		/* Do not dereference a NULL pointer. */
		return PMIC_ERROR;
	}

	/* We only need to acquire a mutex here because the interrupt handler
	 * never modifies the device handle or device handle state. Therefore,
	 * we don't need to worry about conflicts with the interrupt handler
	 * or the need to execute in an interrupt context.
	 *
	 * But we do need a critical section here to avoid problems in case
	 * multiple calls to pmic_convity_open() are made since we can only
	 * allow one of them to succeed.
	 */
	down_interruptible(&mutex);

	/* Check the current device handle state and acquire the handle if
	 * it is available.
	 */
	if (convity.handleState != HANDLE_FREE) {
		/* Cannot open the PMIC connectivity hardware at this time or an invalid
		 * mode was requested.
		 */
		*handle = reset.handle;
	} else {
		/* Let's begin by acquiring the connectivity device handle. */
		convity.handle = (PMIC_CONVITY_HANDLE) (&convity);
		convity.handleState = HANDLE_IN_USE;
		/* Then we can try to set the desired operating mode. */
		rc = pmic_convity_set_mode_internal(mode);

		if (rc == PMIC_SUCCESS) {
			/* Successfully set the desired operating mode, now return the
			 * handle to the caller.
			 */
			*handle = convity.handle;
		} else {
			/* Failed to set the desired mode, return the handle to an unused
			 * state.
			 */
			convity.handle = reset.handle;
			convity.handleState = reset.handleState;

			*handle = reset.handle;
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Terminate further access to the PMIC connectivity hardware. Also allows
 * another process to call pmic_convity_open() to gain access.
 *
 * @param       handle          device handle from open() call
 *
 * @return      PMIC_SUCCESS    if the close request was successful
 */
PMIC_STATUS pmic_convity_close(const PMIC_CONVITY_HANDLE handle)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Begin a critical section here to avoid the possibility of race
	 * conditions if multiple threads happen to call this function and
	 * pmic_convity_open() at the same time.
	 */
	down_interruptible(&mutex);

	/* Confirm that the device handle matches the one assigned in the
	 * pmic_convity_open() call and then close the connection.
	 */
	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		rc = PMIC_SUCCESS;

		/* Deregister for all existing callbacks if necessary and make sure
		 * that the event handling settings are consistent following the
		 * close operation.
		 */
		if (convity.callback != reset.callback) {
			/* Deregister the existing callback function and all registered
			 * events before we completely close the handle.
			 */
			rc = pmic_convity_deregister_all();

		} else if (convity.eventMask != reset.eventMask) {
			/* Having a non-zero eventMask without a callback function being
			 * defined should never occur but let's just make sure here that
			 * we keep things consistent.
			 */
			convity.eventMask = reset.eventMask;
		}

		if (rc == PMIC_SUCCESS) {
			/* Mark the connectivity device handle as being closed. */

			convity.handle = reset.handle;
			convity.handleState = reset.handleState;
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Change the current operating mode of the PMIC connectivity hardware.
 * The available connectivity operating modes is hardware dependent and
 * consists of one or more of the following: USB (including USB On-the-Go),
 * RS-232, and CEA-936. Requesting an operating mode that is not supported
 * by the PMIC hardware will return PMIC_NOT_SUPPORTED.
 *
 * @param       handle          device handle from open() call
 * @param       mode            desired operating mode
 *
 * @return      PMIC_SUCCESS    if the requested mode was successfully set
 */
PMIC_STATUS pmic_convity_set_mode(const PMIC_CONVITY_HANDLE handle,
				  const PMIC_CONVITY_MODE mode)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		rc = pmic_convity_set_mode_internal(mode);
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the current operating mode for the PMIC connectivity hardware.
 *
 * @param       handle          device handle from open() call
 * @param       mode            the current PMIC connectivity operating mode
 *
 * @return      PMIC_SUCCESS    if the requested mode was successfully set
 */
PMIC_STATUS pmic_convity_get_mode(const PMIC_CONVITY_HANDLE handle,
				  PMIC_CONVITY_MODE * const mode)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (mode != (PMIC_CONVITY_MODE *) NULL)) {

		*mode = convity.mode;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Restore all registers to the initial power-on/reset state.
 *
 * @param       handle          device handle from open() call
 *
 * @return      PMIC_SUCCESS    if the reset was successful
 */
PMIC_STATUS pmic_convity_reset(const PMIC_CONVITY_HANDLE handle)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		/* Reset the PMIC Connectivity register to it's power on state. */
		rc = pmic_write_reg(PRIO_CONN, REG_USB, RESET_USBCNTRL_REG_0,
				    REG_FULLMASK);

		rc = pmic_write_reg(PRIO_CONN, REG_CHARGE_USB_SPARE,
				    RESET_USBCNTRL_REG_1, REG_FULLMASK);

		if (rc == PMIC_SUCCESS) {
			/* Also reset the device driver state data structure. */
			convity = reset;
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Register a callback function that will be used to signal PMIC connectivity
 * events. For example, the USB subsystem should register a callback function
 * in order to be notified of device connect/disconnect events. Note, however,
 * that non-USB events may also be signalled depending upon the PMIC hardware
 * capabilities. Therefore, the callback function must be able to properly
 * handle all of the possible events if support for non-USB peripherals is
 * also to be included.
 *
 * @param       handle          device handle from open() call
 * @param       func            a pointer to the callback function
 * @param       eventMask       a mask selecting events to be notified
 *
 * @return      PMIC_SUCCESS    if the callback was successful registered
 */
PMIC_STATUS pmic_convity_set_callback(const PMIC_CONVITY_HANDLE handle,
				      const PMIC_CONVITY_CALLBACK func,
				      const PMIC_CONVITY_EVENTS eventMask)
{
	unsigned long flags;
	PMIC_STATUS rc = PMIC_ERROR;
	/*type_event_notification eventNotify; */

	/* We need to start a critical section here to ensure a consistent state
	 * in case simultaneous calls to pmic_convity_set_callback() are made. In
	 * that case, we must serialize the calls to ensure that the "callback"
	 * and "eventMask" state variables are always consistent.
	 *
	 * Note that we don't actually need to acquire the spinlock until later
	 * when we are finally ready to update the "callback" and "eventMask"
	 * state variables which are shared with the interrupt handler.
	 */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {

		/* Return an error if either the callback function or event mask
		 * is not properly defined.
		 *
		 * It is also considered an error if a callback function has already
		 * been defined. If you wish to register for a new set of events,
		 * then you must first call pmic_convity_clear_callback() to
		 * deregister the existing callback function and list of events
		 * before trying to register a new callback function.
		 */
		if ((func == NULL) || (eventMask == 0)
		    || (convity.callback != NULL)) {
			rc = PMIC_ERROR;

			/* Register for PMIC events from the core protocol driver. */
		} else {

			if ((eventMask & USB_DETECT_4V4_RISE) ||
			    (eventMask & USB_DETECT_4V4_FALL)) {
				/* We need to register for the 4.4V interrupt. */
				printk(KERN_EMERG "in callback");
				pmic_event_init(&eventNotify);
				eventNotify.event = EVENT_USBI;	//EVENT_USB_44VI;  //EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_4V4);
				rc = pmic_event_subscribe(&eventNotify);

				if (rc != PMIC_SUCCESS) {
					return rc;
				}
			}

			if ((eventMask & USB_DETECT_2V0_RISE) ||
			    (eventMask & USB_DETECT_2V0_FALL)) {
				/* We need to register for the 2.0V interrupt. */
				eventNotify.event = EVENT_USBI;	//EVENT_USB_20VI;  //EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_2V0);
				rc = pmic_event_subscribe(&eventNotify);

				if (rc != PMIC_SUCCESS) {
					goto Cleanup_4V4;
				}
			}

			if ((eventMask & USB_DETECT_0V8_RISE) ||
			    (eventMask & USB_DETECT_0V8_FALL)) {
				/* We need to register for the 0.8V interrupt. */
				eventNotify.event = EVENT_USBI;	//EVENT_USB_08VI;  //EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_0V8);
				rc = pmic_event_subscribe(&eventNotify);

				if (rc != PMIC_SUCCESS) {
					goto Cleanup_2V0;
				}
			}

			if ((eventMask & USB_DETECT_MINI_A) ||
			    (eventMask & USB_DETECT_MINI_B)
			    || (eventMask & USB_DETECT_NON_USB_ACCESSORY)
			    || (eventMask & USB_DETECT_FACTORY_MODE)) {
				/* We need to register for the AB_DET interrupt. */
				eventNotify.event = EVENT_IDI;	//EVENT_AB_DETI;//EVENT_IDI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_ABDET);
				rc = pmic_event_subscribe(&eventNotify);

				if (rc != PMIC_SUCCESS) {
					goto Cleanup_0V8;
				}
			}

			/* Use a critical section to maintain a consistent state. */
			spin_lock_irqsave(&lock, flags);

			/* Successfully registered for all events. */
			convity.callback = func;
			convity.eventMask = eventMask;
			spin_unlock_irqrestore(&lock, flags);

			goto End;

			/* This section unregisters any already registered events if we should
			 * encounter an error partway through the registration process. Note
			 * that we don't check the return status here since it is already set
			 * to PMIC_ERROR before we get here.
			 */
		      Cleanup_0V8:

			if ((eventMask & USB_DETECT_0V8_RISE) ||
			    (eventMask & USB_DETECT_0V8_FALL)) {
				eventNotify.event = EVENT_USBI;	//EVENT_USB_08VI;//EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_0V8);
				pmic_event_unsubscribe(&eventNotify);
				goto End;
			}

		      Cleanup_2V0:

			if ((eventMask & USB_DETECT_2V0_RISE) ||
			    (eventMask & USB_DETECT_2V0_FALL)) {
				eventNotify.event = EVENT_USBI;	//EVENT_USB_20VI;//EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_2V0);
				pmic_event_unsubscribe(&eventNotify);
				goto End;
			}

		      Cleanup_4V4:

			if ((eventMask & USB_DETECT_4V4_RISE) ||
			    (eventMask & USB_DETECT_4V4_FALL)) {
				eventNotify.event = EVENT_USBI;	//EVENT_USB_44VI;//EVENT_USBI
				eventNotify.callback =
				    pmic_convity_event_handler;
				eventNotify.param = (void *)(CORE_EVENT_4V4);
				pmic_event_unsubscribe(&eventNotify);

			}
		}
		/* Exit the critical section. */

	}
      End:up(&mutex);
	return rc;

}

/*!
 * Clears the current callback function. If this function returns successfully
 * then all future Connectivity events will only be handled by the default
 * handler within the Connectivity driver.
 *
 * @param       handle          device handle from open() call
 *
 * @return      PMIC_SUCCESS    if the callback was successful cleared
 */
PMIC_STATUS pmic_convity_clear_callback(const PMIC_CONVITY_HANDLE handle)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		rc = pmic_convity_deregister_all();
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the current callback function and event mask.
 *
 * @param       handle          device handle from open() call
 * @param       func            the current callback function
 * @param       eventMask       the current event selection mask
 *
 * @return      PMIC_SUCCESS    if the callback information was successful
 *                              retrieved
 */
PMIC_STATUS pmic_convity_get_callback(const PMIC_CONVITY_HANDLE handle,
				      PMIC_CONVITY_CALLBACK * const func,
				      PMIC_CONVITY_EVENTS * const eventMask)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (func != (PMIC_CONVITY_CALLBACK *) NULL) &&
	    (eventMask != (PMIC_CONVITY_EVENTS *) NULL)) {
		*func = convity.callback;
		*eventMask = convity.eventMask;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */

	up(&mutex);

	return rc;
}

/*@*/

/**************************************************************************
 * USB-specific configuration and setup functions.
 **************************************************************************
 */

/*!
 * @name USB and USB-OTG Connectivity APIs
 * Functions for controlling USB and USB-OTG connectivity.
 */
/*@{*/

/*!
 * Set the USB transceiver speed. Both mc13783 and sc55112 have support
 * for low speed (1.5 Mbps) and full speed (12 Mbps) modes. However, there
 * is currently no support for the high speed (480 Mbps) mode.
 *
 * @param       handle          device handle from open() call
 * @param       speed           the desired USB transceiver speed
 * @param       mode            the USB transceiver mode
 *
 * @return      PMIC_SUCCESS    if the transceiver speed was successfully set
 */
PMIC_STATUS pmic_convity_usb_set_speed(const PMIC_CONVITY_HANDLE handle,
				       const PMIC_CONVITY_USB_SPEED speed,
				       const PMIC_CONVITY_USB_MODE mode)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value = 0;
	unsigned int reg_mask = SET_BITS(regUSB0, FSENB, 1) |
	    SET_BITS(regUSB0, USB_PU, 1) |
	    SET_BITS(regUSB0, UDP_PD, 1) | SET_BITS(regUSB0, UDM_PD, 1);

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		/* Validate the function parameters and if they are valid, then
		 * configure the pull-up and pull-down resistors as required for
		 * the desired operating mode.
		 */
		if (((mode == USB_PERIPHERAL) && (speed == USB_LOW_SPEED)) ||
		    (speed == USB_HIGH_SPEED)) {
			/*  no support of pull-up resistor on D- so we
			 * cannot support operating as a USB low-speed peripheral. Note
			 * that supporting this mode is optional under the USB OTG
			 * specification.
			 *
			 * The USB transceiver also does not support the high speed mode
			 * (which is also optional under the USB OTG specification).
			 */
			rc = PMIC_NOT_SUPPORTED;
		} else if ((speed != USB_LOW_SPEED)
			   && (speed != USB_FULL_SPEED)) {
			/* Final validity check on the speed parameter. */
			rc = PMIC_ERROR;;
		} else {
			/* First configure the D+ and D- pull-up/pull-down resistors as
			 * per the USB OTG specification.
			 */
			if (mode == USB_HOST) {
				/* Activate pull-down resistors on both D+ and D-. */
				reg_value =
				    SET_BITS(regUSB0, UDP_PD,
					     1) | SET_BITS(regUSB0, UDM_PD, 1);
			} else if ((mode == USB_PERIPHERAL)
				   && (speed == USB_FULL_SPEED)) {
				/* Activate pull-up on D+ and pull-down on D-. */
				reg_value =
				    SET_BITS(regUSB0, USB_PU,
					     1) | SET_BITS(regUSB0, UDM_PD, 1);
			}

			/* Now set the desired USB transceiver speed. Note that
			 * USB_FULL_SPEED simply requires FSENB=0 (which it
			 * already is).
			 */
			if (speed == USB_LOW_SPEED) {
				reg_value |= SET_BITS(regUSB0, FSENB, 1);
			}

			rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value,
					    reg_mask);

			if (rc == PMIC_SUCCESS) {
				convity.usbSpeed = speed;
				convity.usbMode = mode;
			}
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the USB transceiver speed.
 *
 * @param       handle          device handle from open() call
 * @param       speed           the current USB transceiver speed
 * @param       mode            the current USB transceiver mode
 *
 * @return      PMIC_SUCCESS    if the transceiver speed was successfully
 *                              obtained
 */
PMIC_STATUS pmic_convity_usb_get_speed(const PMIC_CONVITY_HANDLE handle,
				       PMIC_CONVITY_USB_SPEED * const speed,
				       PMIC_CONVITY_USB_MODE * const mode)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (speed != (PMIC_CONVITY_USB_SPEED *) NULL) &&
	    (mode != (PMIC_CONVITY_USB_MODE *) NULL)) {
		*speed = convity.usbSpeed;
		*mode = convity.usbMode;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Set the USB transceiver's power supply configuration.
 *
 * @param       handle          device handle from open() call
 * @param       pwrin           USB transceiver regulator input power source
 * @param       pwrout          USB transceiver regulator output power level
 *
 * @return      PMIC_SUCCESS    if the USB transceiver's power supply
 *                              configuration was successfully set
 */
PMIC_STATUS pmic_convity_usb_set_power_source(const PMIC_CONVITY_HANDLE handle,
					      const PMIC_CONVITY_USB_POWER_IN
					      pwrin,
					      const PMIC_CONVITY_USB_POWER_OUT
					      pwrout)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value =
	    SET_BITS(regUSB1, VUSBEN, 1) | SET_BITS(regUSB1, VBUSEN,
						    1) | SET_BITS(regUSB1,
								  VUSBIN,
								  2) |
	    SET_BITS(regUSB1, VUSB, 1);
	unsigned int reg_mask =
	    SET_BITS(regUSB1, VUSBIN, 1) | SET_BITS(regUSB1, VUSBEN,
						    1) | SET_BITS(regUSB1,
								  VBUSEN, 1);

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {

		if (pwrin == USB_POWER_INTERNAL_BOOST) {
			reg_value |= SET_BITS(regUSB1, VUSBIN, 0);
		} else if (pwrin == USB_POWER_VBUS) {
			reg_value |= SET_BITS(regUSB1, VUSBIN, 1);
		}

		else if (pwrin == USB_POWER_INTERNAL) {
			reg_value |= SET_BITS(regUSB1, VUSBIN, 2);
		}

		if (pwrout == USB_POWER_3V3) {
			reg_value |= SET_BITS(regUSB1, VUSB, 1);
		}

		else if (pwrout == USB_POWER_2V775) {
			reg_value |= SET_BITS(regUSB1, VUSB, 0);
		}
		rc = pmic_write_reg(PRIO_CONN, REG_CHARGE_USB_SPARE, reg_value,
				    reg_mask);

		if (rc == PMIC_SUCCESS) {
			convity.usbPowerIn = pwrin;
			convity.usbPowerOut = pwrout;
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the USB transceiver's current power supply configuration.
 *
 * @param       handle          device handle from open() call
 * @param       pwrin           USB transceiver regulator input power source
 * @param       pwrout          USB transceiver regulator output power level
 *
 * @return      PMIC_SUCCESS    if the USB transceiver's power supply
 *                              configuration was successfully retrieved
 */
PMIC_STATUS pmic_convity_usb_get_power_source(const PMIC_CONVITY_HANDLE handle,
					      PMIC_CONVITY_USB_POWER_IN *
					      const pwrin,
					      PMIC_CONVITY_USB_POWER_OUT *
					      const pwrout)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (pwrin != (PMIC_CONVITY_USB_POWER_IN *) NULL) &&
	    (pwrout != (PMIC_CONVITY_USB_POWER_OUT *) NULL)) {
		*pwrin = convity.usbPowerIn;
		*pwrout = convity.usbPowerOut;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Set the USB transceiver's operating mode.
 *
 * @param       handle          device handle from open() call
 * @param       mode            desired operating mode
 *
 * @return      PMIC_SUCCESS    if the USB transceiver's operating mode
 *                              was successfully configured
 */
PMIC_STATUS pmic_convity_usb_set_xcvr(const PMIC_CONVITY_HANDLE handle,
				      const PMIC_CONVITY_USB_TRANSCEIVER_MODE
				      mode)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value = SET_BITS(regUSB0, USBXCVREN, 1);
	unsigned int reg_mask = 0;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {

		if (mode == USB_TRANSCEIVER_OFF) {
			reg_value = SET_BITS(regUSB0, USBXCVREN, 0);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);

			rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value,
					    reg_mask);

		}

		if (mode == USB_SINGLE_ENDED_UNIDIR) {
			reg_value |=
			    SET_BITS(regUSB0, DATSE0, 1) | SET_BITS(regUSB0,
								    BIDIR, 0);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		} else if (mode == USB_SINGLE_ENDED_BIDIR) {
			reg_value |=
			    SET_BITS(regUSB0, DATSE0, 1) | SET_BITS(regUSB0,
								    BIDIR, 1);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		} else if (mode == USB_DIFFERENTIAL_UNIDIR) {
			reg_value |=
			    SET_BITS(regUSB0, DATSE0, 0) | SET_BITS(regUSB0,
								    BIDIR, 0);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		} else if (mode == USB_DIFFERENTIAL_BIDIR) {
			reg_value |=
			    SET_BITS(regUSB0, DATSE0, 0) | SET_BITS(regUSB0,
								    BIDIR, 1);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		}

		if (mode == USB_SUSPEND_ON) {
			reg_value |= SET_BITS(regUSB0, USB_SUSPEND, 1);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		} else if (mode == USB_SUSPEND_OFF) {
			reg_value |= SET_BITS(regUSB0, USB_SUSPEND, 0);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		}

		if (mode == USB_OTG_SRP_DLP_START) {
			reg_value |= SET_BITS(regUSB0, USB_PU, 0);
			reg_mask |= SET_BITS(regUSB0, USB_SUSPEND, 1);
		} else if (mode == USB_OTG_SRP_DLP_STOP) {
			reg_value &= SET_BITS(regUSB0, USB_PU, 1);
			reg_mask |= SET_BITS(regUSB0, USB_PU, 1);
		}

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value, reg_mask);

		if (rc == PMIC_SUCCESS) {
			convity.usbXcvrMode = mode;
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the USB transceiver's current operating mode.
 *
 * @param       handle          device handle from open() call
 * @param       mode            current operating mode
 *
 * @return      PMIC_SUCCESS    if the USB transceiver's operating mode
 *                              was successfully retrieved
 */
PMIC_STATUS pmic_convity_usb_get_xcvr(const PMIC_CONVITY_HANDLE handle,
				      PMIC_CONVITY_USB_TRANSCEIVER_MODE *
				      const mode)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (mode != (PMIC_CONVITY_USB_TRANSCEIVER_MODE *) NULL)) {
		*mode = convity.usbXcvrMode;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Set the Data Line Pulse duration (in milliseconds) for the USB OTG
 * Session Request Protocol.
 *
 * For mc13783, this feature is not supported.So return PMIC_NOT_SUPPORTED
 *
 * @param       handle          device handle from open() call
 * @param       duration        the data line pulse duration (ms)
 *
 * @return      PMIC_SUCCESS    if the pulse duration was successfully set
 */
PMIC_STATUS pmic_convity_usb_otg_set_dlp_duration(const PMIC_CONVITY_HANDLE
						  handle,
						  const unsigned int duration)
{
	PMIC_STATUS rc = PMIC_NOT_SUPPORTED;

	/* The setting of the dlp duration is not supported by the mc13783 PMIC hardware. */

	/* No critical section is required. */

	if ((handle != convity.handle)
	    || (convity.handleState != HANDLE_IN_USE)) {
		/* Must return error indication for invalid handle parameter to be
		 * consistent with other APIs.
		 */
		rc = PMIC_ERROR;
	}

	return rc;
}

/*!
 * Get the current Data Line Pulse duration (in milliseconds) for the USB
 * OTG Session Request Protocol.
 *
 * @param       handle          device handle from open() call
 * @param       duration        the data line pulse duration (ms)
 *
 * @return      PMIC_SUCCESS    if the pulse duration was successfully obtained
 */
PMIC_STATUS pmic_convity_usb_otg_get_dlp_duration(const PMIC_CONVITY_HANDLE
						  handle,
						  unsigned int *const duration)
{
	PMIC_STATUS rc = PMIC_NOT_SUPPORTED;

	/* The setting of dlp duration is not supported by the mc13783 PMIC hardware. */

	/* No critical section is required. */

	if ((handle != convity.handle)
	    || (convity.handleState != HANDLE_IN_USE)) {
		/* Must return error indication for invalid handle parameter to be
		 * consistent with other APIs.
		 */
		rc = PMIC_ERROR;
	}

	return rc;
}

/*!
 * Set the USB On-The-Go (OTG) configuration.
 *
 * @param       handle          device handle from open() call
 * @param       cfg             desired USB OTG configuration
 *
 * @return      PMIC_SUCCESS    if the OTG configuration was successfully set
 */
PMIC_STATUS pmic_convity_usb_otg_set_config(const PMIC_CONVITY_HANDLE handle,
					    const PMIC_CONVITY_USB_OTG_CONFIG
					    cfg)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value = 0;
	unsigned int reg_mask = 0;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		if (cfg & USB_OTG_SE0CONN) {
			reg_value = SET_BITS(regUSB0, SE0_CONN, 1);
			reg_mask = SET_BITS(regUSB0, SE0_CONN, 1);
		}

		if (cfg & USB_OTG_DLP_SRP) {
			reg_value |= SET_BITS(regUSB0, DLP_SRP, 1);
			reg_mask |= SET_BITS(regUSB0, DLP_SRP, 1);
		}

		if (cfg & USB_PULL_OVERRIDE) {
			reg_value |= SET_BITS(regUSB0, PULLOVR, 1);
			reg_mask |= SET_BITS(regUSB0, PULLOVR, 1);
		}

		if (cfg & USB_VBUS_CURRENT_LIMIT_HIGH) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 0);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_10MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 1);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_20MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 2);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_30MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 3);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_40MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 4);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_50MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 5);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_60MS) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 6);
		}
		if (cfg & USB_VBUS_CURRENT_LIMIT_LOW) {
			reg_value |= SET_BITS(regUSB0, CURRENT_LIMIT, 7);
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 7);
		}

		if (cfg & USB_VBUS_PULLDOWN) {
			reg_mask |= SET_BITS(regUSB0, VBUSPDENB, 1);
		}

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value, reg_mask);

		if (rc == PMIC_SUCCESS) {
			if ((cfg & USB_VBUS_CURRENT_LIMIT_HIGH) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_10MS) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_20MS) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_30MS) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_40MS) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_50MS) ||
			    (cfg & USB_VBUS_CURRENT_LIMIT_LOW_60MS)) {
				/* Make sure that the VBUS current limit state is
				 * correctly set to either USB_VBUS_CURRENT_LIMIT_HIGH
				 * or USB_VBUS_CURRENT_LIMIT_LOW but never both at the
				 * same time.
				 *
				 * We guarantee this by first clearing both of the
				 * status bits and then resetting the correct one.
				 */
				convity.usbOtgCfg &=
				    ~(USB_VBUS_CURRENT_LIMIT_HIGH |
				      USB_VBUS_CURRENT_LIMIT_LOW |
				      USB_VBUS_CURRENT_LIMIT_LOW_10MS |
				      USB_VBUS_CURRENT_LIMIT_LOW_20MS |
				      USB_VBUS_CURRENT_LIMIT_LOW_30MS |
				      USB_VBUS_CURRENT_LIMIT_LOW_40MS |
				      USB_VBUS_CURRENT_LIMIT_LOW_50MS |
				      USB_VBUS_CURRENT_LIMIT_LOW_60MS);
			}

			convity.usbOtgCfg |= cfg;
		}
	}
	//}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Clears the USB On-The-Go (OTG) configuration. Multiple configuration settings
 * may be OR'd together in a single call. However, selecting conflicting
 * settings (e.g., multiple VBUS current limits) will result in undefined
 * behavior.
 *
 * @param[in]   handle          Device handle from open() call.
 * @param[in]   cfg             USB OTG configuration settings to be cleared.
 *
 * @retval      PMIC_SUCCESS         If the OTG configuration was successfully
 *                                   cleared.
 * @retval      PMIC_PARAMETER_ERROR If the handle is invalid.
 * @retval      PMIC_NOT_SUPPORTED   If the desired USB OTG configuration is
 *                                   not supported by the PMIC hardware.
 */
PMIC_STATUS pmic_convity_usb_otg_clear_config(const PMIC_CONVITY_HANDLE handle,
					      const PMIC_CONVITY_USB_OTG_CONFIG
					      cfg)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value = 0;
	unsigned int reg_mask = 0;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		/* if ((cfg & USB_VBUS_CURRENT_LIMIT_LOW_10MS) ||
		   (cfg & USB_VBUS_CURRENT_LIMIT_LOW_20MS) ||
		   (cfg & USB_VBUS_CURRENT_LIMIT_LOW_30MS) ||
		   (cfg & USB_VBUS_CURRENT_LIMIT_LOW_40MS) ||
		   (cfg & USB_VBUS_CURRENT_LIMIT_LOW_50MS) ||
		   (cfg & USB_VBUS_CURRENT_LIMIT_LOW_60MS))
		   {
		   rc = PMIC_NOT_SUPPORTED;
		   } */
		//else

		if (cfg & USB_OTG_SE0CONN) {
			reg_mask = SET_BITS(regUSB0, SE0_CONN, 1);
		}

		if (cfg & USB_OTG_DLP_SRP) {
			reg_mask |= SET_BITS(regUSB0, DLP_SRP, 1);
		}

		if (cfg & USB_PULL_OVERRIDE) {
			reg_mask |= SET_BITS(regUSB0, PULLOVR, 1);
		}

		if (cfg & USB_VBUS_CURRENT_LIMIT_HIGH) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 0);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_10MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 1);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_20MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 2);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_30MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 3);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_40MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 4);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_50MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 5);
		} else if (cfg & USB_VBUS_CURRENT_LIMIT_LOW_60MS) {
			reg_mask |= SET_BITS(regUSB0, CURRENT_LIMIT, 6);
		}

		if (cfg & USB_VBUS_PULLDOWN) {
			reg_value |= SET_BITS(regUSB0, VBUSPDENB, 1);
			reg_mask |= SET_BITS(regUSB0, VBUSPDENB, 1);
		}

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value, reg_mask);

		if (rc == PMIC_SUCCESS) {
			convity.usbOtgCfg &= ~cfg;
		}
	}
	//}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the current USB On-The-Go (OTG) configuration.
 *
 * @param       handle          device handle from open() call
 * @param       cfg             the current USB OTG configuration
 *
 * @return      PMIC_SUCCESS    if the OTG configuration was successfully
 *                              retrieved
 */
PMIC_STATUS pmic_convity_usb_otg_get_config(const PMIC_CONVITY_HANDLE handle,
					    PMIC_CONVITY_USB_OTG_CONFIG *
					    const cfg)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (cfg != (PMIC_CONVITY_USB_OTG_CONFIG *) NULL)) {
		*cfg = convity.usbOtgCfg;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*@}*/

/**************************************************************************
 * RS-232-specific configuration and setup functions.
 **************************************************************************
 */

/*!
 * @name RS-232 Serial Connectivity APIs
 * Functions for controlling RS-232 serial connectivity.
 */
/*@{*/

/*!
 * Set the connectivity interface to the selected RS-232 operating
 * configuration. Note that the RS-232 operating mode will be automatically
 * overridden if the USB_EN is asserted at any time (e.g., when a USB device
 * is attached). However, we will also automatically return to the RS-232
 * mode once the USB device is detached.
 *
 * @param       handle          device handle from open() call
 * @param       cfgInternal     RS-232 transceiver internal connections
 * @param       cfgExternal     RS-232 transceiver external connections
 *
 * @return      PMIC_SUCCESS    if the requested mode was set
 */
PMIC_STATUS pmic_convity_rs232_set_config(const PMIC_CONVITY_HANDLE handle,
					  const PMIC_CONVITY_RS232_INTERNAL
					  cfgInternal,
					  const PMIC_CONVITY_RS232_EXTERNAL
					  cfgExternal)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value0 = 0, reg_value1 = 0;
	unsigned int reg_mask = SET_BITS(regUSB1, RSPOL, 1);

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE)) {
		rc = PMIC_SUCCESS;

		/* Validate the calling parameters. */
		/*if ((cfgInternal !=  RS232_TX_USE0VM_RX_UDATVP) &&
		   (cfgInternal != RS232_TX_RX_INTERNAL_DEFAULT) && (cfgInternal !=  RS232_TX_UDATVP_RX_URXVM))
		   {

		   rc = PMIC_NOT_SUPPORTED;
		   } */
		if (cfgInternal == RS232_TX_USE0VM_RX_UDATVP) {

			reg_value0 = SET_BITS(regUSB0, INTERFACE_MODE, 1);

		} else if (cfgInternal == RS232_TX_RX_INTERNAL_DEFAULT) {

			reg_value0 = SET_BITS(regUSB0, INTERFACE_MODE, 1);
			reg_mask |= SET_BITS(regUSB1, RSPOL, 1);

		} else if (cfgInternal == RS232_TX_UDATVP_RX_URXVM) {

			reg_value0 = SET_BITS(regUSB0, INTERFACE_MODE, 2);
			reg_value1 |= SET_BITS(regUSB1, RSPOL, 1);

		} else if ((cfgExternal == RS232_TX_UDM_RX_UDP) ||
			   (cfgExternal == RS232_TX_RX_EXTERNAL_DEFAULT)) {
			/* Configure for TX on D+ and RX on D-. */
			reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 1);
			reg_value1 |= SET_BITS(regUSB1, RSPOL, 0);
		} else if (cfgExternal != RS232_TX_UDM_RX_UDP) {
			/* Any other RS-232 configuration is an error. */
			rc = PMIC_ERROR;
		}

		if (rc == PMIC_SUCCESS) {
			/* Configure for TX on D- and RX on D+. */
			rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0,
					    reg_mask);

			rc = pmic_write_reg(PRIO_CONN, REG_CHARGE_USB_SPARE,
					    reg_value1, reg_mask);

			if (rc == PMIC_SUCCESS) {
				convity.rs232CfgInternal = cfgInternal;
				convity.rs232CfgExternal = cfgExternal;
			}
		}
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*!
 * Get the connectivity interface's current RS-232 operating configuration.
 *
 * @param       handle          device handle from open() call
 * @param       cfgInternal     RS-232 transceiver internal connections
 * @param       cfgExternal     RS-232 transceiver external connections
 *
 * @return      PMIC_SUCCESS    if the requested mode was retrieved
 */
PMIC_STATUS pmic_convity_rs232_get_config(const PMIC_CONVITY_HANDLE handle,
					  PMIC_CONVITY_RS232_INTERNAL *
					  const cfgInternal,
					  PMIC_CONVITY_RS232_EXTERNAL *
					  const cfgExternal)
{
	PMIC_STATUS rc = PMIC_ERROR;

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle == convity.handle) &&
	    (convity.handleState == HANDLE_IN_USE) &&
	    (cfgInternal != (PMIC_CONVITY_RS232_INTERNAL *) NULL) &&
	    (cfgExternal != (PMIC_CONVITY_RS232_EXTERNAL *) NULL)) {
		*cfgInternal = convity.rs232CfgInternal;
		*cfgExternal = convity.rs232CfgExternal;

		rc = PMIC_SUCCESS;
	}

	/* Exit the critical section. */
	up(&mutex);

	return rc;
}

/*@}*/

/**************************************************************************
 * CEA-936-specific configuration and setup functions.
 **************************************************************************
 */

/*!
 * @name CEA-936 Connectivity APIs
 * Functions for controlling CEA-936 connectivity.
 */
/*@{*/

/*!
 * Signal the attached device to exit the current CEA-936 operating mode.
 * Returns an error if the current operating mode is not CEA-936.
 *
 * @param       handle          device handle from open() call
 * @param       signal          type of exit signal to be sent
 *
 * @return      PMIC_SUCCESS    if exit signal was sent
 */
PMIC_STATUS pmic_convity_cea936_exit_signal(const PMIC_CONVITY_HANDLE handle,
					    const
					    PMIC_CONVITY_CEA936_EXIT_SIGNAL
					    signal)
{
	PMIC_STATUS rc = PMIC_ERROR;
	unsigned int reg_value = 0;
	unsigned int reg_mask =
	    SET_BITS(regUSB0, IDPD, 1) | SET_BITS(regUSB0, IDPULSE, 1);

	/* Use a critical section to maintain a consistent state. */
	down_interruptible(&mutex);

	if ((handle != convity.handle)
	    || (convity.handleState != HANDLE_IN_USE)) {
		/* Must return error indication for invalid handle parameter to be
		 * consistent with other APIs.
		 */
		rc = PMIC_ERROR;
	} else if (signal == CEA936_UID_PULLDOWN_6MS) {
		reg_value =
		    SET_BITS(regUSB0, IDPULSE, 0) | SET_BITS(regUSB0, IDPD, 0);
	} else if (signal == CEA936_UID_PULLDOWN_6MS) {
		reg_value = SET_BITS(regUSB0, IDPULSE, 1);
	} else if (signal == CEA936_UID_PULLDOWN) {
		reg_value = SET_BITS(regUSB0, IDPD, 1);
	} else if (signal == CEA936_UDMPULSE) {
		reg_value = SET_BITS(regUSB0, DMPULSE, 1);
	}

	rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value, reg_mask);

	up(&mutex);

	return rc;
}

/*@}*/

/**************************************************************************
 * Static functions.
 **************************************************************************
 */

/*!
 * @name Connectivity Driver Internal Support Functions
 * These non-exported internal functions are used to support the functionality
 * of the exported connectivity APIs.
 */
/*@{*/

/*!
 * This internal helper function sets the desired operating mode (either USB
 * OTG or RS-232). It must be called with the mutex already acquired.
 *
 * @param       mode               the desired operating mode (USB or RS232)
 *
 * @return      PMIC_SUCCESS       if the desired operating mode was set
 * @return      PMIC_NOT_SUPPORTED if the desired operating mode is invalid
 */
static PMIC_STATUS pmic_convity_set_mode_internal(const PMIC_CONVITY_MODE mode)
{
	unsigned int reg_value0 = 0, reg_value1 = 0;
	unsigned int reg_mask0 = SET_BITS(regUSB0, UDP_PD, 1) |
	    SET_BITS(regUSB0, UDM_PD, 1) |
	    SET_BITS(regUSB0, INTERFACE_MODE, 1) |
	    SET_BITS(regUSB0, USB_PU, 1) | SET_BITS(regUSB0, VBUSPDENB, 1);

	unsigned int reg_mask1 = SET_BITS(regUSB1, VUSBEN, 1) |
	    SET_BITS(regUSB1, RSTRI, 1) |
	    SET_BITS(regUSB1, VBUSEN, 1) | SET_BITS(regUSB1, RSPOL, 1);

	PMIC_STATUS rc = PMIC_SUCCESS;

	switch (mode) {
	case USB:
		/* For the USB mode, we start by tri-stating the USB bus (by
		 * setting VBUSEN = 0) until a device is connected (i.e.,
		 * until we receive a 4.4V rising edge event). All pull-up
		 * and pull-down resistors are also disabled until a USB
		 * device is actually connected and we have determined which
		 * device is the host and the desired USB bus speed.
		 *
		 * Also tri-state the RS-232 buffers (by setting RSTRI = 1).
		 * This prevents the hardware from automatically returning to
		 * the RS-232 mode when the USB device is detached.
		 */

		reg_value0 = SET_BITS(regUSB0, INTERFACE_MODE, 0);

		reg_value1 = SET_BITS(regUSB1, RSTRI, 1);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);
		if (rc == PMIC_SUCCESS) {

			CHECK_ERROR(pmic_write_reg
				    (PRIO_CONN, REG_CHARGE_USB_SPARE,
				     reg_value1, reg_mask1));
		}
		break;

	case RS232_1:
		/* For the RS-232 mode, we tri-state the USB bus (by setting
		 * VBUSEN = 0) and enable the RS-232 transceiver (by setting
		 * RS232ENB = 0).
		 *
		 * Note that even in the RS-232 mode, if a USB device is
		 * plugged in, we will receive a 4.4V rising edge event which
		 * will automatically disable the RS-232 transceiver and
		 * tri-state the RS-232 buffers. This allows us to temporarily
		 * switch over to USB mode while the USB device is attached.
		 * The RS-232 transceiver and buffers will be automatically
		 * re-enabled when the USB device is detached.
		 */

		/* Explicitly disconnect all of the USB pull-down resistors
		 * and the VUSB power regulator here just to be safe.
		 *
		 * But we do connect the internal pull-up resistor on USB_D+
		 * to avoid having an extra load on the USB_D+ line when in
		 * RS-232 mode.
		 */

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 1) |
		    SET_BITS(regUSB0, VBUSPDENB, 1) |
		    SET_BITS(regUSB0, USB_PU, 1);

		reg_value1 = SET_BITS(regUSB1, RSPOL, 0);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);

		if (rc == PMIC_SUCCESS) {
			CHECK_ERROR(pmic_write_reg
				    (PRIO_CONN, REG_CHARGE_USB_SPARE,
				     reg_value1, reg_mask1));
		}
		break;

	case RS232_2:

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 2) |
		    SET_BITS(regUSB0, VBUSPDENB, 1) |
		    SET_BITS(regUSB0, USB_PU, 1);

		reg_value1 = SET_BITS(regUSB1, RSPOL, 1);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);

		if (rc == PMIC_SUCCESS) {
			CHECK_ERROR(pmic_write_reg
				    (PRIO_CONN, REG_CHARGE_USB_SPARE,
				     reg_value1, reg_mask1));
		}
		break;

	case CEA936_MONO:

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 4);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);
		break;

	case CEA936_STEREO:

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 5);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);
		break;
	case CEA936_TEST_RIGHT:

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 6);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);
		break;
	case CEA936_TEST_LEFT:

		reg_value0 |= SET_BITS(regUSB0, INTERFACE_MODE, 7);

		rc = pmic_write_reg(PRIO_CONN, REG_USB, reg_value0, reg_mask0);
		break;

	default:

		rc = PMIC_NOT_SUPPORTED;
	}

	if (rc == PMIC_SUCCESS) {
		convity.mode = mode;
	}

	return rc;
}

/*!
 * This internal helper function deregisters all of the currently registered
 * callback events. It must be called with the mutual exclusion spinlock
 * already acquired.
 *
 * We've defined the event and callback deregistration code here as a separate
 * function because it can be called by either the pmic_convity_close() or the
 * pmic_convity_clear_callback() APIs. We also wanted to avoid any possible
 * issues with having the same thread calling spin_lock_irq() twice.
 *
 * Note that the mutex must have already been acquired. We will also acquire
 * the spinlock here to avoid any possible race conditions with the interrupt
 * handler.
 *
 * @return      PMIC_SUCCESS    if all of the callback events were cleared
 */
static PMIC_STATUS pmic_convity_deregister_all(void)
{
	unsigned long flags;
	/*type_event_notification eventNotify; */
	PMIC_STATUS rc = PMIC_SUCCESS;

	/* Deregister each of the PMIC events that we had previously
	 * registered for by using pmic_event_subscribe().
	 */

	if ((convity.eventMask & USB_DETECT_MINI_A) ||
	    (convity.eventMask & USB_DETECT_MINI_B) ||
	    (convity.eventMask & USB_DETECT_NON_USB_ACCESSORY) ||
	    (convity.eventMask & USB_DETECT_FACTORY_MODE)) {
		eventNotify.event = EVENT_IDI;	//EVENT_AB_DETI;//EVENT_IDI
		eventNotify.callback = pmic_convity_event_handler;
		eventNotify.param = (void *)(CORE_EVENT_ABDET);

		if (pmic_event_unsubscribe(&eventNotify) == PMIC_SUCCESS) {
			/* Also acquire the spinlock here to avoid any possible race
			 * conditions with the interrupt handler.
			 */

			spin_lock_irqsave(&lock, flags);

			convity.eventMask &= ~(USB_DETECT_MINI_A |
					       USB_DETECT_MINI_B |
					       USB_DETECT_NON_USB_ACCESSORY |
					       USB_DETECT_FACTORY_MODE);

			spin_unlock_irqrestore(&lock, flags);
		} else {
			TRACEMSG
			    ("%s: pmic_event_unsubscribe() for EVENT_AB_DETI failed\n",
			     __FILE__);
			rc = PMIC_ERROR;
		}
	}

	else if ((convity.eventMask & USB_DETECT_0V8_RISE) ||
		 (convity.eventMask & USB_DETECT_0V8_FALL)) {
		eventNotify.event = EVENT_USBI;	//EVENT_USB_08VI;//EVENT_USBI
		eventNotify.callback = pmic_convity_event_handler;
		eventNotify.param = (void *)(CORE_EVENT_0V8);
		if (pmic_event_unsubscribe(&eventNotify) == PMIC_SUCCESS) {
			/* Also acquire the spinlock here to avoid any possible race
			 * conditions with the interrupt handler.
			 */
			spin_lock_irqsave(&lock, flags);

			convity.eventMask &= ~(USB_DETECT_0V8_RISE |
					       USB_DETECT_0V8_FALL);

			spin_unlock_irqrestore(&lock, flags);
		} else {
			TRACEMSG
			    ("%s: pmic_event_unsubscribe() for EVENT_USB_08VI failed\n",
			     __FILE__);
			rc = PMIC_ERROR;
		}

	}

	else if ((convity.eventMask & USB_DETECT_2V0_RISE) ||
		 (convity.eventMask & USB_DETECT_2V0_FALL)) {
		eventNotify.event = EVENT_USBI;	//EVENT_USB_20VI;//EVENT_USBI
		eventNotify.callback = pmic_convity_event_handler;
		eventNotify.param = (void *)(CORE_EVENT_2V0);
		if (pmic_event_unsubscribe(&eventNotify) == PMIC_SUCCESS) {
			/* Also acquire the spinlock here to avoid any possible race
			 * conditions with the interrupt handler.
			 */
			spin_lock_irqsave(&lock, flags);

			convity.eventMask &= ~(USB_DETECT_2V0_RISE |
					       USB_DETECT_2V0_FALL);

			spin_unlock_irqrestore(&lock, flags);
		} else {
			TRACEMSG
			    ("%s: pmic_event_unsubscribe() for EVENT_USB_20VI failed\n",
			     __FILE__);
			rc = PMIC_ERROR;
		}
	}

	else if ((convity.eventMask & USB_DETECT_4V4_RISE) ||
		 (convity.eventMask & USB_DETECT_4V4_FALL)) {

		eventNotify.event = EVENT_USBI;	//EVENT_USB_44VI;//EVENT_USBI
		eventNotify.callback = pmic_convity_event_handler;
		eventNotify.param = (void *)(CORE_EVENT_4V4);

		if (pmic_event_unsubscribe(&eventNotify) == PMIC_SUCCESS) {

			/* Also acquire the spinlock here to avoid any possible race
			 * conditions with the interrupt handler.
			 */
			spin_lock_irqsave(&lock, flags);

			convity.eventMask &= ~(USB_DETECT_4V4_RISE |
					       USB_DETECT_4V4_FALL);

			spin_unlock_irqrestore(&lock, flags);
		} else {
			TRACEMSG
			    ("%s: pmic_event_unsubscribe() for EVENT_USB_44VI failed\n",
			     __FILE__);
			rc = PMIC_ERROR;
		}
	}

	if (rc == PMIC_SUCCESS) {
		/* Also acquire the spinlock here to avoid any possible race
		 * conditions with the interrupt handler.
		 */
		spin_lock_irqsave(&lock, flags);

		/* Restore the initial reset values for the callback function
		 * and event mask parameters. This should be NULL and zero,
		 * respectively.
		 *
		 * Note that we wait until the end here to fully reset everything
		 * just in case some of the pmic_event_unsubscribe() calls above
		 * failed for some reason (which normally shouldn't happen).
		 */
		convity.callback = reset.callback;
		convity.eventMask = reset.eventMask;

		spin_unlock_irqrestore(&lock, flags);
	}
	return rc;
}

/*!
 * This is the default event handler for all connectivity-related events
 * and hardware interrupts.
 *
 * @param       param           event ID
 */
static void pmic_convity_event_handler(void *param)
{
	unsigned long flags;

	/* Update the global list of active interrupt events. */
	spin_lock_irqsave(&lock, flags);
	eventID |= (PMIC_CORE_EVENT) (param);
	spin_unlock_irqrestore(&lock, flags);

	/* Schedule the tasklet to be run as soon as it is convenient to do so. */
	tasklet_schedule(&convityTasklet);
}

/*!
 * @brief This is the connectivity driver tasklet that handles interrupt events.
 *
 * This function is scheduled by the connectivity driver interrupt handler
 * pmic_convity_event_handler() to complete the processing of all of the
 * connectivity-related interrupt events.
 *
 * Since this tasklet runs with interrupts enabled, we can safely call
 * the ADC driver, if necessary, to properly detect the type of USB connection
 * that is being made and to call any user-registered callback functions.
 *
 * @param[in]   arg                 The parameter that was provided above in
 *                                  the DECLARE_TASKLET() macro (unused).
 */
static void pmic_convity_tasklet(unsigned long arg)
{

	static const unsigned USB_ID_ADC_CHANNEL = 7;
	static const unsigned short ADC_FULLSCALE = 0x3ff;
	static const unsigned short ADC_FULLSCALE_VOLTAGE = 230;
	static const unsigned short ADC_SCALE_FACTOR = 100;
	/*static const unsigned short VMC13783                = 250; */
	static const unsigned USB_MINI_A_DETECT_THRESHOLD = 0.15 * 250;
	static const unsigned USB_NON_USB_DETECT_THRESHOLD = 0.85 * 250;
	unsigned short adcResult = 0;
	unsigned voltageUSB_IDScaled = 0;
	PMIC_CONVITY_EVENTS activeEvents = 0;
	unsigned long flags = 0;

	/* Check the interrupt sense bits to determine exactly what
	 * event just occurred.
	 */
	if (eventID & CORE_EVENT_4V4) {
		spin_lock_irqsave(&lock, flags);
		eventID &= ~CORE_EVENT_4V4;
		spin_unlock_irqrestore(&lock, flags);

		activeEvents = pmic_check_sensor(SENSE_USB4V4S) ?
		    USB_DETECT_4V4_RISE : USB_DETECT_4V4_FALL;

		if (activeEvents & ~convity.eventMask) {
			/* The default handler for 4.4 V rising/falling edge detection
			 * is to simply ignore the event.
			 */
			;
		}
	} else if (eventID & CORE_EVENT_2V0) {
		spin_lock_irqsave(&lock, flags);
		eventID &= ~CORE_EVENT_2V0;
		spin_unlock_irqrestore(&lock, flags);

		activeEvents = pmic_check_sensor(SENSE_USB2V0S) ?
		    USB_DETECT_2V0_RISE : USB_DETECT_2V0_FALL;

		if (activeEvents & ~convity.eventMask) {
			/* The default handler for 2.0 V rising/falling edge detection
			 * is to simply ignore the event.
			 */
			;
		}
	} else if (eventID & CORE_EVENT_0V8) {
		spin_lock_irqsave(&lock, flags);
		eventID &= ~CORE_EVENT_0V8;
		spin_unlock_irqrestore(&lock, flags);

		activeEvents = pmic_check_sensor(SENSE_USB0V8S) ?
		    USB_DETECT_0V8_RISE : USB_DETECT_0V8_FALL;

		if (activeEvents & ~convity.eventMask) {
			/* The default handler for 0.8 V rising/falling edge detection
			 * is to simply ignore the event.
			 */
			;
		}
	} else if (eventID & CORE_EVENT_ABDET) {
		spin_lock_irqsave(&lock, flags);
		eventID &= ~CORE_EVENT_ABDET;
		spin_unlock_irqrestore(&lock, flags);

		/* We need to read the ADC comparator voltage level to determine
		 * the type of USB connector. Note that channel 8 of the first
		 * group of ADC channels (AD_SEL = 0 and ADA[2:0] = 111) is reserved
		 * for reading the USB_ID input.
		 */
		if (pmic_adc_convert(USB_ID_ADC_CHANNEL, &adcResult) ==
		    PMIC_SUCCESS) {
			/* As stated in the mc13783 DTS document, the voltage scaling
			 * function is:
			 *
			 *      V_USB_ID_SENSE = V_USB_ID * 0.9 (+/-5%)
			 *
			 * But note that the ADC specifications also state that the full
			 * input voltage range is 0 V - 2.30 V with a 10-bit output.
			 * Therefore, we must first convert the ADC reading back into a
			 * voltage level before using the above equation to calculate the
			 * actual voltage on the USB ID line.
			 *
			 * Another important thing to note is that we must avoid using
			 * floating-point arithmetic here by employing an appropriate
			 * scaling factor of our own. The scaling factor must also provide
			 * at least one digit of precision in the final result.
			 *
			 * To make things clear, we'll calculate the USB ID voltage in two
			 * steps. The first step is to calculate the voltage level as
			 * measured by the ADC. Here we simply scale the ADC result by
			 * 100 to keep some precision in the division operation. Then we
			 * multiply by the ADC full scale voltage of 2.30 V (also scaled
			 * by 100 to avoid floating-point math). This gives us the ADC
			 * measured voltage scaled by 10000.
			 */
			voltageUSB_IDScaled =
			    ((unsigned)adcResult * ADC_SCALE_FACTOR) /
			    ADC_FULLSCALE * ADC_FULLSCALE_VOLTAGE;

			/* The second step now is to apply the scaling equation given
			 * above to calculate the actual USB ID voltage level. Here we
			 * can divide by 80 because the scaling factor from the previous
			 * calculation was 10000. Therefore, we can reduce the overall
			 * scaling factor to be 100 by simply multiplying the denominator
			 * by 100. This also helps us to avoid having to use floating-point
			 * math in the division operation.
			 *
			 * The resulting value is the actual USB ID voltage and is still
			 * scaled by 100 which gives us two decimal places of precision.
			 */
			voltageUSB_IDScaled = voltageUSB_IDScaled / 80;

			if (voltageUSB_IDScaled < USB_MINI_A_DETECT_THRESHOLD) {
				/* Consider voltage as ground (mini-A connected). */
				activeEvents = USB_DETECT_MINI_A;
			} else if (voltageUSB_IDScaled >
				   USB_MINI_A_DETECT_THRESHOLD
				   && voltageUSB_IDScaled <
				   USB_NON_USB_DETECT_THRESHOLD) {

				/*    Connection of non-usb accessory   */
				activeEvents = USB_DETECT_NON_USB_ACCESSORY;
			} else if (voltageUSB_IDScaled >
				   USB_NON_USB_DETECT_THRESHOLD
				   && voltageUSB_IDScaled < 300) {

				/* Consider voltage as floating/connected with high impedence
				 * (mini-B connected).
				 */
				activeEvents = USB_DETECT_MINI_B;
			}
		} else if (voltageUSB_IDScaled > 300) {
			/*             FACTORY MODE                   */

			activeEvents = USB_DETECT_FACTORY_MODE;
		}
		if (activeEvents & ~convity.eventMask) {
			/* The default handler for mini-A/mini-B connector detection
			 * is to simply ignore the event.
			 */
			;
		}
	}

	/* Begin a critical section here so that we don't register/deregister
	 * for events or open/close the connectivity driver while the existing
	 * event handler (if it is currently defined) is in the middle of handling
	 * the current event.
	 */
	spin_lock_irqsave(&lock, flags);

	/* Finally, call the user-defined callback function if required. */
	if ((convity.handleState == HANDLE_IN_USE) &&
	    (convity.callback != NULL) && (activeEvents & convity.eventMask)) {
		(*convity.callback) (activeEvents);
	}

	spin_unlock_irqrestore(&lock, flags);
}

/*@}*/

/**************************************************************************
 * Module initialization and termination functions.
 *
 * Note that if this code is compiled into the kernel, then the
 * module_init() function will be called within the device_initcall()
 * group.
 **************************************************************************
 */

/*!
 * @name Connectivity Driver Loading/Unloading Functions
 * These non-exported internal functions are used to support the connectivity
 * device driver initialization and de-initialization operations.
 */
/*@{*/

/*!
 * @brief This is the connectivity device driver initialization function.
 *
 * This function is called by the kernel when this device driver is first
 * loaded.
 */
static int __init mc13783_pmic_convity_init(void)
{
	/*TRACEMSG_CONVITY(_K_D( "mc13783 Connectivity loading\n")); */
	printk(KERN_INFO "PMIC CONNECTIVITY LOADING\n");

	return 0;
}

/*!
 * @brief This is the Connectivity device driver de-initialization function.
 *
 * This function is called by the kernel when this device driver is about
 * to be unloaded.
 */
static void __exit mc13783_pmic_convity_exit(void)
{
	TRACEMSG(KERN_INFO "mc13783 Connectivity unloaded\n");

	/* Close the device handle if it is still open. This will also
	 * deregister any callbacks that may still be active.
	 */
	if (convity.handleState == HANDLE_IN_USE) {
		pmic_convity_close(convity.handle);
	}

	/* Reset the PMIC Connectivity register to it's power on state.
	 * We should do this when unloading the module so that we don't
	 * leave the hardware in a state which could cause problems when
	 * no device driver is loaded.
	 */
	pmic_write_reg(PRIO_CONN, REG_USB, RESET_USBCNTRL_REG_0, REG_FULLMASK);
	pmic_write_reg(PRIO_CONN, REG_CHARGE_USB_SPARE, RESET_USBCNTRL_REG_1,
		       REG_FULLMASK);
	/* Note that there is no need to reset the "convity" device driver
	 * state structure to the reset state since we are in the final
	 * stage of unloading the device driver. The device driver state
	 * structure will be automatically and properly reinitialized if
	 * this device driver is reloaded.
	 */
}

/*@}*/

/*
 * Module entry points and description information.
 */

module_init(mc13783_pmic_convity_init);
module_exit(mc13783_pmic_convity_exit);

MODULE_DESCRIPTION("mc13783 Connectivity device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
