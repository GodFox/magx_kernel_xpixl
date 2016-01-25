/*
 * Copyright 2004 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2006 Motorola, Inc. */

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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  */

/*
 * DATE          AUTHOR         COMMMENT
 * ----          ------         --------
 * 10/04/2006    Motorola       Apply FSL SDMA script changes for WFN444
 */

/*!
 * @file sdma_script_code.h
 * @brief This file contains functions of SDMA scripts code initialization
 * 
 * The file was generated automatically. Based on sdma scripts library.
 * 
 * @ingroup SDMA
 */

#ifndef SDMA_SCRIPT_CODE_H
#define SDMA_SCRIPT_CODE_H

#ifdef CONFIG_MOT_WFN444

/*! 
* Following define start address of signature script 
*/
#define signature_ADDR	1023
/*! 
* Following define size of signature script 
*/
#define signature_SIZE	1
 
/*! 
* Following define start address of start script 
*/
#define start_ADDR	0
/*! 
* Following define size of start script 
*/
#define start_SIZE	20
 
/*! 
* Following define start address of core script 
*/
#define core_ADDR	80
/*! 
* Following define size of core script 
*/
#define core_SIZE	152
 
/*! 
* Following define start address of common script 
*/
#define common_ADDR	232
/*! 
* Following define size of common script 
*/
#define common_SIZE	191
 
/*! 
* Following define start address of ap_2_ap script 
*/
#define ap_2_ap_ADDR	423
/*! 
* Following define size of ap_2_ap script 
*/
#define ap_2_ap_SIZE	294
 
/*! 
* Following define start address of bp_2_bp script 
*/
#define bp_2_bp_ADDR	717
/*! 
* Following define size of bp_2_bp script 
*/
#define bp_2_bp_SIZE	112
 
/*! 
* Following define start address of ap_2_bp script 
*/
#define ap_2_bp_ADDR	829
/*! 
* Following define size of ap_2_bp script 
*/
#define ap_2_bp_SIZE	200
 
/*! 
* Following define start address of bp_2_ap script 
*/
#define bp_2_ap_ADDR	1029
/*! 
* Following define size of bp_2_ap script 
*/
#define bp_2_ap_SIZE	223
 
/*! 
* Following define start address of app_2_mcu script 
*/
#define app_2_mcu_ADDR	1252
/*! 
* Following define size of app_2_mcu script 
*/
#define app_2_mcu_SIZE	101
 
/*! 
* Following define start address of mcu_2_app script 
*/
#define mcu_2_app_ADDR	1353
/*! 
* Following define size of mcu_2_app script 
*/
#define mcu_2_app_SIZE	127
 
/*! 
* Following define start address of uart_2_mcu script 
*/
#define uart_2_mcu_ADDR	1480
/*! 
* Following define size of uart_2_mcu script 
*/
#define uart_2_mcu_SIZE	105
 
/*! 
* Following define start address of uartsh_2_mcu script 
*/
#define uartsh_2_mcu_ADDR	1585
/*! 
* Following define size of uartsh_2_mcu script 
*/
#define uartsh_2_mcu_SIZE	98
 
/*! 
* Following define start address of mcu_2_shp script 
*/
#define mcu_2_shp_ADDR	1683
/*! 
* Following define size of mcu_2_shp script 
*/
#define mcu_2_shp_SIZE	123
 
/*! 
* Following define start address of shp_2_mcu script 
*/
#define shp_2_mcu_ADDR	1806
/*! 
* Following define size of shp_2_mcu script 
*/
#define shp_2_mcu_SIZE	101
 
/*! 
* Following define start address of error script 
*/
#define error_ADDR	1907
/*! 
* Following define size of error script 
*/
#define error_SIZE	73
 
/*! 
* Following define start address of test script 
*/
#define test_ADDR	1980
/*! 
* Following define size of test script 
*/
#define test_SIZE	63
 
