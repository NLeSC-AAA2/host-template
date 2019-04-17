MAKEFILE:=$(lastword $(MAKEFILE_LIST))
SRCDIR:=$(patsubst %/,%,$(abspath $(dir $(MAKEFILE))))
SUBDIR:=$(notdir $(SRCDIR))
BUILD_DIR:=$(BUILD_DIR)/$(SUBDIR)

ifeq ($(abspath $(CURDIR)), $(abspath $(SRCDIR)))
ROOTDIR:=$(patsubst %/,%,$(dir $(realpath $(MAKEFILE))))
include $(ROOTDIR)/Common.mk
endif

include $(ROOTDIR)/Rules.mk
-include $(BUILD_DIR)/*.d

include $(SRCDIR)/Targets.mk

-include $(SRCDIR)/*/Makefile
BUILD_DIR:=$(patsubst %/,%,$(dir $(BUILD_DIR)))
