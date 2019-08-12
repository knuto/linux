# Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: GPL-2.0
#
# Kernel module implementing a few simple examples of KTF tests
#

include $(srctree)/$(src)/../scripts/ktf_syms.mk

ccflags-y += -I$(srctree)/$(src)/../kernel -I$(src)

obj-m := hello.o h2.o h3.o h4.o

ifdef CONFIG_KGDB
obj-m += kgdemo.o
endif

