#include "cl/cl.hh"
#include <iostream>
#include <string>
#include <complex>
#include <gsl/span>

#include <random>

void randomize_data(gsl::span<std::complex<float>> data)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto &z : data) {
        z.real(dist(gen));
        z.imag(dist(gen));
    }
}

void print_data(gsl::span<std::complex<float>> data)
{
    for (auto const &z : data) {
        std::cout << z.real() << " " << z.imag() << std::endl;
    }
}

void fft_test(std::string const &filename, unsigned fft_size, unsigned repeats)
{
    unsigned data_size = fft_size * repeats,
             byte_size = data_size * sizeof(float) * 2;

    cl::Context context;
    std::vector<cl::Device> devices;
    createContext(context, devices);

    cl::Program program = get_program(context, devices[0], filename);
    cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    cl::Buffer device_input_buf(context, CL_MEM_READ_ONLY, byte_size);
    cl::Buffer device_output_buf(context, CL_MEM_WRITE_ONLY, byte_size);
    cl::Buffer host_input_buf(context,
            CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, byte_size);
    cl::Buffer host_output_buf(context,
            CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, byte_size);

     
    gsl::span<std::complex<float>> input_data(
        reinterpret_cast<std::complex<float>*>(
            queue.enqueueMapBuffer(host_input_buf, CL_TRUE, CL_MAP_WRITE, 0, byte_size)),
        data_size);

    gsl::span<std::complex<float>> output_data(
        reinterpret_cast<std::complex<float>*>(
            queue.enqueueMapBuffer(host_output_buf, CL_TRUE, CL_MAP_READ, 0, byte_size)),
        data_size);
    
    randomize_data(input_data);

    cl::Kernel source_kernel(program, "source"),
               sink_kernel(program, "sink");
    set_args(source_kernel, device_input_buf, data_size);
    set_args(sink_kernel, device_output_buf, data_size);

    cl::Event source_event, sink_event;
    queue.enqueueCopyBuffer(host_input_buf, device_input_buf, 0, 0, byte_size);
    queue.enqueueTask(source_kernel, 0, &source_event);
    queue.enqueueTask(sink_kernel, 0, &sink_event);
    queue.enqueueCopyBuffer(device_output_buf, host_output_buf, 0, 0, byte_size);
    queue.finish();

    cl_ulong start = source_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong stop  = sink_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    double seconds = (stop - start) / 1e9;
    std::cout << "# runtime = " << seconds << " s" << std::endl;

    std::cout << "# input data\n";
    print_data(input_data);
    std::cout << "\n# output data\n";
    print_data(output_data);

    queue.enqueueUnmapMemObject(host_input_buf, input_data.data());
    queue.enqueueUnmapMemObject(host_output_buf, output_data.data());
}