/*! 
* Following define start address of ap_2_bp_patched script 
*/
#define ap_2_bp_patched_ADDR	6144
/*! 
* Following define size of ap_2_bp_patched script 
*/
#define ap_2_bp_patched_SIZE	200
 
/*! 
* Following define start address of bp_2_ap_patched script 
*/
#define bp_2_ap_patched_ADDR	6344
/*! 
* Following define size of bp_2_ap_patched script 
*/
#define bp_2_ap_patched_SIZE	220
 
/*! 
* Following define start address of common_patched script 
*/
#define common_patched_ADDR	6564
/*! 
* Following define size of common_patched script 
*/
#define common_patched_SIZE	77
 
/*! 
* Following define start address of dsp_interrupt_only script 
*/
#define dsp_interrupt_only_ADDR	6641
/*! 
* Following define size of dsp_interrupt_only script 
*/
#define dsp_interrupt_only_SIZE	2
 
/*! 
* Following define start address of firi_2_mcu script 
*/
#define firi_2_mcu_ADDR	6643
/*! 
* Following define size of firi_2_mcu script 
*/
#define firi_2_mcu_SIZE	114
 
/*! 
* Following define start address of loop_dsp script 
*/
#define loop_dsp_ADDR	6757
/*! 
* Following define size of loop_dsp script 
*/
#define loop_dsp_SIZE	86
 
/*! 
* Following define start address of loopback_on_dsp_side script 
*/
#define loopback_on_dsp_side_ADDR	6843
/*! 
* Following define size of loopback_on_dsp_side script 
*/
#define loopback_on_dsp_side_SIZE	101
 
/*! 
* Following define start address of loopback_on_mcu_side script 
*/
#define loopback_on_mcu_side_ADDR	6944
/*! 
* Following define size of loopback_on_mcu_side script 
*/
#define loopback_on_mcu_side_SIZE	100
 
/*! 
* Following define start address of mcu_2_firi script 
*/
#define mcu_2_firi_ADDR	7044
/*! 
* Following define size of mcu_2_firi script 
*/
#define mcu_2_firi_SIZE	74
 
/*! 
* Following define start address of mcu_interrupt_only script 
*/
#define mcu_interrupt_only_ADDR	7118
/*! 
* Following define size of mcu_interrupt_only script 
*/
#define mcu_interrupt_only_SIZE	2
 
/*!
* Following define the start address of sdma ram 
*/

#define RAM_CODE_START_ADDR	6144
/*!
* Following define the size of sdma ram
*/
#define RAM_CODE_SIZE	976

/*!
* This function returns buffer that holds the image of SDMA RAM
*
* @return pointer to buffer that holds the image of SDMA RAM
*/

