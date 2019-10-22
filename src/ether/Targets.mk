cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)

$(LIB_DIR)/libether.a: $(ofiles)

# device: $(DEVICE_KERNEL_DIR)/ether.aocx

report: $(DEVICE_KERNEL_DIR)/ether.aocr

emulator: $(EMULATOR_KERNEL_DIR)/ether.aocr

$(TARGET): $(LIB_DIR)/libether.a $(DEVICE_KERNEL_DIR)/ether.aocr \
    $(EMULATOR_KERNEL_DIR)/ether.aocr
