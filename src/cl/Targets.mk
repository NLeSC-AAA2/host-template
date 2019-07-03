cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)

$(LIB_DIR)/libcl.a: $(ofiles)

$(TARGET): $(LIB_DIR)/libcl.a
