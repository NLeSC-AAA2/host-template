# Make a symlink to this file from any subdir with source files

MAKEFILE:=$(lastword $(MAKEFILE_LIST))
SRCDIR:=$(patsubst %/,%,$(dir $(abspath $(MAKEFILE))))
TARGET:=$(abspath $(MAKEFILE))-targets
.PHONY: $(TARGET)

# Prevent infinite recursion
INCLUDE_FROM_ROOT:=false
ifeq ($(abspath $(CURDIR)), $(abspath $(SRCDIR)))
    ifneq ($(.DEFAULT_GOAL), $(TARGET))
         INCLUDE_FROM_ROOT:=true
    endif
endif

SUBDIR:=$(patsubst $(ROOTDIR)/%,%,$(SRCDIR))
BUILD_DIR:=$(BUILD_ROOT)/$(SUBDIR)

ifeq ($(INCLUDE_FROM_ROOT),true)
    ROOTDIR:=$(patsubst %/,%,$(dir $(realpath $(MAKEFILE))))
    .DEFAULT_GOAL:=$(TARGET)
    include $(ROOTDIR)/Makefile
else
    $(BUILD_DIR)/: ; $(make-dir)

    include $(ROOTDIR)/Rules.mk

    include $(SRCDIR)/Targets.mk

    -include $(BUILD_DIR)/*.d $(SRCDIR)/*/Makefile
endif
