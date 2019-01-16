#include <iostream>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "common.h"

using namespace std;

ostream &os = clog;

void init(cl::Context &context, vector<cl::Device> &devices)
{
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // The selected device
    int i = 0;
    char *opencl_device_cstr = getenv("OPENCL_DEVICE");
    int opencl_device = opencl_device_cstr ? atoi(opencl_device_cstr) : 0;

    os << ">>> OpenCL environment: " << endl;

    // Iterate all platforms
    for (cl::Platform &platform : platforms) {
        print_platform(platform);

        // Get devices for the current platform
        vector<cl::Device> devices_;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices_);

        // Iterate all devices
        for (cl::Device &device : devices_) {
            bool selected;
            if ((selected = (i == opencl_device))) {
                devices.push_back(device);
            }
            print_device(device, selected);
            i++;
        }
    }
    os << endl;

    if (devices.size() == 0) {
        cerr << "Could not get device "  << opencl_device << endl;
        exit(EXIT_FAILURE);
    }

    context = cl::Context(devices);
}

void print_platform(cl::Platform &platform)
{
    os << ">>> Platform: " << endl;
    os << "Name       : " << platform.getInfo<CL_PLATFORM_NAME>() << endl;
    os << "Version    : " << platform.getInfo<CL_PLATFORM_VERSION>() << endl;
    os << "Extensions : " << platform.getInfo<CL_PLATFORM_EXTENSIONS>() << endl;
    os <<  endl;
}

void print_device(cl::Device &device, bool marker)
{
    os << ">>> Device: ";
    if (marker) os << " (selected)";
    os << endl;
    os << "Name            : " << device.getInfo<CL_DEVICE_NAME>() << endl;
    os << "Driver version  : " << device.getInfo<CL_DRIVER_VERSION>() << endl;
    os << "Device version  : " << device.getInfo<CL_DEVICE_VERSION>() << endl;
    os << "Compute units   : " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
    os << "Clock frequency : " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << " MHz" << endl;
    os << "Global memory   : " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() * 1e-9 << " Gb" << endl;
    os << "Local memory    : " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() * 1e-6 << " Mb" << endl;
    os << endl;
}

cl::Program get_program(
    cl::Context& context,
    cl::Device& device,
    string& filename)
{
    os << ">>> Loading program from binary: " << filename << endl;
    try {
        ifstream ifs(filename, ios::in | ios::binary);
        string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        cl::Program::Binaries binaries(1, std::make_pair(str.c_str(), str.length()));
        vector<cl::Device> devices;
        devices.push_back(device);
        os << endl;
        return cl::Program(context, devices, binaries);
    } catch (cl::Error& error) {
        cerr << "Loading binary failed: " << error.what() << endl;
        exit(EXIT_FAILURE);
    }
}

cl::Kernel get_kernel(
    cl::Program& program,
    string& name)
{
    os << ">>> Loading kernel: " << name << endl;
    try {
        os << endl;
        return cl::Kernel(program, name.c_str());
    } catch (cl::Error& error) {
        cerr << "Loading kernel failed: " << error.what() << endl;
        exit(EXIT_FAILURE);
    }
}

double compute_runtime(
    cl::Event& start,
    cl::Event& end)
{
    double runtime = 0;
    runtime -= start.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    runtime +=   end.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    return runtime * 1e-9;
}
