cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)

$(LIB_DIR)/libfir.a: $(ofiles)

$(BUILD_DIR)/FIR-1024x16-host.hh: $(SRCDIR)/Coeffs16384Kaiser-quant.dat \
    $(SRCDIR)/gen_fir.py
	$(PRINTF) " FIR\tweights\n"
	$(AT)$(<D)/gen_fir.py host --taps 16 --channels 1024 $< >$@

$(BUILD_DIR)/FIR-1024x16-channel.h: $(SRCDIR)/Coeffs16384Kaiser-quant.dat \
    $(SRCDIR)/gen_fir.py
	$(PRINTF) " FIR\tweights\n"
	$(AT)$(<D)/gen_fir.py fpga --taps 16 --channels 1024 --vector-size 16 $< >$@

all: $(LIB_DIR)/libfir.a

device: $(DEVICE_KERNEL_DIR)/FIR_filter.aocx

report: $(DEVICE_KERNEL_DIR)/FIR_filter.aocr

emulator: $(EMULATOR_KERNEL_DIR)/FIR_filter.aocr
