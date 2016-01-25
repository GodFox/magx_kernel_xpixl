/*
 *
 * Changelog:
 * Date         Author          Comment
 * 09/18/2006   Motorola       added support for Power Management, and for Bluetooth use of UART
*/

/*
 * Copyright 2006 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2006 Motorola, Inc.
 */
/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
/*!
 * @file serial.c
 *
 * @brief This file contains the UART initiliazation.
 *
 * @ingroup System
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/serial.h>
#include <asm/hardware.h>
#include <asm/arch/mxc_uart.h>
#include <asm/arch/spba.h>

#if defined(CONFIG_SERIAL_MXC) || defined(CONFIG_SERIAL_MXC_MODULE)

/* UART 1 configuration */
/*!
 * This option allows to choose either an interrupt-driven software controlled
 * hardware flow control (set this option to 0) or hardware-driven hardware
 * flow control (set this option to 1).
 */
#ifdef CONFIG_MOT_FEAT_SERIAL
/* Enable support for hardware flow control for bluetooth UART */
#define UART1_HW_FLOW           1
#else
/* No flow control for Irda UART */
#define UART1_HW_FLOW           0
#endif

/*!
 * This specifies the threshold at which the CTS pin is deasserted by the
 * RXFIFO. Set this value in Decimal to anything from 0 to 32 for
 * hardware-driven hardware flow control. Read the HW spec while specifying
 * this value. When using interrupt-driven software controlled hardware
 * flow control set this option to -1.
 */
#ifdef CONFIG_MOT_FEAT_SERIAL
/*!
 * Bluetooth controllers all stop after current byte complete. So configure
 * flow control to trigger after receiving 30 bytes in FIFO. This value is
 * written to CTSTL(15:10) of UCR4. This value is meant to be FIFO size - 2.
 * FIFO size for current HW is 32.
 */
#define UART1_UCR4_CTSTL        30
#else
#define UART1_UCR4_CTSTL        -1
#endif

/*!
 * This is option to enable (set this option to 1) or disable DMA data transfer
 */
/* No DMA for Irda UART */
#define UART1_DMA_ENABLE        0
/*!
 * Specify the size of the DMA receive buffer. The minimum buffer size is 512
 * bytes. The buffer size should be a multiple of 256.
 */
#define UART1_DMA_RXBUFSIZE     1024
/*!
 * Specify the MXC UART's Receive Trigger Level. This controls the threshold at
 * which a maskable interrupt is generated by the RxFIFO. Set this value in
 * Decimal to anything from 0 to 32. Read the HW spec while specifying this
 * value. 
 */
#ifdef CONFIG_MOT_FEAT_SERIAL
/*!
 * We set it to trigger 2 bytes before CTS is asserted. This value is
 * written to RXTL(5:0) of UFIFOCR.
 */
#define UART1_UFCR_RXTL         (UART1_UCR4_CTSTL - 2)
#else
#define UART1_UFCR_RXTL         16
#endif

/*!
 * Specify the MXC UART's Transmit Trigger Level. This controls the threshold at
 * which a maskable interrupt is generated by the TxFIFO. Set this value in
 * Decimal to anything from 0 to 32. Read the HW spec while specifying this
 * value. 
 */
#ifdef CONFIG_MOT_FEAT_SERIAL
/*!
 *This value is written to TXTL(15:10) of UFIFOCR.
 */
#define UART1_UFCR_TXTL         2
#else
#define UART1_UFCR_TXTL         16
#endif

