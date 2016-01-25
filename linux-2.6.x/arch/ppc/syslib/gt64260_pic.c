/*
 * arch/ppc/syslib/gt64260_pic.c
 *
 * Interrupt controller support for Galileo's GT64260.
 *
 * Author: Chris Zankel <source@mvista.com>
 * Modified by: Mark A. Greer <mgreer@mvista.com>
 *
 * Based on sources from Rabeeh Khoury / Galileo Technology
 *
 * 2001 (c) MontaVista, Software, Inc.  This file is licensed under
 * the terms of the GNU General Public License version 2.  This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

/*
 * This file contains the specific functions to support the GT64260
 * interrupt controller.
 *
 * The GT64260 has two main interrupt registers (high and low) that
 * summarizes the interrupts generated by the units of the GT64260.
 * Each bit is assigned to an interrupt number, where the low register
 * are assigned from IRQ0 to IRQ31 and the high cause register
 * from IRQ32 to IRQ63
 * The GPP (General Purpose Port) interrupts are assigned from IRQ64 (GPP0)
 * to IRQ95 (GPP31).
 * get_irq() returns the lowest interrupt number that is currently asserted.
 *
 * Note:
 *  - This driver does not initialize the GPP when used as an interrupt
 *    input.
 */

#include <linux/stddef.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/stddef.h>
#include <linux/delay.h>
#include <linux/irq.h>

#include <asm/io.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/mv64x60.h>

#define CPU_INTR_STR	"gt64260 cpu interface error"
#define PCI0_INTR_STR	"gt64260 pci 0 error"
#define PCI1_INTR_STR	"gt64260 pci 1 error"

/* ========================== forward declaration ========================== */

static void gt64260_unmask_irq(unsigned int);
static void gt64260_mask_irq(unsigned int);

/* ========================== local declarations =========================== */

struct hw_interrupt_type gt64260_pic = {
	.typename = " gt64260_pic ",
	.enable   = gt64260_unmask_irq,
	.disable  = gt64260_mask_irq,
	.ack      = gt64260_mask_irq,
	.end      = gt64260_unmask_irq,
};

u32 gt64260_irq_base = 0;	/* GT64260 handles the next 96 IRQs from here */

static struct mv64x60_handle bh;

/* gt64260_init_irq()
 *
 *  This function initializes the interrupt controller. It assigns
 *  all interrupts from IRQ0 to IRQ95 to the gt64260 interrupt controller.
 *
 * Note:
 *  We register all GPP inputs as interrupt source, but disable them.
 */
void __init
gt64260_init_irq(void)
{
	int i;

	if (ppc_md.progress)
		ppc_md.progress("gt64260_init_irq: enter", 0x0);

	bh.v_base = mv64x60_get_bridge_vbase();

	ppc_cached_irq_mask[0] = 0;
	ppc_cached_irq_mask[1] = 0x0f000000;	/* Enable GPP intrs */
	ppc_cached_irq_mask[2] = 0;

	/* disable all interrupts and clear current interrupts */
	mv64x60_write(&bh, MV64x60_GPP_INTR_MASK, ppc_cached_irq_mask[2]);
	mv64x60_write(&bh, MV64x60_GPP_INTR_CAUSE, 0);
	mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_LO, ppc_cached_irq_mask[0]);
	mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_HI, ppc_cached_irq_mask[1]);

	/* use the gt64260 for all (possible) interrupt sources */
	for (i = gt64260_irq_base; i < (gt64260_irq_base + 96); i++)
		irq_desc[i].handler = &gt64260_pic;

	if (ppc_md.progress)
		ppc_md.progress("gt64260_init_irq: exit", 0x0);
}

/*
 * gt64260_get_irq()
 *
 *  This function returns the lowest interrupt number of all interrupts that
 *  are currently asserted.
 *
 * Input Variable(s):
 *  struct pt_regs*	not used
 *
 * Output Variable(s):
 *  None.
 *
 * Returns:
 *  int	<interrupt number> or -2 (bogus interrupt)
 */
