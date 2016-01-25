/*
 * Copyright 2005 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/* 
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 * 
 * http://www.opensource.org/licenses/gpl-license.html 
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _mcuEpm_h
#define _mcuEpm_h

#include <asm/arch/hardware.h>

/*MCU_sdma (sdma.0.) regs definition */
#define SDMA_BASE_IO_ADDR IO_ADDRESS(SDMA_BASE_ADDR)

#define SDMA_H_C0PTR	*((volatile channelControlBlock **)(SDMA_BASE_IO_ADDR + 0x000))
#define SDMA_H_INTR             *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x004))
#define SDMA_H_STATSTOP         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x008))
#define SDMA_H_START            *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x00C))
#define SDMA_H_EVTOVR           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x010))
#define SDMA_H_DSPOVR           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x014))
#define SDMA_H_HOSTOVR          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x018))
#define SDMA_H_EVTPEND          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x01C))
#define SDMA_H_DSPENBL          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x020))
#define SDMA_H_RESET            *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x024))
#define SDMA_H_EVTERR           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x028))
#define SDMA_H_INTRMSK          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x02C))
#define SDMA_H_PSW              *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x030))
#define SDMA_H_EVTERRDBG        *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x034))
#define SDMA_H_CONFIG           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x038))
#define SDMA_ONCE_ENB           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x040))
#define SDMA_ONCE_DATA          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x044))
#define SDMA_ONCE_INSTR         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x048))
#define SDMA_ONCE_STAT          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x04C))
#define SDMA_ONCE_CMD           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x050))
#define SDMA_EVT_MIRROR         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x054))
#define SDMA_ILLINSTADDR        *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x058))
#define SDMA_CHN0ADDR           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x05C))
#define SDMA_ONCE_RTB           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x060))
#define SDMA_XTRIG_CONF1        *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x070))
#define SDMA_XTRIG_CONF2        *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x074))
#define SDMA_CHNENBL_0          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x080))
#define SDMA_CHNENBL_1          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x084))
#define SDMA_CHNENBL_2          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x088))
#define SDMA_CHNENBL_3          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x08C))
#define SDMA_CHNENBL_4          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x090))
#define SDMA_CHNENBL_5          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x094))
#define SDMA_CHNENBL_6          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x098))
#define SDMA_CHNENBL_7          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x09C))
#define SDMA_CHNENBL_8          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0A0))
#define SDMA_CHNENBL_9          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0A4))
#define SDMA_CHNENBL_10         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0A8))
#define SDMA_CHNENBL_11         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0AC))
#define SDMA_CHNENBL_12         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0B0))
#define SDMA_CHNENBL_13         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0B4))
#define SDMA_CHNENBL_14         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0B8))
#define SDMA_CHNENBL_15         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0BC))
#define SDMA_CHNENBL_16         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0C0))
#define SDMA_CHNENBL_17         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0C4))
#define SDMA_CHNENBL_18         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0C8))
#define SDMA_CHNENBL_19         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0CC))
#define SDMA_CHNENBL_20         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0D0))
#define SDMA_CHNENBL_21         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0D4))
#define SDMA_CHNENBL_22         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0D8))
#define SDMA_CHNENBL_23         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0DC))
#define SDMA_CHNENBL_24         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0E0))
#define SDMA_CHNENBL_25         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0E4))
#define SDMA_CHNENBL_26         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0E8))
#define SDMA_CHNENBL_27         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0EC))
#define SDMA_CHNENBL_28         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0F0))
#define SDMA_CHNENBL_29         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0F4))
#define SDMA_CHNENBL_30         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0F8))
#define SDMA_CHNENBL_31         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x0FC))
#define SDMA_CHNPRI_0           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x100))
#define SDMA_CHNPRI_1           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x104))
#define SDMA_CHNPRI_2           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x108))
#define SDMA_CHNPRI_3           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x10C))
#define SDMA_CHNPRI_4           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x110))
#define SDMA_CHNPRI_5           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x114))
#define SDMA_CHNPRI_6           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x118))
#define SDMA_CHNPRI_7           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x11C))
#define SDMA_CHNPRI_8           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x120))
#define SDMA_CHNPRI_9           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x124))
#define SDMA_CHNPRI_10          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x128))
#define SDMA_CHNPRI_11          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x12C))
#define SDMA_CHNPRI_12          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x130))
#define SDMA_CHNPRI_13          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x134))
#define SDMA_CHNPRI_14          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x138))
#define SDMA_CHNPRI_15          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x13C))
#define SDMA_CHNPRI_16          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x140))
#define SDMA_CHNPRI_17          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x144))
#define SDMA_CHNPRI_18          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x148))
#define SDMA_CHNPRI_19          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x14C))
#define SDMA_CHNPRI_20          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x150))
#define SDMA_CHNPRI_21          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x154))
#define SDMA_CHNPRI_22          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x158))
#define SDMA_CHNPRI_23          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x15C))
#define SDMA_CHNPRI_24          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x160))
#define SDMA_CHNPRI_25          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x164))
#define SDMA_CHNPRI_26          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x168))
#define SDMA_CHNPRI_27          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x16C))
#define SDMA_CHNPRI_28          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x170))
#define SDMA_CHNPRI_29          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x174))
#define SDMA_CHNPRI_30          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x178))
#define SDMA_CHNPRI_31          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x17C))
#define SDMA_ONCE_COUNT         *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x200))
#define SDMA_ONCE_ECTL          *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x204))
#define SDMA_ONCE_EAA           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x208))
#define SDMA_ONCE_EAB           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x20C))
#define SDMA_ONCE_EAM           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x210))
#define SDMA_ONCE_ED            *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x214))
#define SDMA_ONCE_EDM           *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x218))
#define SDMA_ONCE_PCMATCH       *((volatile unsigned long *)(SDMA_BASE_IO_ADDR + 0x21C))

#endif /* _mcuEpm_h */
