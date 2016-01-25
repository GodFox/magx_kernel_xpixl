
/* Generated by otg-fw-c.awk
 *
 * Do not Edit, see otg-state.awk
 */
/*
 * Copyright 2005-2006 Motorola, Inc.
 *
 * Changelog:
 * Date               Author           Comment
 * -----------------------------------------------------------------------------
 * 12/12/2005         Motorola         Initial distribution
 * 10/18/2006         Motorola         Add Open Src Software language
 * 12/12/2006         Motorola         Changes for Open Src compliance.
 *
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
* @file otg/otgcore/otg-fw.c
* @brief OTG Firmware - Input, Output and Timeout definitions
*
* This file defines the OTG State Machine input, output and timeout constants.
*
*
* @ingroup OTGFW
*/

/*!
* @page OTGFW
* @section OTGFW - otg-fw.c
* This contains the input, output and timout definitions for the OTG state machine firmware
*/


#ifdef OTG_APPLICATION
#include "otg-fw.h"
#else	/* OTG_APPLICATION/ */
#include <otg/otg-compat.h>
#include <otg/usbp-chap9.h>
#include <otg/usbp-func.h>
#include <otg/usbp-bus.h>
#include <otg/otg-trace.h>
#include <otg/otg-api.h>
#endif	/* OTG_APPLICATION */

#define OTG_VERSION_FWC 200508171524L


#if OTG_VERSION_FWC != OTG_VERSION_FW
#error OTG_VERSION_FWC != OTG_VERSION_FW
#endif	/* OTG_VERSION_FWC != OTG_VERSION_FW */

/* Generated by otg-input-name-c.awk
 *
 * Do not Edit, see otg-state.awk
 */


/* #include "otg-fw.h" */

#define OTG_VERSION_INPUT_TABLE 200508171524L


#if OTG_VERSION_INPUT_TABLE != OTG_VERSION_FW
#error OTG_VERSION_INPUT_TABLE != OTG_VERSION_FW
#endif	/* OTG_VERSION_INPUT_TABLE != OTG_VERSION_FW */


struct otg_input_name otg_input_names[] = {
	{ID_GND, "ID_GND",},
	{ID_FLOAT, "ID_FLOAT",},
	{DP_HIGH, "DP_HIGH",},
	{DM_HIGH, "DM_HIGH",},
	{B_SESS_END, "B_SESS_END",},
	{A_SESS_VLD, "A_SESS_VLD",},
	{B_SESS_VLD, "B_SESS_VLD",},
	{VBUS_VLD, "VBUS_VLD",},
	{SRP_DET, "SRP_DET",},
	{SE0_DET, "SE0_DET",},
	{SE1_DET, "SE1_DET",},
	{BDIS_ACON, "BDIS_ACON",},
	{CR_INT_DET, "CR_INT_DET",},
	{HUB_PORT_CONNECT, "HUB_PORT_CONNECT",},
	{BUS_RESET, "BUS_RESET",},
	{ADDRESSED, "ADDRESSED",},
	{DEVICE_REQUEST, "DEVICE_REQUEST",},
	{CONFIGURED, "CONFIGURED",},
	{NOT_SUPPORTED, "NOT_SUPPORTED",},
	{BUS_SUSPENDED, "BUS_SUSPENDED",},
	{a_bcon_no_tmout_req, "a_bcon_no_tmout_req",},
	{a_hpwr_req, "a_hpwr_req",},
	{bus_drop, "bus_drop",},
	{a_bus_drop, "a_bus_drop",},
	{b_bus_drop, "b_bus_drop",},
	{bus_req, "bus_req",},
	{a_bus_req, "a_bus_req",},
	{b_bus_req, "b_bus_req",},
	{b_sess_req, "b_sess_req",},
	{suspend_req, "suspend_req",},
	{a_suspend_req, "a_suspend_req",},
	{b_suspend_req, "b_suspend_req",},
	{set_remote_wakeup_cmd, "set_remote_wakeup_cmd",},
	{remote_wakeup_cmd, "remote_wakeup_cmd",},
	{reset_remote_wakeup_cmd, "reset_remote_wakeup_cmd",},
	{clr_err_cmd, "clr_err_cmd",},
	{b_hnp_cmd, "b_hnp_cmd",},
	{ph_int_cmd, "ph_int_cmd",},
	{ph_audio_cmd, "ph_audio_cmd",},
	{cr_int_cmd, "cr_int_cmd",},
	{led_on_cmd, "led_on_cmd",},
	{led_off_cmd, "led_off_cmd",},
	{HNP_ENABLED, "HNP_ENABLED",},
	{HNP_CAPABLE, "HNP_CAPABLE",},
	{HNP_SUPPORTED, "HNP_SUPPORTED",},
	{REMOTE_WAKEUP_ENABLED, "REMOTE_WAKEUP_ENABLED",},
	{REMOTE_CAPABLE, "REMOTE_CAPABLE",},
	{PCD_OK, "PCD_OK",},
	{TCD_OK, "TCD_OK",},
	{HCD_OK, "HCD_OK",},
	{OCD_OK, "OCD_OK",},
	{TMOUT, "TMOUT",},
	{enable_otg, "enable_otg",},
	{AUTO, "AUTO",},
	{0, "0",},

};

 /* eof */

