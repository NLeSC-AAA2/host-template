#include <argagg.hpp>
#include <complex>
#include <functional>
#include <gsl/span>
#include <iostream>
#include <string>
#include <numeric>

#include <random>
#include <thread>

#include "../cl/cl.hh"
#include "command.hh"
#include "validation.hh"
#include "arg-vector.hh"

namespace fft_r2c
{

using namespace TripleA2;

void randomize_data(gsl::span<short> data)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    for (auto &z : data) {
        z = (short) (32768*dist(gen));
    }
}

/*
void print_data(gsl::span<std::complex<float>> data)
{
    for (auto const &z : data) {
        std::cout << z.real() << " " << z.imag() << std::endl;
    }
}
*/

void enqueue(
    cl::CommandQueue &queue,
    cl::Kernel const &kernel,
    cl::Event &event)
{
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1), cl::NullRange, nullptr, &event);
}

template <typename T>
inline T product(std::vector<T> const &v)
{
    return std::accumulate(
        std::begin(v), std::end(v), 1, std::multiplies<T>());
}

void fft_test_r2c(const argagg::parser_results& args)
{
    if (!args["kernel"]) {
        throw std::runtime_error("-k/--kernel option is required");
    }

    if (!args["shape"]) {
        throw std::runtime_error("-s/--shape option is required");
    }

    std::string const &filename = args["kernel"].as<std::string>();
    std::vector<int> shape = args["shape"].as<std::vector<int>>();
    unsigned fft_size = static_cast<unsigned>(product(shape));
    unsigned repeats = args["repeat"].as<unsigned>(100);
    unsigned block_size = args["block"].as<unsigned>(10);
    unsigned data_size = fft_size * block_size,
             byte_size_in = data_size * sizeof(short),
             byte_size_out = data_size * sizeof(float); //complex numbers but half the length

    cl::Context context;
    std::vector<cl::Device> devices;
    createContext(context, devices);

    cl::Program program = get_program(context, devices[0], filename);
    cl::CommandQueue source_queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);
    cl::CommandQueue sink_queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    cl::Buffer device_input_buf(context, CL_MEM_READ_ONLY, byte_size_in);
    cl::Buffer device_output_buf(context, CL_MEM_WRITE_ONLY, byte_size_out);
    cl::Buffer host_input_buf(context,
            CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, byte_size_in);
    cl::Buffer host_output_buf(context,
            CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, byte_size_out);


    cl::Kernel source_kernel(program, "source"),
               sink_kernel(program, "sink");
    set_args(source_kernel, device_input_buf, data_size);
    set_args(sink_kernel, device_output_buf, data_size);

    cl::Event source_event, sink_event;
    std::vector<double> timings;
    std::cout << "#  timing(s)    max_diff(epsrel)\n";
    for (unsigned i = 0; i < repeats; ++i) {
    {
        gsl::span<short> input_data(
        reinterpret_cast<short*>(
            source_queue.enqueueMapBuffer(host_input_buf, CL_TRUE, CL_MAP_WRITE, 0, byte_size_in)),
        data_size);
        randomize_data(input_data);
        source_queue.enqueueCopyBuffer(host_input_buf, device_input_buf, 0, 0, byte_size_in);
    }
        std::thread source_thread(enqueue,
            std::ref(source_queue),
            std::ref(source_kernel),
            std::ref(source_event));
        std::thread sink_thread(enqueue,
            std::ref(sink_queue),
            std::ref(sink_kernel),
            std::ref(sink_event));
        source_thread.join();
        sink_thread.join();
        sink_queue.enqueueCopyBuffer(device_output_buf, host_output_buf, 0, 0, byte_size_out);
        source_queue.finish();
        sink_queue.finish();

        cl_ulong start = source_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        cl_ulong stop  = sink_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        double seconds = (stop - start) / 1e9;
        timings.push_back(seconds);


        gsl::span<short> input_data(
        reinterpret_cast<short*>(
            source_queue.enqueueMapBuffer(host_input_buf, CL_TRUE, CL_MAP_WRITE, 0, byte_size_in)),
        data_size);

    gsl::span<std::complex<float>> output_data(
        reinterpret_cast<std::complex<float>*>(
            sink_queue.enqueueMapBuffer(host_output_buf, CL_TRUE, CL_MAP_READ, 0, byte_size_out)),
        data_size);


        Errors err = validate_fft_r2c(shape, block_size, input_data, output_data);
        sink_queue.enqueueUnmapMemObject(host_output_buf, output_data.data());
        source_queue.enqueueUnmapMemObject(host_input_buf, input_data.data());

        std::cout << seconds << " " << err.abs << " " << err.rel << std::endl;
    }

    double total = 0.0;
    for (double t : timings) total += t;
    std::cout << "# average runtime = " << total / repeats << " s" << std::endl;

}

argagg::parser argparser {{
    { "kernel", {"-k", "--kernel"},
        ".aocx file to load kernel from", 1 },
    { "shape", {"-s", "--shape"},
        "shape of fft; separate values by any non-number non-whitespace character.", 1 },
    { "block", {"-b", "--block"},
        "(10) block size: number of ffts per go", 1 },
    { "repeat", {"-r", "--repeat"},
        "(100) send a multitude of data to source", 1 }
}};

CommandRegistry fftCommand("fft_r2c", std::make_pair(&fft_test_r2c, argparser));
}
