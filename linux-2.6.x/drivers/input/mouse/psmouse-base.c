/*
 * PS/2 mouse driver
 *
 * Copyright (c) 1999-2002 Vojtech Pavlik
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/init.h>
#include "psmouse.h"
#include "synaptics.h"
#include "logips2pp.h"

#define DRIVER_DESC	"PS/2 mouse driver"

MODULE_AUTHOR("Vojtech Pavlik <vojtech@suse.cz>");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

static char *psmouse_proto;
static unsigned int psmouse_max_proto = -1U;
module_param_named(proto, psmouse_proto, charp, 0);
MODULE_PARM_DESC(proto, "Highest protocol extension to probe (bare, imps, exps). Useful for KVM switches.");

int psmouse_resolution = 200;
module_param_named(resolution, psmouse_resolution, uint, 0);
MODULE_PARM_DESC(resolution, "Resolution, in dpi.");

unsigned int psmouse_rate = 100;
module_param_named(rate, psmouse_rate, uint, 0);
MODULE_PARM_DESC(rate, "Report rate, in reports per second.");

int psmouse_smartscroll = 1;
module_param_named(smartscroll, psmouse_smartscroll, bool, 0);
MODULE_PARM_DESC(smartscroll, "Logitech Smartscroll autorepeat, 1 = enabled (default), 0 = disabled.");

static unsigned int psmouse_resetafter;
module_param_named(resetafter, psmouse_resetafter, uint, 0);
MODULE_PARM_DESC(resetafter, "Reset device after so many bad packets (0 = never).");

__obsolete_setup("psmouse_noext");
__obsolete_setup("psmouse_resolution=");
__obsolete_setup("psmouse_smartscroll=");
__obsolete_setup("psmouse_resetafter=");
__obsolete_setup("psmouse_rate=");

static char *psmouse_protocols[] = { "None", "PS/2", "PS2++", "PS2T++", "GenPS/2", "ImPS/2", "ImExPS/2", "SynPS/2"};

/*
 * psmouse_process_byte() analyzes the PS/2 data stream and reports
 * relevant events to the input module once full packet has arrived.
 */

static psmouse_ret_t psmouse_process_byte(struct psmouse *psmouse, struct pt_regs *regs)
{
	struct input_dev *dev = &psmouse->dev;
	unsigned char *packet = psmouse->packet;

	if (psmouse->pktcnt < 3 + (psmouse->type >= PSMOUSE_GENPS))
		return PSMOUSE_GOOD_DATA;

/*
 * Full packet accumulated, process it
 */

	input_regs(dev, regs);

/*
 * The PS2++ protocol is a little bit complex
 */

	if (psmouse->type == PSMOUSE_PS2PP || psmouse->type == PSMOUSE_PS2TPP)
		ps2pp_process_packet(psmouse);

/*
 * Scroll wheel on IntelliMice, scroll buttons on NetMice
 */

	if (psmouse->type == PSMOUSE_IMPS || psmouse->type == PSMOUSE_GENPS)
		input_report_rel(dev, REL_WHEEL, -(signed char) packet[3]);

/*
 * Scroll wheel and buttons on IntelliMouse Explorer
 */

	if (psmouse->type == PSMOUSE_IMEX) {
		input_report_rel(dev, REL_WHEEL, (int) (packet[3] & 8) - (int) (packet[3] & 7));
		input_report_key(dev, BTN_SIDE, (packet[3] >> 4) & 1);
		input_report_key(dev, BTN_EXTRA, (packet[3] >> 5) & 1);
	}

/*
 * Extra buttons on Genius NewNet 3D
 */

	if (psmouse->type == PSMOUSE_GENPS) {
		input_report_key(dev, BTN_SIDE, (packet[0] >> 6) & 1);
		input_report_key(dev, BTN_EXTRA, (packet[0] >> 7) & 1);
	}

/*
 * Generic PS/2 Mouse
 */

	input_report_key(dev, BTN_LEFT,    packet[0]       & 1);
	input_report_key(dev, BTN_MIDDLE, (packet[0] >> 2) & 1);
	input_report_key(dev, BTN_RIGHT,  (packet[0] >> 1) & 1);

	input_report_rel(dev, REL_X, packet[1] ? (int) packet[1] - (int) ((packet[0] << 4) & 0x100) : 0);
	input_report_rel(dev, REL_Y, packet[2] ? (int) ((packet[0] << 3) & 0x100) - (int) packet[2] : 0);

	input_sync(dev);

	return PSMOUSE_FULL_PACKET;
}

