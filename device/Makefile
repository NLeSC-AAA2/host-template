include_flags = -I${ALTERAOCLSDKROOT}/include/kernel_headers
emulator_flags = -march=emulator
compile = aoc $(emulator_flags) $(include_flags)

.PHONY: build

build : fft.aocx

%.aocx : %.cl
	$(compile) $^
