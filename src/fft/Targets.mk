cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config) \
    -isystem /var/scratch/package/altera_pro/18.1.0.222/hld/host/include \
    $(shell pkg-config --cflags fftw3f)

$(LIB_DIR)/libfft.a: $(ofiles)

device: $(KERNEL_DIR)/fft.aocr

device-emulator: $(KERNEL_DIR)/fft.aocx

$(TARGET): $(LIB_DIR)/libfft.a $(KERNEL_DIR)/fft.aocx
