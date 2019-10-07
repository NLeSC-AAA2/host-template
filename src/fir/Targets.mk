cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)

$(LIB_DIR)/libfir.a: $(ofiles)

device: $(DEVICE_KERNEL_DIR)/FIR_filter.aocx

report: $(DEVICE_KERNEL_DIR)/FIR_filter.aocr

emulator: $(EMULATOR_KERNEL_DIR)/FIR_filter.aocr

$(TARGET): $(LIB_DIR)/libfir.a $(DEVICE_KERNEL_DIR)/FIR_filter.aocr \
    $(EMULATOR_KERNEL_DIR)/FIR_filter.aocr
