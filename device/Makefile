include_flags = -I${ALTERAOCLSDKROOT}/include/kernel_headers
emulator_flags = -march=emulator -board=p385a_min_ax115
compile = aoc $(emulator_flags) $(include_flags)

.PHONY: build 

build : fft.aocx

%.aocx : %.cl
	$(compile) $^