int
gt64260_get_irq(struct pt_regs *regs)
{
	int irq;
	int irq_gpp;

	irq = mv64x60_read(&bh, GT64260_IC_MAIN_CAUSE_LO);
	irq = __ilog2((irq & 0x3dfffffe) & ppc_cached_irq_mask[0]);

	if (irq == -1) {
		irq = mv64x60_read(&bh, GT64260_IC_MAIN_CAUSE_HI);
		irq = __ilog2((irq & 0x0f000db7) & ppc_cached_irq_mask[1]);

		if (irq == -1)
			irq = -2; /* bogus interrupt, should never happen */
		else {
			if (irq >= 24) {
				irq_gpp = mv64x60_read(&bh,
					MV64x60_GPP_INTR_CAUSE);
				irq_gpp = __ilog2(irq_gpp &
					ppc_cached_irq_mask[2]);

				if (irq_gpp == -1)
					irq = -2;
				else {
					irq = irq_gpp + 64;
					mv64x60_write(&bh,
						MV64x60_GPP_INTR_CAUSE,
						~(1 << (irq - 64)));
				}
			} else
				irq += 32;
		}
	}

	(void)mv64x60_read(&bh, MV64x60_GPP_INTR_CAUSE);

	if (irq < 0)
		return (irq);
	else
		return (gt64260_irq_base + irq);
}

/* gt64260_unmask_irq()
 *
 *  This function enables an interrupt.
 *
 * Input Variable(s):
 *  unsigned int	interrupt number (IRQ0...IRQ95).
 *
 * Output Variable(s):
 *  None.
 *
 * Returns:
 *  void
 */
static void
gt64260_unmask_irq(unsigned int irq)
{
	irq -= gt64260_irq_base;

	if (irq > 31)
		if (irq > 63) /* unmask GPP irq */
			mv64x60_write(&bh, MV64x60_GPP_INTR_MASK,
				ppc_cached_irq_mask[2] |= (1 << (irq - 64)));
		else /* mask high interrupt register */
			mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_HI,
				ppc_cached_irq_mask[1] |= (1 << (irq - 32)));
	else /* mask low interrupt register */
		mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_LO,
			ppc_cached_irq_mask[0] |= (1 << irq));

	(void)mv64x60_read(&bh, MV64x60_GPP_INTR_MASK);
	return;
}

/* gt64260_mask_irq()
 *
 *  This function disables the requested interrupt.
 *
 * Input Variable(s):
 *  unsigned int	interrupt number (IRQ0...IRQ95).
 *
 * Output Variable(s):
 *  None.
 *
 * Returns:
 *  void
 */
static void
gt64260_mask_irq(unsigned int irq)
{
	irq -= gt64260_irq_base;

	if (irq > 31)
		if (irq > 63) /* mask GPP irq */
			mv64x60_write(&bh, MV64x60_GPP_INTR_MASK,
				ppc_cached_irq_mask[2] &= ~(1 << (irq - 64)));
		else /* mask high interrupt register */
			mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_HI,
				ppc_cached_irq_mask[1] &= ~(1 << (irq - 32)));
	else /* mask low interrupt register */
		mv64x60_write(&bh, GT64260_IC_CPU_INTR_MASK_LO,
			ppc_cached_irq_mask[0] &= ~(1 << irq));

	(void)mv64x60_read(&bh, MV64x60_GPP_INTR_MASK);
	return;
}

static irqreturn_t
gt64260_cpu_error_int_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	printk(KERN_ERR "gt64260_cpu_error_int_handler: %s 0x%08x\n",
		"Error on CPU interface - Cause regiser",
		mv64x60_read(&bh, MV64x60_CPU_ERR_CAUSE));
	printk(KERN_ERR "\tCPU error register dump:\n");
	printk(KERN_ERR "\tAddress low  0x%08x\n",
	       mv64x60_read(&bh, MV64x60_CPU_ERR_ADDR_LO));
	printk(KERN_ERR "\tAddress high 0x%08x\n",
	       mv64x60_read(&bh, MV64x60_CPU_ERR_ADDR_HI));
	printk(KERN_ERR "\tData low     0x%08x\n",
	       mv64x60_read(&bh, MV64x60_CPU_ERR_DATA_LO));
	printk(KERN_ERR "\tData high    0x%08x\n",
	       mv64x60_read(&bh, MV64x60_CPU_ERR_DATA_HI));
	printk(KERN_ERR "\tParity       0x%08x\n",
	       mv64x60_read(&bh, MV64x60_CPU_ERR_PARITY));
	mv64x60_write(&bh, MV64x60_CPU_ERR_CAUSE, 0);
	return IRQ_HANDLED;
}

