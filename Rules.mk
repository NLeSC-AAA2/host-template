$(BUILD_DIR)/%.o: $(SRCDIR)/%.cc | $(BUILD_DIR)/
	$(PRINTF) " CC\t$(notdir $<)\n"
	$(AT)$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(KERNEL_DIR)/%.aocx: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -march=emulator $^ -o $@ \
	    $(if $(AT),2>/dev/null >/dev/null,)

$(KERNEL_DIR)/%.aocr: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -rtl -board=p385a_min_ax115 $^ -o $@ \
	    $(if $(AT),2>/dev/null >/dev/null,)