/* UART 2 configuration */
#define UART2_HW_FLOW           1
#define UART2_UCR4_CTSTL        16
#define UART2_DMA_ENABLE        0
#define UART2_DMA_RXBUFSIZE     1024
#ifdef CONFIG_MOT_FEAT_SERIAL
#define UART2_UFCR_RXTL         (UART2_UCR4_CTSTL - 2)
#define UART2_UFCR_TXTL         2
#else
#define UART2_UFCR_RXTL         16
#define UART2_UFCR_TXTL         16
#endif
/* UART 3 configuration */
#define UART3_HW_FLOW           1
#define UART3_UCR4_CTSTL        16
#ifdef CONFIG_MOT_FEAT_SERIAL
#if defined(CONFIG_MOT_FEAT_DISABLE_UART3_DMA)
#define UART3_DMA_ENABLE        0
#else
#define UART3_DMA_ENABLE        1
#endif /* ! CONFIG_MOT_FEAT_DISABLE_UART3_DMA */
#else
#define UART3_DMA_ENABLE        1
#endif /* CONFIG_MOT_FEAT_SERIAL */
#define UART3_DMA_RXBUFSIZE     1024
#ifdef CONFIG_MOT_FEAT_SERIAL
#define UART3_UFCR_RXTL         (UART3_UCR4_CTSTL - 2)
#define UART3_UFCR_TXTL         2
#else
#define UART3_UFCR_RXTL         16
#define UART3_UFCR_TXTL         16
#endif
/*
 * UART Chip level Configuration that a user may not have to edit. These
 * configuration vary depending on how the UART module is integrated with
 * the ARM core
 */
/*
 * Is the MUXED interrupt output sent to the ARM core
 */
#define INTS_NOTMUXED           0
#define INTS_MUXED              1

/* UART 1 configuration */
/*!
 * This define specifies whether the muxed ANDed interrupt line or the
 * individual interrupts from the UART port is integrated with the ARM core.
 * There exists a define like this for each UART port. Valid values that can
 * be used are \b INTS_NOTMUXED or \b INTS_MUXED.
 */
#define UART1_MUX_INTS          INTS_NOTMUXED
/*!
 * This define specifies the transmitter interrupt number or the interrupt
 * number of the ANDed interrupt in case the interrupts are muxed. There exists
 * a define like this for each UART port.
 */
#define UART1_INT1              INT_UART1_TX
/*!
 * This define specifies the receiver interrupt number. If the interrupts of
 * the UART are muxed, then we specify here a dummy value -1. There exists a
 * define like this for each UART port.
 */
#define UART1_INT2              INT_UART1_RX
/*!
 * This specifies the master interrupt number. If the interrupts of the UART
 * are muxed, then we specify here a dummy value of -1. There exists a define
 * like this for each UART port.
 */
#define UART1_INT3              INT_UART1_MINT
/*!
 * This specifies if the UART is a shared peripheral. It holds the shared
 * peripheral number if it is shared or -1 if it is not shared. There exists
 * a define like this for each UART port.
 */
#define UART1_SHARED_PERI       -1
/* UART 2 configuration */
#define UART2_MUX_INTS          INTS_NOTMUXED
#define UART2_INT1              INT_UART2_TX
#define UART2_INT2              INT_UART2_RX
#define UART2_INT3              INT_UART2_MINT
#define UART2_SHARED_PERI       -1
/* UART 3 configuration */
#define UART3_MUX_INTS          INTS_NOTMUXED
#define UART3_INT1              INT_UART3_TX
#define UART3_INT2              INT_UART3_RX
#define UART3_INT3              INT_UART3_MINT
#define UART3_SHARED_PERI       SPBA_UART3

/*!
 * This is an array where each element holds information about a UART port,
 * like base address of the UART, interrupt numbers etc. This structure is
 * passed to the serial_core.c file. Based on which UART is used, the core file
 * passes back the appropriate port structure as an argument to the control
 * functions.
 */
