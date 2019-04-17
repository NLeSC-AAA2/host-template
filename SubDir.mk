SRCDIR:=$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
SUBDIR:=$(notdir $(SRCDIR))
BUILD_DIR:=$(BUILD_DIR)/$(SUBDIR)

include Rules.mk
-include $(BUILD_DIR)/*.d

include $(SRCDIR)/Targets.mk

-include $(SRCDIR)/*/Makefile
BUILD_DIR:=$(patsubst %/,%,$(dir $(BUILD_DIR)))
