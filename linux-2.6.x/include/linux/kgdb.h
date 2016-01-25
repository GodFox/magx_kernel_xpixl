/*
 * include/linux/kgdb.h
 *
 * This provides the hooks and functions that KGDB needs to share between
 * the core, I/O and arch-specific portions.
 *
 * Author: Amit Kale <amitkale@linsyssoft.com> and
 *         Tom Rini <trini@kernel.crashing.org>
 *
 * 2001-2004 (c) Amit S. Kale and 2003-2004 (c) MontaVista Software, Inc.
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifdef __KERNEL__
#ifndef _KGDB_H_
#define _KGDB_H_

#include <asm/atomic.h>
#include <asm/kgdb.h>
#include <linux/serial_8250.h>

#ifdef CONFIG_KGDB
struct tasklet_struct;
struct pt_regs;
struct task_struct;
struct uart_port;

#ifdef CONFIG_KGDB_CONSOLE
extern struct console kgdbcons;
#endif

/* To enter the debugger explicitly. */
extern void breakpoint(void);
extern int kgdb_connected;
extern int kgdb_may_fault;
extern struct tasklet_struct kgdb_tasklet_breakpoint;

extern atomic_t kgdb_setting_breakpoint;
extern atomic_t cpu_doing_single_step;

extern struct task_struct *kgdb_usethread, *kgdb_contthread;

enum kgdb_bptype {
	bp_breakpoint = '0',
	bp_hardware_breakpoint,
	bp_write_watchpoint,
	bp_read_watchpoint,
	bp_access_watchpoint
};

enum kgdb_bpstate {
	bp_disabled,
	bp_enabled
};

struct kgdb_bkpt {
	unsigned long bpt_addr;
	unsigned char saved_instr[BREAK_INSTR_SIZE];
	enum kgdb_bptype type;
	enum kgdb_bpstate state;
};

#ifndef MAX_BREAKPOINTS
#define MAX_BREAKPOINTS		16
#endif

#define KGDB_HW_BREAKPOINT	1

/* Required functions. */
/**
 *	regs_to_gdb_regs - Convert ptrace regs to GDB regs
 *	@gdb_regs: A pointer to hold the registers in the order GDB wants.
 *	@regs: The &struct pt_regs of the current process.
 *
 *	Convert the pt_regs in @regs into the format for registers that
 *	GDB expects, stored in @gdb_regs.
 */
extern void regs_to_gdb_regs(unsigned long *gdb_regs, struct pt_regs *regs);

/**
 *	sleeping_regs_to_gdb_regs - Convert ptrace regs to GDB regs
 *	@gdb_regs: A pointer to hold the registers in the order GDB wants.
 *	@p: The &struct task_struct of the desired process.
 *
 *	Convert the register values of the sleeping process in @p to
 *	the format that GDB expects.
 *	This function is called when kgdb does not have access to the
 *	&struct pt_regs and therefore it should fill the gdb registers
 *	@gdb_regs with what has	been saved in &struct thread_struct
 *	thread field during switch_to.
 */
extern void sleeping_thread_to_gdb_regs(unsigned long *gdb_regs,
					struct task_struct *p);

/**
 *	gdb_regs_to_regs - Convert GDB regs to ptrace regs.
 *	@gdb_regs: A pointer to hold the registers we've recieved from GDB.
 *	@regs: A pointer to a &struct pt_regs to hold these values in.
 *
 *	Convert the GDB regs in @gdb_regs into the pt_regs, and store them
 *	in @regs.
 */
extern void gdb_regs_to_regs(unsigned long *gdb_regs, struct pt_regs *regs);

/**
 *	kgdb_arch_handle_exception - Handle architecture specific GDB packets.
 *	@vector: The error vector of the exception that happened.
 *	@signo: The signal number of the exception that happened.
 *	@err_code: The error code of the exception that happened.
 *	@InBuffer: The buffer of the packet we have read.
 *	@outBuffer: The buffer, of %BUFMAX to write a packet into.
 *	@regs: The &struct pt_regs of the current process.
 *
 *	This function MUST handle the 'c' and 's' command packets,
 *	as well packets to set / remove a hardware breakpoint, if used.
 *	If there are additional packets which the hardware needs to handle,
 *	they are handled here.  The code should return -1 if it wants to
 *	process more packets, and a %0 or %1 if it wants to exit from the
 *	kgdb hook.
 */
extern int kgdb_arch_handle_exception(int vector, int signo, int err_code,
				      char *inBuffer, char *outBuffer,
				      struct pt_regs *regs);

extern unsigned long kgdb_fault_jmp_regs[];
/**
 * TODO
 */