/*
 * psmouse_interrupt() handles incoming characters, either gathering them into
 * packets or passing them to the command routine as command output.
 */

static irqreturn_t psmouse_interrupt(struct serio *serio,
		unsigned char data, unsigned int flags, struct pt_regs *regs)
{
	struct psmouse *psmouse = serio->private;
	psmouse_ret_t rc;

	if (psmouse->state == PSMOUSE_IGNORE)
		goto out;

	if (flags & (SERIO_PARITY|SERIO_TIMEOUT)) {
		if (psmouse->state == PSMOUSE_ACTIVATED)
			printk(KERN_WARNING "psmouse.c: bad data from KBC -%s%s\n",
				flags & SERIO_TIMEOUT ? " timeout" : "",
				flags & SERIO_PARITY ? " bad parity" : "");
		psmouse->nak = 1;
		clear_bit(PSMOUSE_FLAG_ACK, &psmouse->flags);
		clear_bit(PSMOUSE_FLAG_CMD,  &psmouse->flags);
		wake_up_interruptible(&psmouse->wait);
		goto out;
	}

	if (test_bit(PSMOUSE_FLAG_ACK, &psmouse->flags)) {
		switch (data) {
			case PSMOUSE_RET_ACK:
				psmouse->nak = 0;
				break;

			case PSMOUSE_RET_NAK:
				psmouse->nak = 1;
				break;

			/*
			 * Workaround for mice which don't ACK the Get ID command.
			 * These are valid mouse IDs that we recognize.
			 */
			case 0x00:
			case 0x03:
			case 0x04:
				if (test_bit(PSMOUSE_FLAG_WAITID, &psmouse->flags)) {
					psmouse->nak = 0;
					break;
				}
				/* Fall through */
			default:
				goto out;
		}

		if (!psmouse->nak && psmouse->cmdcnt) {
			set_bit(PSMOUSE_FLAG_CMD, &psmouse->flags);
			set_bit(PSMOUSE_FLAG_CMD1, &psmouse->flags);
		}
		clear_bit(PSMOUSE_FLAG_ACK, &psmouse->flags);
		wake_up_interruptible(&psmouse->wait);

		if (data == PSMOUSE_RET_ACK || data == PSMOUSE_RET_NAK)
			goto out;
	}

	if (test_bit(PSMOUSE_FLAG_CMD, &psmouse->flags)) {
		if (psmouse->cmdcnt)
			psmouse->cmdbuf[--psmouse->cmdcnt] = data;

		if (test_and_clear_bit(PSMOUSE_FLAG_CMD1, &psmouse->flags) && psmouse->cmdcnt)
			wake_up_interruptible(&psmouse->wait);

		if (!psmouse->cmdcnt) {
			clear_bit(PSMOUSE_FLAG_CMD, &psmouse->flags);
			wake_up_interruptible(&psmouse->wait);
		}
		goto out;
	}

	if (psmouse->state == PSMOUSE_INITIALIZING)
		goto out;

	if (psmouse->state == PSMOUSE_ACTIVATED &&
	    psmouse->pktcnt && time_after(jiffies, psmouse->last + HZ/2)) {
		printk(KERN_WARNING "psmouse.c: %s at %s lost synchronization, throwing %d bytes away.\n",
		       psmouse->name, psmouse->phys, psmouse->pktcnt);
		psmouse->pktcnt = 0;
	}

	psmouse->last = jiffies;
	psmouse->packet[psmouse->pktcnt++] = data;

	if (psmouse->packet[0] == PSMOUSE_RET_BAT) {
		if (psmouse->pktcnt == 1)
			goto out;

		if (psmouse->pktcnt == 2) {
			if (psmouse->packet[1] == PSMOUSE_RET_ID) {
				psmouse->state = PSMOUSE_IGNORE;
				serio_reconnect(serio);
				goto out;
			}
			if (psmouse->type == PSMOUSE_SYNAPTICS) {
				/* neither 0xAA nor 0x00 are valid first bytes
				 * for a packet in absolute mode
				 */
				psmouse->pktcnt = 0;
				goto out;
			}
		}
	}

	rc = psmouse->protocol_handler(psmouse, regs);

	switch (rc) {
		case PSMOUSE_BAD_DATA:
			printk(KERN_WARNING "psmouse.c: %s at %s lost sync at byte %d\n",
				psmouse->name, psmouse->phys, psmouse->pktcnt);
			psmouse->pktcnt = 0;

			if (++psmouse->out_of_sync == psmouse_resetafter) {
				psmouse->state = PSMOUSE_IGNORE;
				printk(KERN_NOTICE "psmouse.c: issuing reconnect request\n");
				serio_reconnect(psmouse->serio);
			}
			break;

		case PSMOUSE_FULL_PACKET:
			psmouse->pktcnt = 0;
			if (psmouse->out_of_sync) {
				psmouse->out_of_sync = 0;
				printk(KERN_NOTICE "psmouse.c: %s at %s - driver resynched.\n",
					psmouse->name, psmouse->phys);
			}
			break;

		case PSMOUSE_GOOD_DATA:
			break;
	}
out:
	return IRQ_HANDLED;
}

