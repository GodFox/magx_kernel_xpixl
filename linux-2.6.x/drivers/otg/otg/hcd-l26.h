/*
 * otg/hcd/hc_xfer.h - Generic transfer level USBOTG aware Host Controller Driver (HCD)
 * @(#) balden@seth2.belcarratech.com|otg/otg/hcd-l26.h|20051116205001|12978
 *
 *      Copyright (c) 2004-2005 Belcarra Technologies
 *
 * By:
 *      Stuart Lynne <sl@belcara.com>,
 *      Bruce Balden <balden@belcara.com>
 *
 * Copyright 2005-2006 Motorola, Inc.
 *
 * Changelog:
 * Date               Author           Comment
 * -----------------------------------------------------------------------------
 * 12/12/2005         Motorola         Initial distribution
 * 10/18/2006         Motorola         Add Open Src Software language
 * 12/12/2006         Motorola         Changes for Open Src compliance.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
/*!
 * @file otg/otg/hcd-l26.h
 * @brief Implements Linux version of Generic Host Controller driver
 *
 * @ingroup OTGHCD
 */
#ifndef HC_XFER_H
#define HC_XFER_H 1

/*===========================================================================*
 * Data structures and functions provided by the generic transfer aware
 * host controller framework.
 *
 * The generic framework must be linked with a hardware specific component,
 * whose functions are specified in hc_xfer_hw.h, and a generic root hub
 * component, whose functions are specified in hc_xfer_rh.h.
 *
 * The resulting module provides a table of operations to the usb core layer,
 * passed to the core layer by a call to usb_alloc_bus().
 *===========================================================================*/

/*! @name Descriptor sizes per descriptor type
 */
 /*! @{ */

// XXX this should be fixed....

#define USB_DT_DEVICE_SIZE              18
#define USB_DT_CONFIG_SIZE              9
#define USB_DT_INTERFACE_SIZE           9
#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_ENDPOINT_AUDIO_SIZE      9               /* Audio extension */
#define USB_DT_HUB_NONVAR_SIZE          7
#define USB_DT_HID_SIZE                 9
/*! @} */


#define XHC_RH_DESCRIPTORS_SIZE (USB_DT_DEVICE_SIZE+USB_DT_CONFIG_SIZE+USB_DT_INTERFACE_SIZE+USB_DT_ENDPOINT_SIZE+USB_DT_HUB_NONVAR_SIZE+2)

typedef struct rh_hcpriv {
        struct urb *int_urb;
        struct usbd_device_descriptor *dd;
        struct usbd_configuration_descriptor *cd;
        struct usbd_interface_descriptor *id;
        struct usbd_endpoint_descriptor *ed;
        struct hub_descriptor *hd;
        struct timer_list poll_timer;
        //int poll_jiffies;
        struct usb_device *dev;
        int dev_registered;
        u32 hub_change_status;   // For shadowing the HW
        u32 *port_change_status; // An array of [num_ports] values for shadowing the HW
        u32 hub_port_change_status;
        struct WORK_STRUCT psc_bh;
        u8 curr_cfg;
        u8 curr_itf;
        //u8 num_ports;
        u8 otg_device_mask;   // (1 << port_num) bit set iff port_num is acting as a device, not host, and so not usable
        u8 descriptors[XHC_RH_DESCRIPTORS_SIZE];
        int suspended;
} rh_hcpriv_t;

typedef struct bus_hcpriv {          // XFER level Host Controller Info
        struct usb_bus *usb_bus;
#if 1	//SHP
	struct usb_hcd	hcd;
#endif	
	void *hw_hci;              // HW specific information for this HC
        int root_hub_addr;         // Initially 0, set by the virtual root hub when addressed.
        struct rh_hcpriv *rh_hcpriv;          // Generic root hub info (see hc_xfer_rh.[hc])
        int terminating;           // Boolean, TRUE if shutting down.
        //struct hcd_instance *hcd;  // OTG Controller Info
        int max_active_urbs;
        struct urb **active_urbs;
        int device_registered;     // bus_device status
        struct device bus_device;      // Linux Driver Model info.
        //struct hcd_ops otg_ops;    // operations for OTG component.
        struct usb_driver *usb_driver;

        u8 num_ports;
        u8 otg_port;
        u8 otg_capable_mask;
        u16 rh_vendorid;
        u16 rh_productid;
        u16 rh_bcddevice;
        char *rh_serial;
        char *rh_product;
        char *rh_manufacturer;
        u8 rh_bmAttributes;
        u8 rh_bMaxPower;

        struct usb_device *roothub_dev;
        struct usb_device *first_dev;

        int max_active_transfers;

} bus_hcpriv_t;

typedef struct dev_hcpriv {      // XFER level HCI per attached device info
        struct usb_device *dev;
        struct list_head queued_urbs_both[2][16];
        u32 num_urbs_both[2][16];
        u8 epq_state_both[2][16];
} dev_hcpriv_t;

#define EPQ_EMPTY         0
#define EPQ_RUNNING       1
#define EPQ_WAITING       2

/*===========================================================================*
 * For the hardware specific component.
 *===========================================================================*/

extern void hcd_transfer_complete(struct bus_hcpriv *bus_hcpriv, int transfer_id, int format,
                                  int cc, u32 remaining, int next_toggle);

/*===========================================================================*
 * For the virtual root hub.
 *===========================================================================*/

extern void hcd_rh_urb_complete(struct bus_hcpriv *bus_hcpriv, struct urb *urb);

#include "../otg/otg-trace.h"
extern otg_tag_t xfer_hci_trace_tag;


#endif
