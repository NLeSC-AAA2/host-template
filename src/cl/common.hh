#pragma once
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <vector>
#include <string>
#include <utility>
#include <iostream>

void init(
    cl::Context &context,
    std::vector<cl::Device> &devices);

void print_platform(
    cl::Platform &platform);

void print_devices();

void print_device(
    cl::Device &device,
    bool marker = false);

void print_program(
    cl::Program& program);

std::string get_source(
    std::string& filename);

std::string get_flags();

void write_program(
    cl::Program& program,
    std::string& filename);

cl::Program compile_program(
    cl::Context& context,
    cl::Device& device,
    std::string& source);

void write_source(
    std::string& source,
    std::string& filename);

cl::Program get_program(
    cl::Context& context,
    cl::Device& device,
    std::string const &filename);

cl::Kernel get_kernel(
    cl::Program& program,
    std::string& name);

double compute_runtime(
    cl::Event& start,
    cl::Event& end);

template <unsigned N>
void set_args_n(cl::Kernel &k) {}

template <unsigned N, typename First, typename ...Rest>
void set_args_n(cl::Kernel &k, First &&first, Rest &&...rest)
{
    k.setArg(N, first);
    set_args_n<N+1>(k, std::forward<Rest>(rest)...);
}

template <typename ...Args>
void set_args(cl::Kernel &k, Args &&...args)
{
    set_args_n<0>(k, std::forward<Args>(args)...);
}

namespace TripleA2
{
class Kernel
{
    cl::Kernel kernel;
    cl::CommandQueue queue;
    cl::Event event;

    static cl::Context getContext(const cl::Program& prog);

  public:
    Kernel(const cl::Program& prog, std::string name);

    template<typename... Args>
    void operator()(Args&&... args)
    {
        set_args(kernel, std::forward<Args>(args)...);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange, nullptr, &event);
    }

    void finish();
};
}
