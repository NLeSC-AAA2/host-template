.POSIX:
.SUFFIXES:
.PHONY: clean build test device device-emulator
.DEFAULT: all

BUILD_DIR:=./build
LIB_DIR:=$(BUILD_DIR)/lib
EXE_DIR:=./build
LD:=g++
CXX:=g++
CXXFLAGS:=-std=c++14 -Wall -pedantic -O3 -Ivendor -iquote include

all: build device-emulator

device:

device-emulator:

$(LIB_DIR)/:
	$(PRINTF) " MKDIR\t$(@)\n"
	$(AT)mkdir -p $@

build:

test:

clean: BUILD_DIR:=$(BUILD_DIR)
clean:
	-rm -rf $(BUILD_DIR)

%.a: | $(LIB_DIR)/
	$(PRINTF) " AR\t$(@F)\n"
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

include src/Makefile test/Makefile
