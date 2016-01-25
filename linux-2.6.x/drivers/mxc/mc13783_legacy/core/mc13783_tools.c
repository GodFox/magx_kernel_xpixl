/*
 * Copyright 2006 Freescale Semiconductor, Inc. All Rights Reserved.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 *
 */

/*!
 * @file mc13783_tools.c
 * @brief This file contains Linux system functions.
 *
 * @ingroup MC13783
 */

/*
 * Includes
 */

#include "mc13783_config.h"

/*!
 * This function initializes a new semaphore.
 *
 * @param        sema   the semaphore
 *
 * @return       This function returns 0 if successful.
 */
int init_semaphore(osi_semaphore * sema)
{
	init_MUTEX_LOCKED(sema);
	return ERROR_NONE;
}

/*!
 * This function downs a semaphore.
 *
 * @param        sema   the semaphore
 *
 * @return       This function returns 0 if successful.
 */
int down_sema(osi_semaphore * sema)
{
	return down_interruptible(sema);
}

/*!
 * This function ups a semaphore.
 *
 * @param        sema   the semaphore
 *
 * @return       This function returns 0 if successful.
 */
int up_sema(osi_semaphore * sema)
{
	up(sema);
	return ERROR_NONE;
}
