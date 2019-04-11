aoc_include_flags = -I${ALTERAOCLSDKROOT}/include/kernel_headers
aoc = aoc $(aoc_include_flags)

device-emulator: $(build_dir)/fft.emulator.aocx

device: $(build_dir)/fft.aocx

$(build_dir)/%.emulator.aocx : %.cl
	$(PRINTF) " AOC\t$(@F)\n"
	$(AT)$(aoc) -march=emulator $^ -o $@

$(build_dir)/%.aocx : %.cl
	$(PRINTF) " AOC\t$(@F)\n"
	$(AT)$(aoc) -rtl -board=p385a_min_ax115 $^ -o $@