/*
 * psmouse_sendbyte() sends a byte to the mouse, and waits for acknowledge.
 * It doesn't handle retransmission, though it could - because when there would
 * be need for retransmissions, the mouse has to be replaced anyway.
 *
 * psmouse_sendbyte() can only be called from a process context
 */

static int psmouse_sendbyte(struct psmouse *psmouse, unsigned char byte)
{
	psmouse->nak = 1;
	set_bit(PSMOUSE_FLAG_ACK, &psmouse->flags);

	if (serio_write(psmouse->serio, byte) == 0)
		wait_event_interruptible_timeout(psmouse->wait,
				!test_bit(PSMOUSE_FLAG_ACK, &psmouse->flags),
				msecs_to_jiffies(200));

	clear_bit(PSMOUSE_FLAG_ACK, &psmouse->flags);
	return -psmouse->nak;
}

/*
 * psmouse_command() sends a command and its parameters to the mouse,
 * then waits for the response and puts it in the param array.
 *
 * psmouse_command() can only be called from a process context
 */

int psmouse_command(struct psmouse *psmouse, unsigned char *param, int command)
{
	int timeout;
	int send = (command >> 12) & 0xf;
	int receive = (command >> 8) & 0xf;
	int rc = -1;
	int i;

	timeout = msecs_to_jiffies(command == PSMOUSE_CMD_RESET_BAT ? 4000 : 500);

	clear_bit(PSMOUSE_FLAG_CMD, &psmouse->flags);
	if (command == PSMOUSE_CMD_GETID)
		set_bit(PSMOUSE_FLAG_WAITID, &psmouse->flags);

	if (receive && param)
		for (i = 0; i < receive; i++)
			psmouse->cmdbuf[(receive - 1) - i] = param[i];

	psmouse->cmdcnt = receive;

	if (command & 0xff)
		if (psmouse_sendbyte(psmouse, command & 0xff))
			goto out;

	for (i = 0; i < send; i++)
		if (psmouse_sendbyte(psmouse, param[i]))
			goto out;

	timeout = wait_event_interruptible_timeout(psmouse->wait,
				!test_bit(PSMOUSE_FLAG_CMD1, &psmouse->flags), timeout);

	if (psmouse->cmdcnt && timeout > 0) {
		if (command == PSMOUSE_CMD_RESET_BAT && jiffies_to_msecs(timeout) > 100)
			timeout = msecs_to_jiffies(100);

		if (command == PSMOUSE_CMD_GETID &&
		    psmouse->cmdbuf[receive - 1] != 0xab && psmouse->cmdbuf[receive - 1] != 0xac) {
			/*
			 * Device behind the port is not a keyboard
			 * so we don't need to wait for the 2nd byte
			 * of ID response.
			 */
			clear_bit(PSMOUSE_FLAG_CMD, &psmouse->flags);
			psmouse->cmdcnt = 0;
		}

		wait_event_interruptible_timeout(psmouse->wait,
				!test_bit(PSMOUSE_FLAG_CMD, &psmouse->flags), timeout);
	}

	if (param)
		for (i = 0; i < receive; i++)
			param[i] = psmouse->cmdbuf[(receive - 1) - i];

	if (psmouse->cmdcnt && (command != PSMOUSE_CMD_RESET_BAT || psmouse->cmdcnt != 1))
		goto out;

	rc = 0;

out:
	clear_bit(PSMOUSE_FLAG_CMD, &psmouse->flags);
	clear_bit(PSMOUSE_FLAG_CMD1, &psmouse->flags);
	clear_bit(PSMOUSE_FLAG_WAITID, &psmouse->flags);
	return rc;
}

