cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config) \
    $(shell pkg-config --cflags fftw3f)

$(LIB_DIR)/libfft.a: $(ofiles)

device: $(DEVICE_KERNEL_DIR)/fft.aocx

report: $(DEVICE_KERNEL_DIR)/fft.aocr

emulator: $(EMULATOR_KERNEL_DIR)/fft.aocr

$(TARGET): $(LIB_DIR)/libfft.a $(DEVICE_KERNEL_DIR)/fft.aocr \
    $(EMULATOR_KERNEL_DIR)/fft.aocr
