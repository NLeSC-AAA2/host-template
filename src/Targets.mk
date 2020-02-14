$(BUILD_DIR)/main.o: CXXFLAGS+=$(shell aocl compile-config)

$(EXE_DIR)/host-template: LDFLAGS+=$(shell aocl link-config) -lpthread $(shell pkg-config --libs fftw3f)
$(EXE_DIR)/host-template: $(BUILD_DIR)/main.o $(LIB_DIR)/libfft.a $(LIB_DIR)/libcl.a $(LIB_DIR)/libether.a $(LIB_DIR)/libfir.a $(LIB_DIR)/libfft_r2c.a
	$(PRINTF) " LD\t$(@F)\n"
	$(AT)$(LD) $(LDFLAGS) -o $@ -Wl,--whole-archive $^ -Wl,--no-whole-archive

all: $(EXE_DIR)/host-template