/*
 * psmouse_sliced_command() sends an extended PS/2 command to the mouse
 * using sliced syntax, understood by advanced devices, such as Logitech
 * or Synaptics touchpads. The command is encoded as:
 * 0xE6 0xE8 rr 0xE8 ss 0xE8 tt 0xE8 uu where (rr*64)+(ss*16)+(tt*4)+uu
 * is the command.
 */
int psmouse_sliced_command(struct psmouse *psmouse, unsigned char command)
{
	int i;

	if (psmouse_command(psmouse, NULL, PSMOUSE_CMD_SETSCALE11))
		return -1;

	for (i = 6; i >= 0; i -= 2) {
		unsigned char d = (command >> i) & 3;
		if (psmouse_command(psmouse, &d, PSMOUSE_CMD_SETRES))
			return -1;
	}

	return 0;
}


/*
 * psmouse_reset() resets the mouse into power-on state.
 */
int psmouse_reset(struct psmouse *psmouse)
{
	unsigned char param[2];

	if (psmouse_command(psmouse, param, PSMOUSE_CMD_RESET_BAT))
		return -1;

	if (param[0] != PSMOUSE_RET_BAT && param[1] != PSMOUSE_RET_ID)
		return -1;

	return 0;
}


/*
 * Genius NetMouse magic init.
 */
static int genius_detect(struct psmouse *psmouse)
{
	unsigned char param[4];

	param[0] = 3;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRES);
	psmouse_command(psmouse,  NULL, PSMOUSE_CMD_SETSCALE11);
	psmouse_command(psmouse,  NULL, PSMOUSE_CMD_SETSCALE11);
	psmouse_command(psmouse,  NULL, PSMOUSE_CMD_SETSCALE11);
	psmouse_command(psmouse, param, PSMOUSE_CMD_GETINFO);

	return param[0] == 0x00 && param[1] == 0x33 && param[2] == 0x55;
}

/*
 * IntelliMouse magic init.
 */
static int intellimouse_detect(struct psmouse *psmouse)
{
	unsigned char param[2];

	param[0] = 200;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	param[0] = 100;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	param[0] =  80;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	psmouse_command(psmouse, param, PSMOUSE_CMD_GETID);

	return param[0] == 3;
}

/*
 * Try IntelliMouse/Explorer magic init.
 */
static int im_explorer_detect(struct psmouse *psmouse)
{
	unsigned char param[2];

	intellimouse_detect(psmouse);

	param[0] = 200;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	param[0] = 200;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	param[0] =  80;
	psmouse_command(psmouse, param, PSMOUSE_CMD_SETRATE);
	psmouse_command(psmouse, param, PSMOUSE_CMD_GETID);

	return param[0] == 4;
}

/*
 * psmouse_extensions() probes for any extensions to the basic PS/2 protocol
 * the mouse may have.
 */

