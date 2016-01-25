/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/module.h>
#include <asm/arch/gpio.h>
#include <asm/io.h>
#include <asm/arch/board.h>
#include <asm/arch/clock.h>
#include <linux/delay.h>

#include "iomux.h"

/*!
 * @file i30030ads_gpio.c
 *
 * @brief This file contains all the GPIO setup functions for the board.
 *
 * @ingroup GPIO
 */

/*!
 * Setup GPIO for a UART port to be active
 *
 * @param  port         a UART port
 * @param  no_irda      indicates if the port is used for SIR
 */
void gpio_uart_active(int port, int no_irda)
{
	/*
	 * Configure the IOMUX control registers for the UART signals
	 */
	switch (port) {
		/* UART 1 IOMUX Configs */
	case 0:
		if (no_irda == 1) {
			iomux_config_mux(PIN_UART_TXD1, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_RXD1, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_RTS1_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_CTS1_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
		}
		break;
		/* UART 2 IOMUX Configs */
	case 1:
		if (no_irda == 1) {
			iomux_config_mux(PIN_USB_VMOUT, OUTPUTCONFIG_ALT2, INPUTCONFIG_ALT2);	/* TXD */
			iomux_config_mux(PIN_USB_VPOUT, OUTPUTCONFIG_ALT2, INPUTCONFIG_ALT2);	/* RXD */
			iomux_config_mux(PIN_USB_XRXD, OUTPUTCONFIG_ALT1, INPUTCONFIG_ALT1);	/* RTS */
			iomux_config_mux(PIN_UART_CTS2_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_DSR2_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_DTR2_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_RI2_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_DCD2_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
		}
		break;
		/* UART 3 IOMUX Configs */
	case 2:
		if (no_irda == 1) {
			iomux_config_mux(PIN_UART_TXD3, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_RXD3, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_RTS3_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
			iomux_config_mux(PIN_UART_CTS3_B, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_FUNC);
		}
		break;
		/*
		 * UART 4 IOMUX Configs
		 * UART 4 is used for Irda
		 */
	case 3:
		if (no_irda == 0) {
			/*
			 * IOMUX configs for Irda pins
			 */
			iomux_config_mux(PIN_IRDA_TX4, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_NONE);
			iomux_config_mux(PIN_IRDA_RX4, INPUTCONFIG_NONE,
					 INPUTCONFIG_FUNC);
/* Different GPIO pins are used based on the board rev */
#ifdef CONFIG_MACH_I30030EVB_VER52
			iomux_config_mux(PIN_GPIO6, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_NONE);
			gpio_config(0, 6, true, GPIO_INT_NONE);
			/* Clear the SD/Mode signal */
			gpio_set_data(0, 6, 0);
#else
			iomux_config_mux(PIN_GPIO7, OUTPUTCONFIG_FUNC,
					 INPUTCONFIG_NONE);
			gpio_config(0, 7, true, GPIO_INT_NONE);
			/* Clear the SD/Mode signal */
			gpio_set_data(0, 7, 0);
#endif
		}
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for a UART port to be inactive
 *
 * @param  port         a UART port
 * @param  no_irda      indicates if the port is used for SIR
 */
void gpio_uart_inactive(int port, int no_irda)
{
	/*
	 * Disable the UART Transceiver by configuring the GPIO pin
	 */
	switch (port) {
	case 0:
		/*
		 * Disable the UART 1 Transceiver
		 */
		break;
	case 1:
		/*
		 * Disable the UART 2 Transceiver
		 * Note: Check this, currently modelled this way in Virtio
		 */
		break;
	case 2:
		/*
		 * Disable the UART 3 Transceiver
		 */
		break;
	case 3:
		/*
		 * Disable the Irda Transmitter
		 */
		break;
	default:
		break;
	}
}

/*!
 * Configure the IOMUX GPR register to receive shared SDMA UART events
 *
 * @param  port         a UART port
 */
void config_uartdma_event(int port)
{
	switch (port) {
	case 3:
		/* Configure to receive UART 4 SDMA events */
		iomux_config_gpr(MUX_PGP_FIRI, false);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for a Keypad to be active
 */
void gpio_keypad_active(void)
{
	iomux_config_mux(PIN_KEY_COL0, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL2, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL3, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL6, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_COL7, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW0, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW2, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW3, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW6, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(PIN_KEY_ROW7, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
}

EXPORT_SYMBOL(gpio_keypad_active);

/*!
 * Setup GPIO for a keypad to be inactive
 */
void gpio_keypad_inactive(void)
{
	iomux_config_mux(PIN_KEY_COL0, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL1, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL2, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL3, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL4, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL5, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL6, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_COL7, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW0, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW1, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW2, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW3, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW4, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW5, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW6, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(PIN_KEY_ROW7, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
}

EXPORT_SYMBOL(gpio_keypad_inactive);

/*!
 * Setup GPIO for a CSPI device to be active
 *
 * @param  cspi_mod         an CSPI device
 */
void gpio_spi_active(int cspi_mod)
{
	switch (cspi_mod) {
	case 0:
		/* SPI1_SS0 IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CS_0, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI1 MISO IOMux configuration */
		iomux_config_mux(PIN_CSPI1_DI, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI1 MOSI IOMux configuration */
		iomux_config_mux(PIN_CSPI1_DO, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI1_SS1 IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CS_1, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI1 CLK IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CK, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		break;
	case 1:
		/* SPI2_SS0 IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CS_0, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI2 MISO IOMux configuration */
		iomux_config_mux(PIN_CSPI2_DI, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI2 MOSI IOMux configuration */
		iomux_config_mux(PIN_CSPI2_DO, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI2_SS1 IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CS_1, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		/* SPI2 CLK IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CK, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for a CSPI device to be inactive
 *
 * @param  cspi_mod         a CSPI device
 */
void gpio_spi_inactive(int cspi_mod)
{
	switch (cspi_mod) {
	case 0:
		/* SPI1_SS0 IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CS_0, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI1 MISO IOMux configuration */
		iomux_config_mux(PIN_CSPI1_DI, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI1 MOSI IOMux configuration */
		iomux_config_mux(PIN_CSPI1_DO, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI1_SS1 IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CS_1, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI1 CLK IOMux configuration */
		iomux_config_mux(PIN_CSPI1_CK, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		break;
	case 1:
		/* SPI2_SS0 IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CS_0, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI2 MISO IOMux configuration */
		iomux_config_mux(PIN_CSPI2_DI, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI2 MOSI IOMux configuration */
		iomux_config_mux(PIN_CSPI2_DO, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI2_SS1 IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CS_1, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		/* SPI2 CLK IOMux configuration */
		iomux_config_mux(PIN_CSPI2_CK, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for an I2C device to be active
 *
 * @param  i2c_num         an I2C device
 */
void gpio_i2c_active(int i2c_num)
{
	switch (i2c_num) {
	case 0:
		iomux_config_mux(PIN_I2C_CLK, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_I2C_DAT, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for an I2C device to be inactive
 *
 * @param  i2c_num         an I2C device
 */
void gpio_i2c_inactive(int i2c_num)
{
	switch (i2c_num) {
	case 0:
		iomux_config_mux(PIN_I2C_CLK, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		iomux_config_mux(PIN_I2C_DAT, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_GPIO);
		break;
	default:
		break;
	}
}

/*!
 * This function configures the MC13783 intrrupt operations.
 *
 */
void gpio_mc13783_active(void *irq_handler)
{
	iomux_config_mux(PIN_PM_INT, OUTPUTCONFIG_ALT2, INPUTCONFIG_ALT2);
}

/*!
 * This function clears the MC13783 intrrupt.
 *
 */
void gpio_mc13783_clear_int(void)
{
}

/*!
 * This function return the SPI connected to MC13783.
 *
 */
int gpio_mc13783_get_spi(void)
{
	/* SPI2 = 1 */
	return 1;
}

/*!
 * This function return the SPI smave select for MC13783.
 *
 */
int gpio_mc13783_get_ss(void)
{
	/* SS = 0 */
	return 0;
}

/*!
 * Setup GPIO for SDHC to be active
 *
 * @param module SDHC module number
 */
void gpio_sdhc_active(int module)
{
	switch (module) {
	case 0:
		iomux_config_mux(PIN_MMC1_CLK, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC1_CMD, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC1_DATA_0, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC1_DATA_1, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC1_DATA_2, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC1_DATA_3, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		break;
	case 1:
		iomux_config_mux(PIN_MMC2_CLK, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC2_CMD, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC2_DATA_0, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC2_DATA_1, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC2_DATA_2, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		iomux_config_mux(PIN_MMC2_DATA_3, OUTPUTCONFIG_FUNC,
				 INPUTCONFIG_FUNC);
		break;
	default:
		break;
	}
}

EXPORT_SYMBOL(gpio_sdhc_active);

/*!
 * Setup GPIO for SDHC1 to be inactive
 *
 * @param module SDHC module number
 */
void gpio_sdhc_inactive(int module)
{
	switch (module) {
	case 0:
		iomux_config_mux(PIN_MMC1_CLK, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC1_CMD, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC1_DATA_0, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC1_DATA_1, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC1_DATA_2, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC1_DATA_3, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		break;
	case 1:
		iomux_config_mux(PIN_MMC2_CLK, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC2_CMD, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC2_DATA_0, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC2_DATA_1, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC2_DATA_2, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		iomux_config_mux(PIN_MMC2_DATA_3, OUTPUTCONFIG_GPIO,
				 INPUTCONFIG_NONE);
		break;
	default:
		break;
	}
}

EXPORT_SYMBOL(gpio_sdhc_inactive);

/*
 * Probe for the card. If present the GPIO data would be set.
 */
int sdhc_find_card(void)
{
	return gpio_get_data(0, 3);
}

EXPORT_SYMBOL(sdhc_find_card);

/*!
 * Invert the IOMUX/GPIO for SDHC1 SD1_DET.
 *
 * @param flag Flag represents whether the card is inserted/removed.
 *             Using this sensitive level of GPIO signal is changed.
 *
 **/
void sdhc_intr_clear(int flag)
{
	if (flag) {
		set_irq_type(IOMUX_TO_IRQ(PIN_GPIO3), IRQT_FALLING);
	} else {
		set_irq_type(IOMUX_TO_IRQ(PIN_GPIO3), IRQT_RISING);
	}
}

EXPORT_SYMBOL(sdhc_intr_clear);

/*!
 * Setup the IOMUX/GPIO for SDHC1 SD1_DET.
 *
 * @param  host Pointer to MMC/SD host structure.
 * @param  handler      GPIO ISR function pointer for the GPIO signal.
 * @return The function returns 0 on success and -1 on failure.
 *
 **/
int sdhc_intr_setup(void *host,
		    irqreturn_t(*handler) (int, void *, struct pt_regs *))
{
	int ret;

	/* MMC1_SDDET is connected to GPIO37 */
	iomux_config_mux(PIN_GPIO3, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);

	/* check if a card in the slot if so we need to start with
	 * the proper edge definition
	 */
	sdhc_intr_clear(sdhc_find_card());

	ret = request_irq(IOMUX_TO_IRQ(PIN_GPIO3), handler, 0, "MXCMMC", host);
	return ret;
}

EXPORT_SYMBOL(sdhc_intr_setup);

/*!
 * Clear the IOMUX/GPIO for SDHC1 SD1_DET.
 */
void sdhc_intr_destroy(void *host)
{
	free_irq(IOMUX_TO_IRQ(PIN_GPIO3), host);
}

EXPORT_SYMBOL(sdhc_intr_destroy);

/*
 * Find the minimum clock for SDHC.
 *
 * @param clk SDHC module number.
 * @return Returns the minimum SDHC clock.
 */
unsigned int sdhc_get_min_clock(enum mxc_clocks clk)
{
	return (mxc_get_clocks(SDHC1_CLK) / 8) / 32;
}

EXPORT_SYMBOL(sdhc_get_min_clock);

/*
 * Find the maximum clock for SDHC.
 * @param clk SDHC module number.
 * @return Returns the maximum SDHC clock.
 */
unsigned int sdhc_get_max_clock(enum mxc_clocks clk)
{
	return mxc_get_clocks(SDHC1_CLK) / 4;
}

EXPORT_SYMBOL(sdhc_get_max_clock);

/*!
 * Setup GPIO for LCD to be active
 *
 */
void gpio_lcd_active(void)
{
	/* TODO: */
}

/*!
 * Setup GPIO for LCD to be inactive
 *
 */
void gpio_lcd_inactive(void)
{
	/* TODO: */
}

/*!
 * Setup GPIO for Camera sensor to be active
 *
 */
void gpio_sensor_active(void)
{
	/* enable flash strobe */
	iomux_config_mux(PIN_GPIO0, OUTPUTCONFIG_ALT2, INPUTCONFIG_ALT2);
}

/*!
 * Setup GPIO for Camera sensor to be active
 *
 */
void gpio_sensor_inactive(void)
{
	/* enable flash strobe */
	iomux_config_mux(PIN_GPIO0, OUTPUTCONFIG_GPIO, INPUTCONFIG_GPIO);
}

/*!
 * camera reset/standby setup
 *
 */
void gpio_sensor_setup(void)
{
	/* Reset line */
	iomux_config_mux(PIN_GPIO14, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	gpio_config(0, 14, true, GPIO_INT_NONE);

	/* Standby line */
	iomux_config_mux(PIN_GPIO15, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	gpio_config(0, 15, true, GPIO_INT_NONE);

	/* Flash light */
	iomux_config_mux(PIN_GPIO0, OUTPUTCONFIG_ALT2, INPUTCONFIG_ALT2);
}

/*!
 * camera reset
 * @param flag: true - reset  false - out of reset.
 */
void gpio_sensor_reset(bool flag)
{
	if (flag)
		gpio_set_data(0, 14, 0);
	else
		gpio_set_data(0, 14, 1);
}

/*!
 * camera suspend
 * @param suspend: true - suspend mode false - normal operational mode.
 */
void gpio_sensor_suspend(bool flag)
{
	if (flag)
		gpio_set_data(0, 15, 0);
	else
		gpio_set_data(0, 15, 1);
}

void slcd_gpio_config(void)
{
	// PIN_GPIO36 is actually bit 23 in in GPIO module!!!
	gpio_set_data(0, 23, 0);
	gpio_config(0, 23, true, GPIO_INT_NONE);
	iomux_config_mux(PIN_GPIO36, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	msleep(1);

	gpio_set_data(0, 23, 1);
	msleep(1);
}
