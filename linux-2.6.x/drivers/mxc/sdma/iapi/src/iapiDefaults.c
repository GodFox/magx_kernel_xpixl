/******************************************************************************
 *
 * Copyright 2005 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 * 
 ******************************************************************************
 *
 * File: iapiDefaults.c
 *
 * $Id iapiDefaults.c $
 *
 * Description:
 *  This library is written in C to guarantee functionality and integrity in
 * the usage of SDMA virtual DMA channels. This API (Application Programming
 * Interface)  allow SDMA channels' access in an OPEN, READ, WRITE, CLOSE
 * fashion.
 *
 * Usage:
 *
 * Files:
 *
 *
* /
 *
 * $Log iapiDefaults.c $
 *
 *****************************************************************************/

/* ****************************************************************************
 * Include File Section
 ******************************************************************************/
#include "iapiDefaults.h"

/* ****************************************************************************
 * Global Variable Section
 ******************************************************************************/

/**
 * @brief System Call-back ISRs Table
 */
void (* callbackIsrTable[CH_NUM])(channelDescriptor* cd_p, void* arg);

/**
 * @brief User registered pointers table
 */
void * userArgTable[CH_NUM];

/**
 * @brief Pointer to the first CCB in the CCB array
 */
channelControlBlock * iapi_CCBHead = NULL;


/**Default channel description.
 *
 * Initialization values are:\n
 *  - channelNumber    = 0
 *  - bufferDescNumber = 1
 *  - bufferSize       = 8
 *  - blocking         = 0
 *  - callbackSynch    = DEFAULT_POLL
 *  - ownership        = CD_DEFAULT_OWNERSHIP
 *  - priority         = 1 
 *  - trust            = TRUE
 *  - useDataSize      = 0
 *  - dataSize         = 0
 *  - forceClose       = 0
 *  - scriptId         = 0
 *  - watermarkLevel   = 0
 *  - eventMask1       = 0
 *  - eventMask2       = 0
 *  - peripheralAddr   = NULL
 *  - callbackISR_ptr  = NULL
 *  - iapi_channelControlBlock = NULL
 */
channelDescriptor iapi_ChannelDefaults = {0, 1, 8, 0, DEFAULT_POLL, 
                                CD_DEFAULT_OWNERSHIP, 1, TRUE, 0, 0, 0, 0, 
                                0, 0x00, 0x00, NULL, NULL, NULL};

/**
 * Integrated error management
 */
unsigned int iapi_errno = 0;
volatile unsigned long iapi_SDMAIntr = 0;

/* Default config register.
 * Initialization values are:
 * dspdma used
 * Real-Time Debug pins disabled
 * AHB freq / core freq = 2
 * dynamic context switch
*/
configs_data iapi_ConfigDefaults = {1, 0, 0, 3};

/* ***************************************************************************/