static int psmouse_extensions(struct psmouse *psmouse,
			      unsigned int max_proto, int set_properties)
{
	int synaptics_hardware = 0;

/*
 * Try Synaptics TouchPad
 */
	if (max_proto > PSMOUSE_PS2 && synaptics_detect(psmouse)) {
		synaptics_hardware = 1;

		if (set_properties) {
			psmouse->vendor = "Synaptics";
			psmouse->name = "TouchPad";
		}

		if (max_proto > PSMOUSE_IMEX) {
			if (!set_properties || synaptics_init(psmouse) == 0)
				return PSMOUSE_SYNAPTICS;
/*
 * Some Synaptics touchpads can emulate extended protocols (like IMPS/2).
 * Unfortunately Logitech/Genius probes confuse some firmware versions so
 * we'll have to skip them.
 */
			max_proto = PSMOUSE_IMEX;
		}
/*
 * Make sure that touchpad is in relative mode, gestures (taps) are enabled
 */
		synaptics_reset(psmouse);
	}

	if (max_proto > PSMOUSE_IMEX && genius_detect(psmouse)) {

		if (set_properties) {
			set_bit(BTN_EXTRA, psmouse->dev.keybit);
			set_bit(BTN_SIDE, psmouse->dev.keybit);
			set_bit(REL_WHEEL, psmouse->dev.relbit);
			psmouse->vendor = "Genius";
			psmouse->name = "Wheel Mouse";
		}

		return PSMOUSE_GENPS;
	}

	if (max_proto > PSMOUSE_IMEX) {
		int type = ps2pp_init(psmouse, set_properties);
		if (type > PSMOUSE_PS2)
			return type;
	}

	if (max_proto >= PSMOUSE_IMEX && im_explorer_detect(psmouse)) {

		if (set_properties) {
			set_bit(REL_WHEEL, psmouse->dev.relbit);
			set_bit(BTN_SIDE, psmouse->dev.keybit);
			set_bit(BTN_EXTRA, psmouse->dev.keybit);
			if (!psmouse->name)
				psmouse->name = "Explorer Mouse";
		}

		return PSMOUSE_IMEX;
	}

	if (max_proto >= PSMOUSE_IMPS && intellimouse_detect(psmouse)) {

		if (set_properties) {
			set_bit(REL_WHEEL, psmouse->dev.relbit);
			if (!psmouse->name)
				psmouse->name = "Wheel Mouse";
		}

		return PSMOUSE_IMPS;
	}

/*
 * Okay, all failed, we have a standard mouse here. The number of the buttons
 * is still a question, though. We assume 3.
 */
	if (synaptics_hardware) {
/*
 * We detected Synaptics hardware but it did not respond to IMPS/2 probes.
 * We need to reset the touchpad because if there is a track point on the
 * pass through port it could get disabled while probing for protocol
 * extensions.
 */
		psmouse_reset(psmouse);
		psmouse_command(psmouse, NULL, PSMOUSE_CMD_RESET_DIS);
	}

	return PSMOUSE_PS2;
}

/*
 * psmouse_probe() probes for a PS/2 mouse.
 */

static int psmouse_probe(struct psmouse *psmouse)
{
	unsigned char param[2];

/*
 * First, we check if it's a mouse. It should send 0x00 or 0x03
 * in case of an IntelliMouse in 4-byte mode or 0x04 for IM Explorer.
 */

	param[0] = 0xa5;

	if (psmouse_command(psmouse, param, PSMOUSE_CMD_GETID))
		return -1;

	if (param[0] != 0x00 && param[0] != 0x03 && param[0] != 0x04)
		return -1;

/*
 * Then we reset and disable the mouse so that it doesn't generate events.
 */

	if (psmouse_command(psmouse, NULL, PSMOUSE_CMD_RESET_DIS))
		printk(KERN_WARNING "psmouse.c: Failed to reset mouse on %s\n", psmouse->serio->phys);

	return 0;
}

/*
 * Here we set the mouse resolution.
 */

static void psmouse_set_resolution(struct psmouse *psmouse)
{
	unsigned char param[1];

	if (psmouse->type == PSMOUSE_PS2PP && psmouse_resolution > 400) {
		ps2pp_set_800dpi(psmouse);
		return;
	}

	if (!psmouse_resolution || psmouse_resolution >= 200)
		param[0] = 3;
	else if (psmouse_resolution >= 100)
		param[0] = 2;
	else if (psmouse_resolution >= 50)
		param[0] = 1;
	else if (psmouse_resolution)
		param[0] = 0;

        psmouse_command(psmouse, param, PSMOUSE_CMD_SETRES);
}

/*
 * Here we set the mouse report rate.
 */

static void psmouse_set_rate(struct psmouse *psmouse)
{
	unsigned char rates[] = { 200, 100, 80, 60, 40, 20, 10, 0 };
	int i = 0;

	while (rates[i] > psmouse_rate) i++;
	psmouse_command(psmouse, rates + i, PSMOUSE_CMD_SETRATE);
}

/*
 * psmouse_initialize() initializes the mouse to a sane state.
 */

static void psmouse_initialize(struct psmouse *psmouse)
{
	unsigned char param[2];

/*
 * We set the mouse report rate, resolution and scaling.
 */

	if (psmouse_max_proto != PSMOUSE_PS2) {
		psmouse_set_rate(psmouse);
		psmouse_set_resolution(psmouse);
		psmouse_command(psmouse,  NULL, PSMOUSE_CMD_SETSCALE11);
	}

/*
 * We set the mouse into streaming mode.
 */

	psmouse_command(psmouse, param, PSMOUSE_CMD_SETSTREAM);
}

