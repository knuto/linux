/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
 *    Author: Alan Maguire <alan.maguire@oracle.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 * ktf_override.c: support for overriding function entry.
 */
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include "ktf.h"
#include "ktf_override.h"

asmlinkage void ktf_just_return_func(void);

asm(
	".type ktf_just_return_func, @function\n"
	".globl ktf_just_return_func\n"
	"ktf_just_return_func:\n"
	"	ret\n"
	".size ktf_just_return_func, .-ktf_just_return_func\n"
);

void ktf_post_handler(struct kprobe *kp, struct pt_regs *regs,
		      unsigned long flags)
{
	/*
	 * A dummy post handler is required to prohibit optimizing, because
	 * jump optimization does not support execution path overriding.
	 */
}
EXPORT_SYMBOL(ktf_post_handler);

void ktf_override_function_with_return(struct pt_regs *regs)
{
	KTF_SET_INSTRUCTION_POINTER(regs, (unsigned long)&ktf_just_return_func);
}
EXPORT_SYMBOL(ktf_override_function_with_return);
NOKPROBE_SYMBOL(ktf_override_function_with_return);

int ktf_register_override(struct kprobe *kp)
{
	return register_kprobe(kp);
}
EXPORT_SYMBOL(ktf_register_override);
