.PHONY: all clean build test device emulator

all: $(EXE_DIR)/host-template $(EXE_DIR)/run-tests

device:

report:

emulator:

test: $(EXE_DIR)/run-tests
	$(AT)$(EXE_DIR)/run-tests
