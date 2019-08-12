# Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: GPL-2.0
#
# Implementation of the kernel part of Kernel Test Framework (KTF),
# a framework for running unit test like tests within the kernel.
#

obj-m := ktf.o

include $(srctree)/$(src)/../scripts/ktf_syms.mk

ktf-y := ktf_context.o ktf_nl.o ktf_map.o ktf_test.o ktf_debugfs.o ktf_cov.o \
	 ktf_override.o ktf_netctx.o

