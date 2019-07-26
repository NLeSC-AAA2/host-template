ROOTDIR?=$(abspath $(CURDIR))
.POSIX:
.SUFFIXES:
.PHONY: all clean build test device device-emulator

BUILD_ROOT:=$(ROOTDIR)/build
LIB_DIR:=$(BUILD_ROOT)/lib
EXE_DIR:=$(BUILD_ROOT)
KERNEL_DIR:=$(BUILD_ROOT)/kernels

ifdef ALTERAOCLSDKROOT
SDKROOT:=${ALTERAOCLSDKROOT}
else
SDKROOT:=${INTELOCLSDKROOT}
endif

CXX:=g++
CXXFLAGS:=-std=c++14 -Wall -pedantic -O3 -I$(ROOTDIR)/vendor \
    -iquote $(ROOTDIR)/include -isystem$(SDKROOT)/host/include/
AOC:=aoc
AOCFLAGS:=-I${SDKROOT}/include/kernel_headers
LD:=g++

all: build report

device:

report:

emulator:

define make-dir =
$(PRINTF) " MKDIR\t$(patsubst $(ROOTDIR)/%/,%,$(@))\n"
$(AT)mkdir -p $@
endef

$(LIB_DIR)/ $(BUILD_ROOT)/ $(KERNEL_DIR): ; $(make-dir)

build: $(EXE_DIR)/host-template $(EXE_DIR)/run-tests

test: $(EXE_DIR)/run-tests
	@$(EXE_DIR)/run-tests

clean:
	-rm -rf $(BUILD_ROOT)

$(LIB_DIR)/%.a: | $(LIB_DIR)/
	$(PRINTF) " AR\t$(patsubst $(LIB_DIR)/%,%,$@)\n"
	$(AT)$(AR) rcs $@ $^

V = 0
AT_0 := @
AT_1 :=
AT = $(AT_$(V))

ifeq ($(V), 1)
    PRINTF := @\#
else
    PRINTF := @printf
endif

include $(ROOTDIR)/src/Makefile $(ROOTDIR)/test/Makefile