static uart_mxc_port mxc_ports[] = {
	[0] = {
	       .port = {
			.membase = (void *)IO_ADDRESS(UART1_BASE_ADDR),
			.mapbase = UART1_BASE_ADDR,
			.iotype = SERIAL_IO_MEM,
			.irq = UART1_INT1,
			.fifosize = 32,
			.flags = ASYNC_BOOT_AUTOCONF,
			.line = 0,
			},
	       .ints_muxed = UART1_MUX_INTS,
	       .irqs = {UART1_INT2, UART1_INT3},
	       .mode = UART1_MODE,
	       .ir_mode = UART1_IR,
	       .enabled = UART1_ENABLED,
	       .hardware_flow = UART1_HW_FLOW,
	       .cts_threshold = UART1_UCR4_CTSTL,
	       .dma_enabled = UART1_DMA_ENABLE,
	       .dma_rxbuf_size = UART1_DMA_RXBUFSIZE,
	       .rx_threshold = UART1_UFCR_RXTL,
	       .tx_threshold = UART1_UFCR_TXTL,
	       .shared = UART1_SHARED_PERI,
	       .clock_id = UART1_BAUD,
#ifdef CONFIG_MOT_FEAT_SERIAL
	       .uart_suspended = true,
#endif
	       },
	[1] = {
	       .port = {
			.membase = (void *)IO_ADDRESS(UART2_BASE_ADDR),
			.mapbase = UART2_BASE_ADDR,
			.iotype = SERIAL_IO_MEM,
			.irq = UART2_INT1,
			.fifosize = 32,
			.flags = ASYNC_BOOT_AUTOCONF,
			.line = 1,
			},
	       .ints_muxed = UART2_MUX_INTS,
	       .irqs = {UART2_INT2, UART2_INT3},
	       .mode = UART2_MODE,
	       .ir_mode = UART2_IR,
	       .enabled = UART2_ENABLED,
	       .hardware_flow = UART2_HW_FLOW,
	       .cts_threshold = UART2_UCR4_CTSTL,
	       .dma_enabled = UART2_DMA_ENABLE,
	       .dma_rxbuf_size = UART2_DMA_RXBUFSIZE,
	       .rx_threshold = UART2_UFCR_RXTL,
	       .tx_threshold = UART2_UFCR_TXTL,
	       .shared = UART2_SHARED_PERI,
	       .clock_id = UART2_BAUD,
#ifdef CONFIG_MOT_FEAT_SERIAL
	       .uart_suspended = true,
#endif
	       },
	[2] = {
	       .port = {
			.membase = (void *)IO_ADDRESS(UART3_BASE_ADDR),
			.mapbase = UART3_BASE_ADDR,
			.iotype = SERIAL_IO_MEM,
			.irq = UART3_INT1,
			.fifosize = 32,
			.flags = ASYNC_BOOT_AUTOCONF,
			.line = 2,
			},
	       .ints_muxed = UART3_MUX_INTS,
	       .irqs = {UART3_INT2, UART3_INT3},
	       .mode = UART3_MODE,
	       .ir_mode = UART3_IR,
	       .enabled = UART3_ENABLED,
	       .hardware_flow = UART3_HW_FLOW,
	       .cts_threshold = UART3_UCR4_CTSTL,
	       .dma_enabled = UART3_DMA_ENABLE,
	       .dma_rxbuf_size = UART3_DMA_RXBUFSIZE,
	       .rx_threshold = UART3_UFCR_RXTL,
	       .tx_threshold = UART3_UFCR_TXTL,
	       .shared = UART3_SHARED_PERI,
	       .clock_id = UART3_BAUD,
#ifdef CONFIG_MOT_FEAT_SERIAL
	       .uart_suspended = true,
#endif
	       },
};

static struct platform_device mxc_uart_device1 = {
	.name = "mxcintuart",
	.id = 0,
	.dev = {
		.platform_data = &mxc_ports[0],
		},
};

static struct platform_device mxc_uart_device2 = {
	.name = "mxcintuart",
	.id = 1,
	.dev = {
		.platform_data = &mxc_ports[1],
		},
};

static struct platform_device mxc_uart_device3 = {
	.name = "mxcintuart",
	.id = 2,
	.dev = {
		.platform_data = &mxc_ports[2],
		},
};

static int __init mxc_init_uart(void)
{
	/* Register all the MXC UART platform device structures */
	platform_device_register(&mxc_uart_device1);
	platform_device_register(&mxc_uart_device2);

	/* Grab ownership of shared UART 3, only when enabled */
#if UART3_ENABLED == 1
#if UART3_DMA_ENABLE == 1
	spba_take_ownership(UART3_SHARED_PERI, (SPBA_MASTER_A | SPBA_MASTER_C));
#else
	spba_take_ownership(UART3_SHARED_PERI, SPBA_MASTER_A);
#endif				/* UART3_DMA_ENABLE */
	platform_device_register(&mxc_uart_device3);
#endif				/* UART3_ENABLED */
	return 0;
}

#else
static int __init mxc_init_uart(void)
{
	return 0;
}
#endif

arch_initcall(mxc_init_uart);
