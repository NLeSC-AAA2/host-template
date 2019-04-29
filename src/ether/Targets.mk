cfiles:=$(wildcard $(SRCDIR)/*.cc)
ofiles:=$(patsubst $(SRCDIR)/%.cc,$(BUILD_DIR)/%.o,$(cfiles))

$(BUILD_DIR)/%.o: CXXFLAGS+=$(shell aocl compile-config) \
    -isystem /var/scratch/package/altera_pro/18.1.0.222/hld/host/include \

$(LIB_DIR)/libether.a: $(ofiles)

device: $(KERNEL_DIR)/ether.aocr

device-emulator: $(KERNEL_DIR)/ether.aocx

$(TARGET): $(LIB_DIR)/libether.a $(KERNEL_DIR)/ether.aocx
