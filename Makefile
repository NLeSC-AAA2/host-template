build_dir = ./build

fmt_cflags = -DFMT_HEADER_ONLY
aocl_cflags = $(shell aocl compile-config)
aocl_lflags = $(shell aocl link-config)

compile_flags = -std=c++14 -Wall -O3 -Iinclude -Isrc $(fmt_cflags) $(aocl_cflags)
compile = g++

link_flags = -lstdc++fs $(aocl_lflags)
link = g++

cc_files = $(shell find ./src -name *.cc)
obj_files = $(cc_files:%.cc=$(build_dir)/%.o)
dep_files = $(obj_files:%.o=%.d)

.PHONY: clean build

build: $(build_dir)/host-template

-include $(dep_files)

$(build_dir)/%.o : %.cc
	@mkdir -p $(@D)
	$(compile) $(compile_flags) -MMD -c $< -o $@

$(build_dir)/host-template : $(obj_files)
	@mkdir -p $(@D)
	$(link) $^ $(link_flags) -o $@

clean:
	-rm -rf $(build_dir)