static irqreturn_t
gt64260_pci_error_int_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	u32 val;
	unsigned int pci_bus = (unsigned int)dev_id;

	if (pci_bus == 0) {	/* Error on PCI 0 */
		val = mv64x60_read(&bh, MV64x60_PCI0_ERR_CAUSE);
		printk(KERN_ERR "%s: Error in PCI %d Interface\n",
			"gt64260_pci_error_int_handler", pci_bus);
		printk(KERN_ERR "\tPCI %d error register dump:\n", pci_bus);
		printk(KERN_ERR "\tCause register 0x%08x\n", val);
		printk(KERN_ERR "\tAddress Low    0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI0_ERR_ADDR_LO));
		printk(KERN_ERR "\tAddress High   0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI0_ERR_ADDR_HI));
		printk(KERN_ERR "\tAttribute      0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI0_ERR_DATA_LO));
		printk(KERN_ERR "\tCommand        0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI0_ERR_CMD));
		mv64x60_write(&bh, MV64x60_PCI0_ERR_CAUSE, ~val);
	}
	if (pci_bus == 1) {	/* Error on PCI 1 */
		val = mv64x60_read(&bh, MV64x60_PCI1_ERR_CAUSE);
		printk(KERN_ERR "%s: Error in PCI %d Interface\n",
			"gt64260_pci_error_int_handler", pci_bus);
		printk(KERN_ERR "\tPCI %d error register dump:\n", pci_bus);
		printk(KERN_ERR "\tCause register 0x%08x\n", val);
		printk(KERN_ERR "\tAddress Low    0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI1_ERR_ADDR_LO));
		printk(KERN_ERR "\tAddress High   0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI1_ERR_ADDR_HI));
		printk(KERN_ERR "\tAttribute      0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI1_ERR_DATA_LO));
		printk(KERN_ERR "\tCommand        0x%08x\n",
		       mv64x60_read(&bh, MV64x60_PCI1_ERR_CMD));
		mv64x60_write(&bh, MV64x60_PCI1_ERR_CAUSE, ~val);
	}
	return IRQ_HANDLED;
}

static int __init
gt64260_register_hdlrs(void)
{
	int rc;

	/* Register CPU interface error interrupt handler */
	if ((rc = request_irq(MV64x60_IRQ_CPU_ERR,
		gt64260_cpu_error_int_handler, SA_INTERRUPT, CPU_INTR_STR, 0)))
		printk(KERN_WARNING "Can't register cpu error handler: %d", rc);

	mv64x60_write(&bh, MV64x60_CPU_ERR_MASK, 0);
	mv64x60_write(&bh, MV64x60_CPU_ERR_MASK, 0x000000fe);

	/* Register PCI 0 error interrupt handler */
	if ((rc = request_irq(MV64360_IRQ_PCI0, gt64260_pci_error_int_handler,
		    SA_INTERRUPT, PCI0_INTR_STR, (void *)0)))
		printk(KERN_WARNING "Can't register pci 0 error handler: %d",
			rc);

	mv64x60_write(&bh, MV64x60_PCI0_ERR_MASK, 0);
	mv64x60_write(&bh, MV64x60_PCI0_ERR_MASK, 0x003c0c24);

	/* Register PCI 1 error interrupt handler */
	if ((rc = request_irq(MV64360_IRQ_PCI1, gt64260_pci_error_int_handler,
		    SA_INTERRUPT, PCI1_INTR_STR, (void *)1)))
		printk(KERN_WARNING "Can't register pci 1 error handler: %d",
			rc);

	mv64x60_write(&bh, MV64x60_PCI1_ERR_MASK, 0);
	mv64x60_write(&bh, MV64x60_PCI1_ERR_MASK, 0x003c0c24);

	return 0;
}

arch_initcall(gt64260_register_hdlrs);