/* Generated by otg-ioctls-names-c.awk
 *
 * Do not Edit, see otg-state.awk
 */
#if defined(OTG_LINUX) || defined(OTG_WINCE)


/* #include "otg-fw.h" */

#define OTG_VERSION_IOCTL_TABLE 200508171524L


#if OTG_VERSION_IOCTL_TABLE != OTG_VERSION_FW
#error OTG_VERSION_IOCTL_TABLE != OTG_VERSION_FW
#endif	/* OTG_VERSION_IOCTL_TABLE != OTG_VERSION_FW */

struct otg_ioctl_name otg_ioctl_names[] = {
	{OTGADMIN_A_BCON_NO_TMOUT_REQ, a_bcon_no_tmout_req, "OTGADMIN_A_BCON_NO_TMOUT_REQ",},
	{OTGADMIN_A_HPWR_REQ, a_hpwr_req, "OTGADMIN_A_HPWR_REQ",},
	{OTGADMIN_BUS_DROP, bus_drop, "OTGADMIN_BUS_DROP",},
	{OTGADMIN_A_BUS_DROP, a_bus_drop, "OTGADMIN_A_BUS_DROP",},
	{OTGADMIN_B_BUS_DROP, b_bus_drop, "OTGADMIN_B_BUS_DROP",},
	{OTGADMIN_BUS_REQ, bus_req, "OTGADMIN_BUS_REQ",},
	{OTGADMIN_A_BUS_REQ, a_bus_req, "OTGADMIN_A_BUS_REQ",},
	{OTGADMIN_B_BUS_REQ, b_bus_req, "OTGADMIN_B_BUS_REQ",},
	{OTGADMIN_B_SESS_REQ, b_sess_req, "OTGADMIN_B_SESS_REQ",},
	{OTGADMIN_SUSPEND_REQ, suspend_req, "OTGADMIN_SUSPEND_REQ",},
	{OTGADMIN_A_SUSPEND_REQ, a_suspend_req, "OTGADMIN_A_SUSPEND_REQ",},
	{OTGADMIN_B_SUSPEND_REQ, b_suspend_req, "OTGADMIN_B_SUSPEND_REQ",},
	{OTGADMIN_SET_REMOTE_WAKEUP_CMD, set_remote_wakeup_cmd, "OTGADMIN_SET_REMOTE_WAKEUP_CMD",},
	{OTGADMIN_REMOTE_WAKEUP_CMD, remote_wakeup_cmd, "OTGADMIN_REMOTE_WAKEUP_CMD",},
	{OTGADMIN_RESET_REMOTE_WAKEUP_CMD, reset_remote_wakeup_cmd, "OTGADMIN_RESET_REMOTE_WAKEUP_CMD",},
	{OTGADMIN_CLR_ERR_CMD, clr_err_cmd, "OTGADMIN_CLR_ERR_CMD",},
	{OTGADMIN_B_HNP_CMD, b_hnp_cmd, "OTGADMIN_B_HNP_CMD",},
	{OTGADMIN_PH_INT_CMD, ph_int_cmd, "OTGADMIN_PH_INT_CMD",},
	{OTGADMIN_PH_AUDIO_CMD, ph_audio_cmd, "OTGADMIN_PH_AUDIO_CMD",},
	{OTGADMIN_CR_INT_CMD, cr_int_cmd, "OTGADMIN_CR_INT_CMD",},
	{OTGADMIN_LED_ON_CMD, led_on_cmd, "OTGADMIN_LED_ON_CMD",},
	{OTGADMIN_LED_OFF_CMD, led_off_cmd, "OTGADMIN_LED_OFF_CMD",},
	{OTGADMIN_ENABLE_OTG, enable_otg, "OTGADMIN_ENABLE_OTG",},
	{0, 0, "",},

};

 /* eof */
#endif				/* defined(OTG_LINUX) || defined(OTG_WINCE) */

/* Generated by otg-output-names-c.awk
 *
 * Do not Edit, see otg-state.awk
 */

