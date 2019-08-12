
GTEST_CFLAGS ?= -I$(GTEST_PATH)/include -DGTEST_HAS_PTHREAD=1 -lpthread
GTEST_LIBS ?= -L$(GTEST_PATH)/lib64 -lgtest -lpthread
NETLINK_CFLAGS ?= $(shell pkgconf --cflags libnl-genl-3.0)
HOST_EXTRACFLAGS = -I$(srctree)/$(src)/../lib $(NETLINK_CFLAGS) $(GTEST_CFLAGS) \
		-Wall -Werror \
		-Wno-packed-bitfield-compat -D_GNU_SOURCE

HOST_EXTRACXXFLAGS = -I$(srctree)/$(src)/../lib $(NETLINK_CFLAGS) $(GTEST_CFLAGS) \
		-Wall \
		-Wno-packed-bitfield-compat \
		-Wno-pointer-arith -Werror \
		-D__FILENAME__=\"`basename $<`\"
NETLINK_LIBS ?= $(shell pkgconf --libs libnl-genl-3.0)
KBUILD_HOSTLDLIBS =	-L$(obj)/../lib -lktf $(NETLINK_LIBS) $(GTEST_LIBS)

hostprogs-y := ktfrun ktfcov ktftest

__build: $(addprefix $(obj)/,$(hostprogs-y))

## Simple kernel test runner sample program:
ktfrun-cxxobjs = ktfrun.o
ktfcov-cxxobjs = ktfcov.o

## Configure and run the KTF selftests:
ktftest-cxxobjs = ktftest.o hybrid.o
