#pragma once
#include <vector>
#include <string>

#include "common.hh"

extern cl::Program createProgramFromBinaries(
        cl::Context &context,
        std::vector<cl::Device> &devices,
        const std::string &name);

