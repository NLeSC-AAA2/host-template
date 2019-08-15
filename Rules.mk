.PRECIOUS: $(DEVICE_KERNEL_DIR)/%.aoco

$(BUILD_DIR)/%.o: $(SRCDIR)/%.cc | $(BUILD_DIR)/
	$(PRINTF) " CC\t$(notdir $<)\n"
	$(AT)$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(EMULATOR_KERNEL_DIR)/%.aoco: $(SRCDIR)/%.cl | $(EMULATOR_KERNEL_DIR)
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -c -march=emulator -emulator-channel-depth-model=strict -board=p385a_min_ax115 \
	    -o $@ $^ $(if $(AT),2>/dev/null >/dev/null,)

$(DEVICE_KERNEL_DIR)/%.aoco: $(SRCDIR)/%.cl | $(DEVICE_KERNEL_DIR)
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -c -board=p385a_min_ax115 -o $@ $^ \
	    $(if $(AT),2>/dev/null >/dev/null,)

$(DEVICE_KERNEL_DIR)/%.aocr: $(DEVICE_KERNEL_DIR)/%.aoco
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -report -profile=all -board=p385a_min_ax115 \
	    $^ -rtl -o $@ $(if $(AT),2>/dev/null >/dev/null,)

$(EMULATOR_KERNEL_DIR)/%.aocr: $(EMULATOR_KERNEL_DIR)/%.aoco
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)$(AOC) $(AOCFLAGS) -march=emulator -emulator-channel-depth-model=strict -board=p385a_min_ax115 $^ -rtl \
	    -o $@ $(if $(AT),2>/dev/null >/dev/null,)

$(DEVICE_KERNEL_DIR)/%.aocx: $(DEVICE_KERNEL_DIR)/%.aocr
	$(PRINTF) " AOC\t$(^F)\n"
	$(AT)cd $(DEVICE_KERNEL_DIR) && $(AOC) $(AOCFLAGS) $^ \
	    $(if $(AT),2>/dev/null >/dev/null,)

