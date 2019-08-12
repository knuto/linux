# Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: GPL-2.0
#
# Kernel module implementing a test suite for testing KTF itself
#

ccflags-y += -Wno-vla

ccflags-y += -I$(srctree)/$(src)/../kernel -I$(src)

obj-m := selftest.o

include $(srctree)/$(src)/../scripts/ktf_syms.mk

selftest-y := self.o hybrid.o context.o

