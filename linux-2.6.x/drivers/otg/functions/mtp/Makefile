#
# Copyright (c) 2006 Motorola, All Rights Reserved
#
# This program is licensed under a BSD license with the following terms:
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#   *  Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#   *  Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#   *  Neither the name of Motorola nor the names of its contributors may
#      be used to endorse or promote products derived from this software without
#      specific prior written permission.
# 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# Changelog:
# Date               Author           Comment
# -----------------------------------------------------------------------------
# 03/28/2006        Motorola        Initial Creation
# 06/06/2006        Motorola        PictBridge support
#


mtp_if-objs	:= mtp-fd.o mtp-io.o mtp-linux.o
pbg_if-objs	:= mtp-fd.o mtp-io.o pbg-linux.o

obj-$(CONFIG_OTG_MTP) += mtp_if.o
obj-$(CONFIG_OTG_PBG) += pbg_if.o

OTG=$(TOPDIR)/drivers/otg
MTPD=$(OTG)/functions/mtp
OTGCORE_DIR=$(OTG)/otgcore
EXTRA_CFLAGS += -I$(MTPD) -I$(OTG) -Wno-unused -Wno-format -I$(OTGCORE_DIR)
EXTRA_CFLAGS_nostdinc += -I$(MTPD) -I$(OTG) -Wno-unused -Wno-format -I$(OTGCORE_DIR)