char           *otg_output_names[] = {
	"tcd_init_out",				 /* 0   Initiate Transceiver Controller Driver Initialization (or De-initialization.) */
	"pcd_init_out",				 /* 1   Initiate Peripheral Controller Driver Initialization (or De-initialization.) */
	"hcd_init_out",				 /* 2   Initiate Host Controller Driver Initialization (or De-initialization). */
	"ocd_init_out",				 /* 3   Initiate OTG Controller Driver Initialization (or De-initialization). */
	"tcd_en_out",				 /* 4   Enable Transceiver Controller Driver */
	"pcd_en_out",				 /* 5   Enable Peripheral Controller Driver */
	"hcd_en_out",				 /* 6   Enable Host Controller Driver */
	"drv_vbus_out",				 /* 7   A-Device will Drive Vbus to 5V through charge pump. */
	"chrg_vbus_out",			 /* 8   B-Device will charge Vbus to 3.3V through resistor (SRP.) */
	"dischrg_vbus_out",			 /* 9   B-Device will discharge Vbus (enable dischage resistor.) */
	"dm_pullup_out",			 /* 10   DM pullup control - aka loc_carkit */
	"dm_pulldown_out",			 /* 11   DM pulldown control */
	"dp_pullup_out",			 /* 12   DP pullup control - aka loc_conn */
	"dp_pulldown_out",			 /* 13   DP pulldown control */
	"clr_overcurrent_out",			 /* 14   Clear overcurrent indication */
	"dm_det_out",				 /* 15   Enable B-Device D- High detect */
	"dp_det_out",				 /* 16   Enable B-Device D+ High detect */
	"cr_det_out",				 /* 17   Enable D+ CR detect */
	"charge_pump_out",			 /* 18   Enable external charge pump. */
	"bdis_acon_out",			 /* 19   Enable auto A-connect after B-disconnect. */
	"id_pulldown_out",			 /* 20   Enable the ID to ground pulldown ( (CEA-936 - 5 wire carkit.) */
	"uart_out",				 /* 21   Enable Transparent UART mode (CEA-936.) */
	"audio_out",				 /* 22   Enable Audio mode (CEA-936 CarKit interrupt detector.) */
	"mono_out",				 /* 23   Enable Mono-Audio mode (CEA-936.) */
	"remote_wakeup_out",			 /* 24   Peripheral will perform remote wakeup. */
	"hcd_rh_out",				 /* 25   Host will enable root hub */
	"loc_sof_out",				 /* 26   Host will enable packet traffic. */
	"loc_suspend_out",			 /* 27   Host will suspend bus. */
	"remote_wakeup_en_out",			 /* 28   Host will send remote wakeup enable or disable request. */
	"hnp_en_out",				 /* 29   Host will send HNP enable request. */
	"hpwr_out",				 /* 30   Host will enable high power (external charge pump.) */
	NULL,

};

 /* eof */

/* Generated by otg-meta-names-c.awk
 *
 * Do not Edit, see otg-state.awk
 */



char           *otg_meta_names[] = {
	"a_idle",				 /*  0 -  */
	"a_wait_vrise",				 /*  1 -  */
	"a_wait_bcon",				 /*  2 -  */
	"a_host",				 /*  3 -  */
	"a_suspend",				 /*  4 -  */
	"a_peripheral",				 /*  5 -  */
	"a_wait_vfall",				 /*  6 -  */
	"a_vbus_err",				 /*  7 -  */
	"b_idle",				 /*  8 -  */
	"b_srp_init",				 /*  9 -  */
	"b_peripheral",				 /* 10 -  */
	"b_suspend",				 /* 11 -  */
	"b_wait_acon",				 /* 12 -  */
	"b_host",				 /* 13 -  */
	"b_suspended",				 /* 14 -  */
	"ph_disc",				 /* 15 -  Equivalent to b_peripheral */
	"ph_init",				 /* 16 -  */
	"ph_uart",				 /* 17 -  */
	"ph_aud",				 /* 18 -  */
	"ph_wait",				 /* 19 -  */
	"ph_exit",				 /* 20 -  */
	"cr_init",				 /* 21 -  */
	"cr_uart",				 /* 22 -  */
	"cr_aud",				 /* 23 -  */
	"cr_ack",				 /* 24 -  */
	"cr_wait",				 /* 25 -  */
	"cr_disc",				 /* 26 -  */
	"otg_init",				 /* 27 -  */
	"usb_accessory",			 /* 28 -  */
	"usb_factory",				 /* 29 -  */
	"unknown",				 /* 30 -  */

	"",
};

/* eof */
