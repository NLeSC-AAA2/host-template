#pragma once
#include <CL/cl.hpp>
#include <vector>

extern void createContext(cl::Context &context, std::vector<cl::Device> &devices);

