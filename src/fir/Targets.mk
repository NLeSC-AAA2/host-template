cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

clfiles:=$(wildcard $(SRCDIR)/*.cl)
aocofiles:=$(patsubst $(SRCDIR)/%.cl,$(DEVICE_KERNEL_DIR)/%.aoco,$(clfiles)) \
    $(patsubst $(SRCDIR)/%.cl,$(EMULATOR_KERNEL_DIR)/%.aoco,$(clfiles))

devicefiles:=$(patsubst $(SRCDIR)/%.cl,$(DEVICE_KERNEL_DIR)/%.aocx,$(clfiles))
reportfiles:=$(patsubst $(SRCDIR)/%.cl,$(DEVICE_KERNEL_DIR)/%.aocr,$(clfiles))
emulatorfiles:=$(patsubst $(SRCDIR)/%.cl,$(EMULATOR_KERNEL_DIR)/%.aocr,$(clfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config)
$(ofiles): $(BUILD_DIR)/FIR-1024x16-host.hh

$(LIB_DIR)/libfir.a: $(ofiles)

$(aocofiles): AOCFLAGS+=-I$(BUILD_DIR)/

$(DEVICE_KERNEL_DIR)/FIR_channel_vectorised.aoco \
    $(EMULATOR_KERNEL_DIR)/FIR_channel_vectorised.aoco: \
        $(BUILD_DIR)/FIR-1024x16-channel.h

$(BUILD_DIR)/FIR-1024x16-host.hh: $(SRCDIR)/Coeffs16384Kaiser-quant.dat \
    $(SRCDIR)/gen_fir.py
	$(PRINTF) " FIR\tweights\n"
	$(AT)$(<D)/gen_fir.py host --taps 16 --channels 1024 $< >$@

$(BUILD_DIR)/FIR-1024x16-channel.h: $(SRCDIR)/Coeffs16384Kaiser-quant.dat \
    $(SRCDIR)/gen_fir.py
	$(PRINTF) " FIR\tweights\n"
	$(AT)$(<D)/gen_fir.py fpga --taps 16 --channels 1024 --vector-size 16 $< >$@

all: $(LIB_DIR)/libfir.a

device: $(devicefiles)

report: $(reportfiles)

emulator: $(emulatorfiles)
