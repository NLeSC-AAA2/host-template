.POSIX:
.SUFFIXES:
.PHONY: clean build test device device-emulator
.DEFAULT: all

BUILD_ROOT:=$(ROOTDIR)/build
BUILD_DIR:=$(BUILD_ROOT)
LIB_DIR:=$(BUILD_ROOT)/lib
EXE_DIR:=$(BUILD_ROOT)
LD:=g++
CXX:=g++
CXXFLAGS:=-std=c++14 -Wall -pedantic -O3 -I$(ROOTDIR)/vendor -iquote $(ROOTDIR)/include

all: build device-emulator

device:

device-emulator:

define make-dir =
$(PRINTF) " MKDIR\t$(patsubst $(ROOTDIR)/%/,%,$(@))\n"
$(AT)mkdir -p $@
endef

$(LIB_DIR)/: ; $(make-dir)

build:

test:

clean: BUILD_DIR:=$(BUILD_DIR)
clean:
	-rm -rf $(BUILD_DIR)

$(LIB_DIR)/%.a: | $(LIB_DIR)/
	$(PRINTF) " AR\t$@\n"
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
