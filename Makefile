# Enable POSIX compliance mode
.POSIX:
# Disable built-in suffix rules
.SUFFIXES:
# Stop make from trying to delete directories
.PRECIOUS: %/

###############################################################################
# Toggle-able silencing of command output                                     #
#                                                                             #
# The silencing is controlled by the $(V) variable. If V is 0 (the default)   #
# commands prefixed by $(AT) are silenced and $(PRINTF) reports it output.    #
#                                                                             #
# When V is 1, for example when make is invoked with "make V=1" the silencing #
# output of $(PRINTF) is surpressed and the command silencing of $(AT) is     #
# disabled.                                                                   #
###############################################################################

V = 0
AT_0 := @
AT_1 :=
AT = $(AT_$(V))

ifeq ($(V), 1)
    PRINTF := @\#
else
    PRINTF := @printf
endif

###############################################################################
# Recipe for creating directories                                             #
#                                                                             #
# Can be used in rules for directory creation.                                #
###############################################################################

define make-dir
	$(PRINTF) " MKDIR\t$(patsubst $(ROOTDIR)/%/,%,$(@))\n"
	$(AT)mkdir -p $@
endef

%/: ; $(make-dir)

###############################################################################
# Global variables for parts of the source and build tree                     #
#                                                                             #
# These variables check where in the source tree make is being called, where  #
# the root directory is and defines variables pointing at relevant            #
# subdirectories.                                                             #
###############################################################################

# Disable builtin rules and variables in GNU Make 4.0 and later
MAKEFLAGS+= --no-builtin-rules --no-builtin-variables
# Store the location of this Makefile
MAKEFILE:=$(lastword $(MAKEFILE_LIST))
# Realpath resolves symlinks to their realpath, letting us find the root
# directory of the project. This rootdir is then stored.
ROOTDIR:=$(patsubst %/,%,$(dir $(realpath $(MAKEFILE))))
# Define a build root so we can build "out of tree"
BUILD_ROOT:=$(ROOTDIR)/build
# Store the directory from where make was invoked
MAKEDIR:=$(patsubst %/,%,$(dir $(abspath $(MAKEFILE))))

# Default to building all for the subdirectory if no targets is specified
.DEFAULT_GOAL:=$(MAKEDIR)-all
.PHONY: $(MAKEDIR)-%

###############################################################################
# Determine all the target subdirectories                                     #
#                                                                             #
# We use find to find all Targets.mk under the root directory.                #
#                                                                             #
# We then make a list of corresponding Build.mk files under the BUILD_ROOT,   #
# the rule for creating these Build.mk files is defined below.                #
###############################################################################

TARGETS:=$(foreach makefile, $(shell find $(ROOTDIR) -name Targets.mk),\
    $(patsubst $(ROOTDIR)%/Targets.mk,$(BUILD_ROOT)%/Build.mk, $(makefile)))

###############################################################################
# Rule for building subdirectory makefiles                                    #
#                                                                             #
# The targets in each subdirectory need to know the correct SRCDIR and        #
# BUILD_DIR to use for their rules. Instead of using complicated recursive    #
# include tricks, we simply build a separate Makefile for each Targets.mk     #
# makefile. This makefile consists of:                                        #
#                                                                             #
#   - The correct SRCDIR and BUILD_DIR definitions for that Makefile          #
#   - The common rules defined in Common.mk                                   #
#   - All the rules defined in Targets.mk                                     #
#                                                                             #
# All the local targets should start with $(BUILD_ROOT) or some other         #
# variable, so we consider all other targets as "global" targets, such as     #
# 'all'. We do additional processing with sed to rename these targets to a    #
# subdirectory specific target and add a conditional to activate that rule if #
# make was invoked from this directory.                                       #
#                                                                             #
# This rule depends on the specific Targets.mk, Common.mk, and the main       #
# Makefile, triggering a rebuild of the Makefile when any of these change.    #
###############################################################################
$(TARGETS): $(BUILD_ROOT)%/Build.mk: $(ROOTDIR)%/Targets.mk \
    $(ROOTDIR)/Common.mk $(ROOTDIR)/Makefile | $(BUILD_ROOT)%/
	$(PRINTF) " CONFIG\t$(patsubst $(ROOTDIR)/%,%,$@)\n"
	$(AT)printf "SRCDIR:=$(ROOTDIR)$*\nBUILD_DIR:=$(BUILD_ROOT)$*\n\n" >$@
	$(AT)cat $(ROOTDIR)/Common.mk >>$@
	$(AT)sed 's#^\([a-z]\+\): #ifeq ($$(MAKEDIR), $$(SRCDIR))\n\1: $(ROOTDIR)$*-\1\nendif\n$(ROOTDIR)$*-\1: #' $< >>$@

###############################################################################
# Set global variables                                                        #
#                                                                             #
# All global variables that will be used in 'immediate' contexts (i.e.,       #
# targets and dependencies) should go here before all other makefiles are     #
# included.                                                                   #
###############################################################################

LIB_DIR:=$(BUILD_ROOT)/lib
EXE_DIR:=$(BUILD_ROOT)
EMULATOR_KERNEL_DIR:=$(BUILD_ROOT)/kernels/emulator
DEVICE_KERNEL_DIR:=$(BUILD_ROOT)/kernels/device
UNIBOARD_KERNEL_DIR:=$(BUILD_ROOT)/kernels/uniboard

###############################################################################
# Include subdirectory makefiles                                              #
#                                                                             #
# Include all the subdirectory makefiles, all the generated dependency files  #
# under BUILD_ROOT, and set the SRCDIR and BUILD_DIR for rules from the root  #
# makefile.                                                                   #
###############################################################################
include $(TARGETS)
-include $(shell find $(BUILD_ROOT) -name *.d 2>/dev/null)
SRCDIR:=$(patsubst %/,%,$(dir $(abspath $(MAKEFILE))))
BUILD_DIR:=$(patsubst $(ROOTDIR)/%,$(BUILD_ROOT)/%,$(SRCDIR))

###############################################################################
# Rules and variables for root Makefile go here                               #
###############################################################################

ifdef ALTERAOCLSDKROOT
SDKROOT:=${ALTERAOCLSDKROOT}
else
SDKROOT:=${INTELOCLSDKROOT}
endif

AOC:=aoc
AOCFLAGS:=-I${SDKROOT}/include/kernel_headers
AR=ar
CXX:=g++
LD:=g++

CXXFLAGS:=-std=c++14 -Wall -pedantic -O3 -I$(ROOTDIR)/vendor \
    -iquote $(ROOTDIR)/include -isystem $(SDKROOT)/host/include \
    -Wno-ignored-attributes -g

clean:
	$(PRINTF) " CLEAN\n"
	$(AT)-rm -rf $(BUILD_ROOT)

$(LIB_DIR)/%.a: | $(LIB_DIR)/
	$(PRINTF) " AR\t$(patsubst $(LIB_DIR)/%,%,$@)\n"
	$(AT)$(AR) rcs $@ $^
