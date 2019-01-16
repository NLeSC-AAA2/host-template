#pragma once
#include <CL/cl.hpp>
#include <vector>
#include <string>

extern cl::Program createProgramFromBinaries(
        cl::Context &context,
        std::vector<cl::Device> &devices,
        const std::string &name);

