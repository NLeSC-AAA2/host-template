cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)

$(LIB_DIR)/libether.a: $(ofiles)

device: $(KERNEL_DIR)/ether.aocx

report: $(KERNEL_DIR)/ether.aocr

emulator: $(KERNEL_DIR)/ether.emulator.aocr

$(TARGET): $(LIB_DIR)/libether.a $(KERNEL_DIR)/ether.aocr
