$(BUILD_DIR)/:
	$(PRINTF) " MKDIR\t$(@)\n"
	$(AT)mkdir -p $@

AOCFLAGS:=-I${ALTERAOCLSDKROOT}/include/kernel_headers
AOC:=aoc

$(BUILD_DIR)/%.o: $(SRCDIR)/%.cc | $(BUILD_DIR)/
	$(PRINTF) " CC\t$(@F)\n"
	$(AT)$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.emulator.aocx: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(@F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -march=emulator $^ -o $@

$(BUILD_DIR)/%.aocx: $(SRCDIR)/%.cl
	$(PRINTF) " AOC\t$(@F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -rtl -board=p385a_min_ax115 $^ -o $@
