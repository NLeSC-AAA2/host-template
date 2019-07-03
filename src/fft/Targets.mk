cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config) \
    $(shell pkg-config --cflags fftw3f)

$(LIB_DIR)/libfft.a: $(ofiles)

device: $(KERNEL_DIR)/fft.aocx

device-emulator: $(KERNEL_DIR)/fft.aocr

$(TARGET): $(LIB_DIR)/libfft.a $(KERNEL_DIR)/fft.aocr
