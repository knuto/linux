
GTEST_CFLAGS ?= -I$(GTEST_PATH)/include -DGTEST_HAS_PTHREAD=1 -lpthread
GTEST_LIBS ?= -L$(GTEST_PATH)/lib64 -lgtest -lpthread
NETLINK_CFLAGS ?= $(shell pkgconf --cflags libnl-genl-3.0)
HOST_EXTRACFLAGS = -I$(srctree)/$(src)/.. $(NETLINK_CFLAGS) $(GTEST_CFLAGS) \
		-Wall -Werror -Wno-packed-bitfield-compat -D_GNU_SOURCE
HOST_EXTRACXXFLAGS = -I$(srctree)/$(src)/.. $(NETLINK_CFLAGS) $(GTEST_CFLAGS) \
		-Wall \
		-Wno-packed-bitfield-compat \
		-Wno-pointer-arith -Werror \
		-D__FILENAME__=\"`basename $<`\"

hostcxxlibs-y := libktf.so
libktf-cshobjs = ktf_unlproto.o
libktf-cxxshobjs = ktf_int.o ktf_run.o ktf_debug.o

targets := $(addprefix $(obj)/,$(libktf-cshobjs)) \
	   $(addprefix $(obj)/,$(libktf-cxxshobjs)) \
	   $(addprefix $(obj)/,$(hostcxxlibs-y))

__build: $(targets)
