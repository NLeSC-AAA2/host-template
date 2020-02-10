cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))
clfiles:=$(wildcard $(SRCDIR)/*.cl)

device_targets:=$(patsubst $(SRCDIR)/%.cl,$(DEVICE_KERNEL_DIR)/%.aocx,$(clfiles))
report_targets:=$(patsubst $(SRCDIR)/%.cl,$(DEVICE_KERNEL_DIR)/%.aocr,$(clfiles))
emulator_targets:=$(patsubst $(SRCDIR)/%.cl,$(EMULATOR_KERNEL_DIR)/%.aocr,$(clfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config) \
    $(shell pkg-config --cflags fftw3f)

$(LIB_DIR)/libfft_r2c.a: $(ofiles)

report: $(report_targets)

emulator: $(emulator_targets)

$(TARGET): $(LIB_DIR)/libfft_r2c.a 
