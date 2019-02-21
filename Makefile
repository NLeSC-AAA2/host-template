build_dir = ./build

fmt_cflags = -DFMT_HEADER_ONLY
aocl_cflags = $(shell aocl compile-config)
aocl_lflags = $(shell aocl link-config)
aocl_shut_up = -isystem /var/scratch/package/altera_pro/18.1.0.222/hld/host/include

fftw_cflags = $(shell pkg-config --cflags fftw3f)
fftw_lflags = $(shell pkg-config --libs fftw3f)

compile_flags = -std=c++14 -Wall -O3 -Iinclude -iquote src -iquote test/gtest \
		-iquote test/gmock -pthread $(fmt_cflags) $(aocl_cflags) $(fftw_cflags) \
		$(aocl_shut_up)
compile = g++

link_flags = -lstdc++fs $(aocl_lflags) $(fftw_lflags) -pthread
link = g++

cc_files = $(shell find ./src -name *.cc ! -name main.cc)
obj_files = $(cc_files:%.cc=$(build_dir)/%.o)
dep_files = $(obj_files:%.o=%.d)

main_cc_file = ./src/main.cc
main_obj_file = $(build_dir)/src/main.o
dep_files += $(build_dir)/src/main.d

test_cc_files = ./test/gtest/src/gtest-all.cc ./test/gmock/src/gmock-all.cc ./test/gtest/src/gtest_main.cc
test_cc_files += $(shell find test -name '*.cc' ! -path 'test/gtest/*' ! -path 'test/gmock/*')
test_obj_files = $(test_cc_files:%.cc=$(build_dir)/%.o)
test_dep_files = $(test_obj_files:%.o=%.d)

.PHONY: clean build test

build: $(build_dir)/host-template

test: $(build_dir)/run-tests
	$(build_dir)/run-tests

-include $(dep_files)
-include $(test_dep_files)

V = 0
AT_0 := @
AT_1 :=
AT = $(AT_$(V))

ifeq ($(V), 1)
    PRINTF := @\#
else
    PRINTF := @printf
endif

$(build_dir)/%.o : %.cc Makefile
	$(PRINTF) " CC\t$@\n"
	$(AT)mkdir -p $(@D)
	$(AT)$(compile) $(compile_flags) -MMD -c $< -o $@

$(build_dir)/host-template : $(obj_files) $(main_obj_file)
	$(PRINTF) " LD\t$@\n"
	$(AT)mkdir -p $(@D)
	$(AT)$(link) $^ $(link_flags) -o $@

$(build_dir)/run-tests : $(test_obj_files) $(obj_files)
	$(PRINTF) " LD\t$@\n"
	$(AT)mkdir -p $(@D)
	$(AT)$(link) $^ $(link_flags) -o $@

clean:
	-rm -rf $(build_dir)

