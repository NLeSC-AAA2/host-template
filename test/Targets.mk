test_cc_files = $(SRCDIR)/gtest/src/gtest-all.cc $(SRCDIR)/gmock/src/gmock-all.cc $(SRCDIR)/gtest/src/gtest_main.cc
test_cc_files += $(wildcard $(SRCDIR)/*.cc)
test_obj_files = $(test_cc_files:$(SRCDIR)/%.cc=$(BUILD_DIR)/%.o)

$(test_obj_files): CXXFLAGS+=-iquote $(SRCDIR)/gtest/ -iquote $(SRCDIR)/gmock/
$(test_obj_files): | $(BUILD_DIR)/gtest/src/ $(BUILD_DIR)/gmock/src/

$(BUILD_DIR)/gtest/src/ $(BUILD_DIR)/gmock/src/:
	$(PRINTF) " MKDIR\t$(@)\n"
	$(AT)mkdir -p $@

$(EXE_DIR)/run-tests: LDFLAGS+=-lpthread
$(EXE_DIR)/run-tests: $(test_obj_files)
	$(PRINTF) " LD\t$(@F)\n"
	$(AT)$(LD) $^ $(LDFLAGS) -o $@

build: $(EXE_DIR)/run-tests

test: $(EXE_DIR)/run-tests
	@$(EXE_DIR)/run-tests
