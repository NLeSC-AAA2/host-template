#pragma once
#include <CL/cl.hpp>
#include <vector>
#include <string>

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
    std::string& filename);

cl::Kernel get_kernel(
    cl::Program& program,
    std::string& name);

double compute_runtime(
    cl::Event& start,
    cl::Event& end);