/*
 * psmouse_set_state() sets new psmouse state and resets all flags and
 * counters while holding serio lock so fighting with interrupt handler
 * is not a concern.
 */

static void psmouse_set_state(struct psmouse *psmouse, enum psmouse_state new_state)
{
	serio_pause_rx(psmouse->serio);
	psmouse->state = new_state;
	psmouse->pktcnt = psmouse->cmdcnt = psmouse->out_of_sync = 0;
	psmouse->flags = 0;
	serio_continue_rx(psmouse->serio);
}

/*
 * psmouse_activate() enables the mouse so that we get motion reports from it.
 */

static void psmouse_activate(struct psmouse *psmouse)
{
	if (psmouse_command(psmouse, NULL, PSMOUSE_CMD_ENABLE))
		printk(KERN_WARNING "psmouse.c: Failed to enable mouse on %s\n", psmouse->serio->phys);

	psmouse_set_state(psmouse, PSMOUSE_ACTIVATED);
}


/*
 * psmouse_deactivate() puts the mouse into poll mode so that we don't get motion
 * reports from it unless we explicitely request it.
 */

static void psmouse_deactivate(struct psmouse *psmouse)
{
	if (psmouse_command(psmouse, NULL, PSMOUSE_CMD_DISABLE))
		printk(KERN_WARNING "psmouse.c: Failed to deactivate mouse on %s\n", psmouse->serio->phys);

	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);
}


/*
 * psmouse_cleanup() resets the mouse into power-on state.
 */

static void psmouse_cleanup(struct serio *serio)
{
	struct psmouse *psmouse = serio->private;

	psmouse_reset(psmouse);
}

/*
 * psmouse_disconnect() closes and frees.
 */

static void psmouse_disconnect(struct serio *serio)
{
	struct psmouse *psmouse, *parent;

	psmouse = serio->private;
	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);

	if (serio->parent && (serio->type & SERIO_TYPE) == SERIO_PS_PSTHRU) {
		parent = serio->parent->private;
		if (parent->pt_deactivate)
			parent->pt_deactivate(parent);
	}

	if (psmouse->disconnect)
		psmouse->disconnect(psmouse);

	psmouse_set_state(psmouse, PSMOUSE_IGNORE);

	input_unregister_device(&psmouse->dev);
	serio_close(serio);
	kfree(psmouse);
}

/*
 * psmouse_connect() is a callback from the serio module when
 * an unhandled serio port is found.
 */
static void psmouse_connect(struct serio *serio, struct serio_driver *drv)
{
	struct psmouse *psmouse, *parent = NULL;

	if ((serio->type & SERIO_TYPE) != SERIO_8042 &&
	    (serio->type & SERIO_TYPE) != SERIO_PS_PSTHRU)
		return;

	/*
	 * If this is a pass-through port deactivate parent so the device
	 * connected to this port can be successfully identified
	 */
	if (serio->parent && (serio->type & SERIO_TYPE) == SERIO_PS_PSTHRU) {
		parent = serio->parent->private;
		psmouse_deactivate(parent);
	}

	if (!(psmouse = kmalloc(sizeof(struct psmouse), GFP_KERNEL)))
		goto out;

	memset(psmouse, 0, sizeof(struct psmouse));

	init_waitqueue_head(&psmouse->wait);
	init_input_dev(&psmouse->dev);
	psmouse->dev.evbit[0] = BIT(EV_KEY) | BIT(EV_REL);
	psmouse->dev.keybit[LONG(BTN_MOUSE)] = BIT(BTN_LEFT) | BIT(BTN_MIDDLE) | BIT(BTN_RIGHT);
	psmouse->dev.relbit[0] = BIT(REL_X) | BIT(REL_Y);
	psmouse->serio = serio;
	psmouse->dev.private = psmouse;
	psmouse_set_state(psmouse, PSMOUSE_INITIALIZING);

	serio->private = psmouse;
	if (serio_open(serio, drv)) {
		kfree(psmouse);
		serio->private = NULL;
		goto out;
	}

	if (psmouse_probe(psmouse) < 0) {
		serio_close(serio);
		kfree(psmouse);
		serio->private = NULL;
		goto out;
	}

	psmouse->type = psmouse_extensions(psmouse, psmouse_max_proto, 1);
	if (!psmouse->vendor)
		psmouse->vendor = "Generic";
	if (!psmouse->name)
		psmouse->name = "Mouse";
	if (!psmouse->protocol_handler)
		psmouse->protocol_handler = psmouse_process_byte;

	sprintf(psmouse->devname, "%s %s %s",
		psmouse_protocols[psmouse->type], psmouse->vendor, psmouse->name);
	sprintf(psmouse->phys, "%s/input0",
		serio->phys);

	psmouse->dev.name = psmouse->devname;
	psmouse->dev.phys = psmouse->phys;
	psmouse->dev.id.bustype = BUS_I8042;
	psmouse->dev.id.vendor = 0x0002;
	psmouse->dev.id.product = psmouse->type;
	psmouse->dev.id.version = psmouse->model;

	input_register_device(&psmouse->dev);

	printk(KERN_INFO "input: %s on %s\n", psmouse->devname, serio->phys);

	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);

	psmouse_initialize(psmouse);

	if (parent && parent->pt_activate)
		parent->pt_activate(parent);

	if (serio->child) {
		/*
		 * Nothing to be done here, serio core will detect that
		 * the driver set serio->child and will register it for us.
		 */
		printk(KERN_INFO "serio: %s port at %s\n", serio->child->name, psmouse->phys);
	}

	psmouse_activate(psmouse);