static const short sdma_code[] =
{
0xc147, 0x7d62, 0x0e70, 0x0611, 0x5016, 0xc158, 0x7c5f, 0x5ac0,
0x5bc8, 0xc795, 0x5bd0, 0x5df8, 0x6d47, 0x018c, 0xd9a4, 0x7d58,
0xd9b6, 0x7c58, 0x5ad8, 0x5be0, 0xc795, 0x5be8, 0x0688, 0xc791,
0x01b8, 0x04b8, 0x04d1, 0x0089, 0x7c01, 0x008c, 0x04a0, 0x01a0,
0x05df, 0x7c04, 0x6d03, 0xd8ae, 0x6503, 0x982b, 0x6dc5, 0xd87e,
0x7e46, 0x7f47, 0x65c0, 0x53fe, 0x6044, 0x00a3, 0xc79b, 0x6044,
0x58fe, 0x4c00, 0x7c0e, 0x52de, 0x53e6, 0x54ee, 0xd9c1, 0x7c25,
0x008e, 0xd9b6, 0x0688, 0x7c21, 0x5be6, 0x038c, 0xc791, 0x03b0,
0x5bee, 0x4900, 0x7cd4, 0x018c, 0x5df6, 0x52c6, 0x53ce, 0x54d6,
0xc166, 0x7c11, 0x008e, 0xc158, 0x0688, 0x7c0d, 0x5bce, 0x5cd6,
0x5dfe, 0x6d47, 0x038d, 0x0489, 0x55f6, 0x51d6, 0x0289, 0xc791,
0x02b0, 0x5ad6, 0x9817, 0x0600, 0x984a, 0x52c6, 0x53ce, 0x54d6,
0x0455, 0x0453, 0xc166, 0x987b, 0x0600, 0x9800, 0x0600, 0x9805,
0x0200, 0x980e, 0x0200, 0x9810, 0x0207, 0x680c, 0x9871, 0x0207,
0x68cc, 0x0107, 0x684c, 0x53fe, 0x6044, 0x00a3, 0x58f6, 0xc7a2,
0xd9c1, 0xc7a8, 0xc166, 0x0601, 0x0200, 0x9800, 0x3d03, 0x4d00,
0x7d0e, 0x0a04, 0x02a5, 0x00da, 0x7d01, 0x0288, 0x00a2, 0x0388,
0x008a, 0x7802, 0x62c8, 0x6a49, 0x7e1f, 0x7f1f, 0x008b, 0x0388,
0x0015, 0x0015, 0x6dcf, 0x52ee, 0x027f, 0x7d0e, 0x7803, 0x62e8,
0x6a4b, 0x0000, 0x7e11, 0x7f11, 0x008b, 0x3803, 0x65c0, 0x6dc5,
0x7802, 0x62c8, 0x6a49, 0x98ac, 0x7804, 0x62e8, 0x0210, 0x6a4b,
0x0000, 0x7e02, 0x7f02, 0x989c, 0x6a68, 0x0006, 0x53ee, 0x037f,
0x0388, 0x3b03, 0x0015, 0x0015, 0x7d05, 0x7803, 0x620b, 0x6a4b,
0x0000, 0x98bf, 0x7804, 0x620b, 0x0210, 0x6a4b, 0x0000, 0x008b,
0x7b02, 0x6209, 0x6a49, 0x7ea8, 0x6a68, 0x7fab, 0x0006, 0x0707,
0xc147, 0x7d65, 0x0e70, 0x0611, 0x5016, 0xc158, 0x7c62, 0x5ac0,
0x5bc8, 0xc795, 0x5bd0, 0x6d63, 0x018c, 0xd9a4, 0x7d5c, 0xd9b6,
0x7c5c, 0x5ad8, 0x5be0, 0xc795, 0x5be8, 0x0688, 0xc791, 0x01b8,
0x04b8, 0x04d1, 0x0089, 0x7c01, 0x008c, 0x04a0, 0x01a0, 0x05df,
0x7c09, 0x5dfe, 0x6d07, 0xd97d, 0x55fe, 0x6007, 0x00a5, 0xc79b,
0x6507, 0x98fa, 0x5dfe, 0x6dd5, 0xd94e, 0x55fe, 0x60d0, 0x00a5,
0xc79b, 0x65d0, 0x4900, 0x7c16, 0x018c, 0x5df6, 0x52c6, 0x53ce,
0x54d6, 0xc166, 0x7c24, 0x008e, 0xc158, 0x0688, 0x7c20, 0x5bce,
0x5cd6, 0x6d60, 0x038d, 0x0489, 0x51d6, 0x0289, 0xc791, 0x02b0,
0x5ad6, 0x55f6, 0x4c00, 0x7cca, 0x52de, 0x53e6, 0x54ee, 0xd9c1,
0x7c0c, 0x008e, 0xd9b6, 0x0688, 0x7c06, 0x5be6, 0x038c, 0xc791,
0x03b0, 0x5bee, 0x98de, 0x0100, 0x9919, 0x0200, 0x9919, 0x0089,
0x52de, 0x53e6, 0x54ee, 0x0455, 0x0453, 0xd9c1, 0x994c, 0x0600,
0x98c8, 0x0600, 0x98cd, 0x0200, 0x98d5, 0x0100, 0x98d7, 0x0007,
0x680c, 0x680c, 0x6007, 0x6807, 0x682b, 0x6828, 0x0007, 0x680c,
0x9945, 0x0007, 0x68cc, 0x55fe, 0x60d0, 0x684c, 0x00a5, 0x58f6,
0xc7a2, 0xd9c1, 0xc7a8, 0xc166, 0x0600, 0x98c8, 0x3d03, 0x4d00,
0x7d0e, 0x0a04, 0x02a5, 0x00da, 0x7d01, 0x0288, 0x00a2, 0x0388,
0x008a, 0x7802, 0x6269, 0x6ac8, 0x7ee8, 0x7fe3, 0x008b, 0x0388,
0x0015, 0x0015, 0x6ddf, 0x52ee, 0x027f, 0x7c10, 0x7804, 0x626b,
0x0210, 0x6ac8, 0x0000, 0x7ed9, 0x7fd4, 0x008b, 0x3803, 0x6ddd,
0x7802, 0x6269, 0x6ac8, 0x7ed1, 0x7fcc, 0x0006, 0x7803, 0x626b,
0x6ac8, 0x0000, 0x7eca, 0x7fc5, 0x996d, 0x0388, 0x3b03, 0x0015,
0x0015, 0x0588, 0x008d, 0x0a08, 0x05da, 0x7c01, 0x008a, 0x52ee,
0x027f, 0x7c15, 0x7803, 0x626b, 0x0210, 0x6a0b, 0x7ea8, 0x6a28,
0x7fa6, 0x0000, 0x0a08, 0x05da, 0x7c02, 0x2508, 0x9982, 0x008b,
0x7b02, 0x6269, 0x6a09, 0x7e9b, 0x6a28, 0x7f99, 0x0006, 0x7802,
0x626b, 0x6a0b, 0x998e, 0x0707, 0x0a70, 0x0211, 0x531a, 0x5202,
0x4a00, 0x7d0b, 0x0317, 0x0317, 0x0317, 0x0317, 0x029b, 0x0007,
0x6a23, 0x7f3d, 0x630b, 0x7e3b, 0x4b00, 0x0006, 0x0007, 0x6b23,
0x640b, 0x7e2a, 0x0470, 0x7c04, 0x650b, 0x7e26, 0x660b, 0x7e24,
0x0006, 0x0007, 0x6b07, 0x0430, 0x6c2b, 0x7f1e, 0x0471, 0x7c06,
0x038a, 0x1b04, 0x6b23, 0x630b, 0x7e22, 0x99cf, 0x1b0c, 0x0474,
0x7c09, 0x058a, 0x1d0c, 0x6d07, 0x6d23, 0x650b, 0x7e18, 0x055c,
0x6d2b, 0x7f15, 0x6a07, 0x6b2b, 0x7f12, 0x0473, 0x7d02, 0x0474,
0x7c01, 0x0101, 0x0472, 0x0006, 0x038a, 0x1b0c, 0x6b07, 0x6b23,
0x630b, 0x7e05, 0x035d, 0x6b2b, 0x7f02, 0x04d4, 0x0006, 0x680c,
0x0707, 0x0700, 0x99f1, 0x1e10, 0x0870, 0x0011, 0x5010, 0xc0ec,
0x7d39, 0x5ac0, 0x5bc8, 0x5ef8, 0xc0fe, 0x56f8, 0x7d02, 0x0200,
0x99fa, 0x6d07, 0x5df0, 0x0dff, 0x0511, 0x1dff, 0x05bc, 0x4d00,
0x7d17, 0x6ec3, 0x62c8, 0x7e28, 0x0264, 0x7d08, 0x0b70, 0x0311,
0x522b, 0x02b9, 0x4a00, 0x7c18, 0x0400, 0x9a09, 0x0212, 0x3aff,
0x008a, 0x05d8, 0x7d01, 0x008d, 0x0a10, 0x6ed3, 0x6ac8, 0xda44,
0x6a28, 0x7f17, 0x0b70, 0x0311, 0x5013, 0xda5c, 0x52c0, 0x53c8,
0xc10d, 0x7dd0, 0x0200, 0x99fa, 0x008f, 0x00d5, 0x7d01, 0x008d,
0xda44, 0x9a07, 0x0200, 0x99f7, 0x0007, 0x68cc, 0x6a28, 0x7f01,
0x9a42, 0x0007, 0x6a0c, 0x6a0c, 0x6207, 0x6a07, 0x6a2b, 0x6a28,
0x0007, 0x680c, 0x0454, 0x9a20, 0x05a0, 0x1e08, 0x6ec3, 0x0388,
0x3b03, 0x0015, 0x0015, 0x7802, 0x62c8, 0x6a0b, 0x7ee5, 0x6a28,
0x7fe8, 0x0000, 0x6ec1, 0x008b, 0x7802, 0x62c8, 0x6a09, 0x7edc,
0x6a28, 0x7fdf, 0x2608, 0x0006, 0x55f0, 0x6207, 0x02a5, 0x0dff,
0x0511, 0x1dff, 0x04b5, 0x049a, 0x0006, 0x0015, 0x0015, 0x0188,
0x3903, 0x0015, 0x0015, 0x0d04, 0x7803, 0x6d58, 0x6d68, 0x0000,
0x6958, 0x6d68, 0x0006, 0x0761, 0x7c01, 0x624a, 0x0760, 0x7c01,
0x6149, 0x7e07, 0x0761, 0x7c01, 0x6a4a, 0x0760, 0x7c01, 0x6949,
0x6968, 0x0006, 0x028b, 0x0804, 0x7804, 0x614b, 0x590a, 0x1a01,
0x0000, 0x7f10, 0x7e0f, 0x028b, 0x0803, 0x7804, 0x510a, 0x694b,
0x1a01, 0x0000, 0x510a, 0x696b, 0x7f05, 0x7e04, 0x2710, 0x0a10,
0x07da, 0x7de8, 0x0006, 0x028b, 0x008f, 0x0015, 0x0015, 0x7803,
0x614b, 0x590a, 0x1a01, 0x028b, 0x0761, 0x7c01, 0x654a, 0x0760,
0x7c01, 0x6649, 0x7f0f, 0x7e0e, 0x008f, 0x0015, 0x0015, 0x7803,
0x510a, 0x694b, 0x1a01, 0x0761, 0x7c01, 0x6d4a, 0x0760, 0x7c01,
0x6e49, 0x6968, 0x0006, 0xc0ec, 0x7d61, 0x0e70, 0x0611, 0x5016,
0x5ac0, 0x5bc8, 0x5ef8, 0xc0fe, 0x56f8, 0x5cd0, 0x7d02, 0x0200,
0x9ac1, 0x078d, 0x1a10, 0x5ad8, 0x6ae7, 0x63c8, 0x7eec, 0x0000,
0x4b00, 0x7dfa, 0x5be0, 0x0000, 0x5ef8, 0xc0fe, 0x56f8, 0x5ce8,
0x7cfa, 0x068f, 0x6ed5, 0x6dc5, 0x08ff, 0x0011, 0x28ff, 0x00bc,
0x0788, 0x0904, 0x00d1, 0x7d18, 0x0904, 0x06a5, 0x3e03, 0x3d03,
0x01a5, 0x3903, 0x0089, 0xdaff, 0x7e16, 0x07a1, 0x008f, 0x4e00,
0x7d01, 0x9afc, 0x6dcf, 0x6edf, 0x0015, 0x0015, 0xdaff, 0x7e0b,
0x3f03, 0x008f, 0x6dcd, 0x6edd, 0xdaff, 0x7e05, 0x9b04, 0x7802,
0x62d8, 0x0000, 0x0006, 0xc777, 0x078b, 0x0e70, 0x0611, 0x5016,
0x52d8, 0x53e0, 0x54e8, 0xc10d, 0x7d02, 0x0900, 0x9b10, 0x0901,
0x0e70, 0x0611, 0x5016, 0x52c0, 0x53c8, 0x54d0, 0xc10d, 0x7d01,
0x9b1a, 0x1901, 0x4902, 0x7da5, 0x0200, 0x9ac1, 0x0200, 0x9abb,
0xc147, 0x7d60, 0x0e70, 0x0611, 0x5016, 0x5ac0, 0x5bc8, 0x5ef8,
0xc158, 0x56f8, 0x5cd0, 0x7d02, 0x0600, 0x9b26, 0x078d, 0x1a10,
0x5ad8, 0x6a63, 0x634b, 0x7e33, 0x0000, 0x4b00, 0x7dfa, 0x5be0,
0x0000, 0x5ef8, 0xc158, 0x56f8, 0x038f, 0x5ce8, 0x7cf9, 0x068d,
0x058f, 0x6e44, 0x6d60, 0x08ff, 0x0011, 0x28ff, 0x00bc, 0x0788,
0x0904, 0x00d1, 0x7d16, 0x0f04, 0x06a5, 0x3e03, 0x3d03, 0x07a5,
0x3f03, 0xda73, 0xdb64, 0x00a7, 0x0788, 0x4e00, 0x7d08, 0x0810,
0x07d8, 0x7c02, 0xda82, 0xdb64, 0xda9b, 0xdb64, 0x9b68, 0xda65,
0x7f06, 0xda73, 0xdb64, 0x9b68, 0x7f02, 0x7e01, 0x0006, 0xc773,
0x078b, 0x0e70, 0x0611, 0x5016, 0x52d8, 0x53e0, 0x54e8, 0xc166,
0x7d02, 0x0900, 0x9b74, 0x0901, 0x0e70, 0x0611, 0x5016, 0x52c0,
0x53c8, 0x54d0, 0xc166, 0x7d01, 0x9b7e, 0x1901, 0x4902, 0x7da6,
0x0600, 0x9b26, 0x0600, 0x9b20, 0x0870, 0x0011, 0x5010, 0xc0ec,
0x7d43, 0x5ac0, 0x5bc8, 0x5ef8, 0xc0fe, 0x56f8, 0x7d02, 0x0200,
0x9b8a, 0x0b70, 0x0311, 0x6ed3, 0x6d03, 0x0dff, 0x0511, 0x1dff,
0x05bc, 0x4d00, 0x7d28, 0x522b, 0x02b9, 0x4a00, 0x7c04, 0x62c8,
0x7e1c, 0x0400, 0x9b9b, 0x008f, 0x00d5, 0x7d01, 0x008d, 0x05a0,
0x0060, 0x7c04, 0x6209, 0x7e14, 0x6ac8, 0x7f0f, 0x0015, 0x0060,
0x7c04, 0x620a, 0x7e0d, 0x6ac8, 0x7f08, 0x0015, 0x7802, 0x620b,
0x6ac8, 0x0000, 0x7e05, 0x7f01, 0x9b99, 0x0007, 0x68cc, 0x9bc2,
0x0007, 0x6a0c, 0x0454, 0x62c8, 0x7ef8, 0x5013, 0x52c0, 0x53c8,
0xc10d, 0x7dc0, 0x0200, 0x9b8a, 0x0200, 0x9b87, 0x0300, 0x9bce,
0x9bce
};
#else
#error UNSUPPORTED SDMA CONFIGURATION
#endif /* CONFIG_MOT_WFN444 */

#endif