extern asmlinkage int kgdb_fault_setjmp(unsigned long *curr_context);
extern asmlinkage void kgdb_fault_longjmp(unsigned long *curr_context);

/* Optional functions. */
extern int kgdb_arch_init(void);
extern void kgdb_disable_hw_debug(struct pt_regs *regs);
extern void kgdb_post_master_code(struct pt_regs *regs, int eVector,
				  int err_code);
extern void kgdb_roundup_cpus(unsigned long flags);
extern int kgdb_set_hw_break(unsigned long addr);
extern int kgdb_remove_hw_break(unsigned long addr);
extern void kgdb_remove_all_hw_break(void);
extern void kgdb_correct_hw_break(void);
extern void kgdb_shadowinfo(struct pt_regs *regs, char *buffer,
			    unsigned threadid);
extern struct task_struct *kgdb_get_shadow_thread(struct pt_regs *regs,
						  int threadid);
extern struct pt_regs *kgdb_shadow_regs(struct pt_regs *regs, int threadid);

/**
 * struct kgdb_arch - Desribe architecture specific values.
 * @gdb_bpt_instr: The instruction to trigger a breakpoint.
 * @flags: Flags for the breakpoint, currently just %KGDB_HW_BREAKPOINT.
 * @shadowth: A value of %1 indicates we shadow information on processes.
 * @set_breakpoint: Allow an architecture to specify how to set a software
 * breakpoint.
 * @remove_breakpoint: Allow an architecture to specify how to remove a
 * software breakpoint.
 * @set_hw_breakpoint: Allow an architecture to specify how to set a hardware
 * breakpoint.
 * @remove_hw_breakpoint: Allow an architecture to specify how to remove a
 * hardware breakpoint.
 *
 * The @shadowth flag is an option to shadow information not retrievable by
 * gdb otherwise.  This is deprecated in favor of a binutils which supports
 * CFI macros.
 */
struct kgdb_arch {
	unsigned char gdb_bpt_instr[BREAK_INSTR_SIZE];
	unsigned long flags;
	unsigned shadowth;
	int (*set_breakpoint) (unsigned long, char *);
	int (*remove_breakpoint)(unsigned long, char *);
	int (*set_hw_breakpoint)(unsigned long, int, enum kgdb_bptype);
	int (*remove_hw_breakpoint)(unsigned long, int, enum kgdb_bptype);
};

/* Thread reference */
typedef unsigned char threadref[8];

/**
 * struct kgdb_io - Desribe the interface for an I/O driver to talk with KGDB.
 * @read_char: Pointer to a function that will return one char.
 * @write_char: Pointer to a function that will write one char.
 * @flush: Pointer to a function that will flush any pending writes.
 * @init: Pointer to a function that will initialize the device.
 * @late_init: Pointer to a function that will do any setup that has
 * other dependencies.
 *
 * The @init and @late_init function pointers allow for an I/O driver
 * such as a serial driver to fully initialize the port with @init and
 * be called very early, yet safely call request_irq() later in the boot
 * sequence.
 *
 * @init is allowed to return a non-0 return value to indicate failure.
 * If this is called early on, then KGDB will try again when it would call
 * @late_init.  If it has failed later in boot as well, the user will be
 * notified.
 */
struct kgdb_io {
	int (*read_char) (void);
	void (*write_char) (int);
	void (*flush) (void);
	int (*init) (void);
	void (*late_init) (void);
};

extern struct kgdb_io kgdb_io_ops;
extern struct kgdb_arch arch_kgdb_ops;
extern int kgdb_initialized;

extern void kgdb8250_add_port(int i, struct uart_port *serial_req);
extern void kgdb8250_add_platform_port(int i, struct plat_serial8250_port *serial_req);

extern int kgdb_hex2long(char **ptr, long *longValue);
extern char *kgdb_mem2hex(char *mem, char *buf, int count);
extern char *kgdb_hex2mem(char *buf, char *mem, int count);
extern int kgdb_get_mem(char *addr, unsigned char *buf, int count);
extern int kgdb_set_mem(char *addr, unsigned char *buf, int count);
extern int kgdb_handle_exception(int exVector, int signo, int err_code,
				struct pt_regs *regs);
extern void kgdb_nmihook(int cpu, void *regs);
extern int debugger_step;
extern atomic_t debugger_active;
#else
/* Stubs for when KGDB is not set. */
static const atomic_t debugger_active = ATOMIC_INIT(0);
#endif				/* CONFIG_KGDB */

#ifdef CONFIG_KGDB_8250
extern int kgdb8250_ttyS;
#else
/* Stubs for when KGDB 8250 is not set. */
#define kgdb8250_ttyS	-1
#endif
#endif				/* _KGDB_H_ */
#endif				/* __KERNEL__ */
