$(BUILD_DIR)/%.o: $(SRCDIR)/%.cc | $(BUILD_DIR)/
	$(PRINTF) " CC\t$(^F)\n"
	$(AT)$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(KERNEL_DIR)/%.aocx: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -march=emulator $^ -o $@

$(KERNEL_DIR)/%.aocr: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -rtl -board=p385a_min_ax115 $^ -o $@
