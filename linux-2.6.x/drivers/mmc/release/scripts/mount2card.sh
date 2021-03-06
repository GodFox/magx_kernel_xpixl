#!/bin/sh
# Copyright (c) 2006 , Motorola Inc

# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at
# your option) any later version.  You should have
# received a copy of the GNU General Public License
# along with this program; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave,
# Cambridge, MA 02139, USA

# This Program is distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of
# MERCHANTIBILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

#Date           Author          Comment
#10/20/06       Motorola        create
#

TFLASH_DEV_NODE=/dev/mmc/blk0/part1
TFLASH_MOUNT_PT=/mmc/mmca1

SD_DEV_NODE=/dev/mmc/blk1/part1
SD_MOUNT_PT=/mmc/mmcb1

MOUNT_OPTIONS="-o uid=2000 -o gid=233 -o iocharset=utf8 -o shortname=mixed -o umask=002 -o noatime"

mount -t vfat $TFLASH_DEV_NODE $TFLASH_MOUNT_PT $MOUNT_OPTIONS

mount -t vfat $SD_DEV_NODE $SD_MOUNT_PT $MOUNT_OPTIONS