out:
	/* If this is a pass-through port the parent awaits to be activated */
	if (parent)
		psmouse_activate(parent);
}


static int psmouse_reconnect(struct serio *serio)
{
	struct psmouse *psmouse = serio->private;
	struct psmouse *parent = NULL;
	struct serio_driver *drv = serio->drv;
	int rc = -1;

	if (!drv || !psmouse) {
		printk(KERN_DEBUG "psmouse: reconnect request, but serio is disconnected, ignoring...\n");
		return -1;
	}

	if (serio->parent && (serio->type & SERIO_TYPE) == SERIO_PS_PSTHRU) {
		parent = serio->parent->private;
		psmouse_deactivate(parent);
	}

	psmouse_set_state(psmouse, PSMOUSE_INITIALIZING);

	if (psmouse->reconnect) {
	       if (psmouse->reconnect(psmouse))
			goto out;
	} else if (psmouse_probe(psmouse) < 0 ||
		   psmouse->type != psmouse_extensions(psmouse, psmouse_max_proto, 0))
		goto out;

	/* ok, the device type (and capabilities) match the old one,
	 * we can continue using it, complete intialization
	 */
	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);

	psmouse_initialize(psmouse);

	if (parent && parent->pt_activate)
		parent->pt_activate(parent);

	psmouse_activate(psmouse);
	rc = 0;

out:
	/* If this is a pass-through port the parent waits to be activated */
	if (parent)
		psmouse_activate(parent);

	return rc;
}


static struct serio_driver psmouse_drv = {
	.driver		= {
		.name	= "psmouse",
	},
	.description	= DRIVER_DESC,
	.interrupt	= psmouse_interrupt,
	.connect	= psmouse_connect,
	.reconnect	= psmouse_reconnect,
	.disconnect	= psmouse_disconnect,
	.cleanup	= psmouse_cleanup,
};

static inline void psmouse_parse_proto(void)
{
	if (psmouse_proto) {
		if (!strcmp(psmouse_proto, "bare"))
			psmouse_max_proto = PSMOUSE_PS2;
		else if (!strcmp(psmouse_proto, "imps"))
			psmouse_max_proto = PSMOUSE_IMPS;
		else if (!strcmp(psmouse_proto, "exps"))
			psmouse_max_proto = PSMOUSE_IMEX;
		else
			printk(KERN_ERR "psmouse: unknown protocol type '%s'\n", psmouse_proto);
	}
}

int __init psmouse_init(void)
{
	psmouse_parse_proto();
	serio_register_driver(&psmouse_drv);
	return 0;
}

void __exit psmouse_exit(void)
{
	serio_unregister_driver(&psmouse_drv);
}

module_init(psmouse_init);
module_exit(psmouse_exit);
