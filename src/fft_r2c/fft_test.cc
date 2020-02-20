#include <argagg.hpp>
#include <complex>
#include <functional>
#include <gsl/span>
#include <iostream>
#include <string>
#include <numeric>
#include <memory>

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

    short *input_data;
    posix_memalign((void**)&input_data, 64, byte_size_in);
    gsl::span<short> input_data_gsl(input_data, data_size);

    float *output_data;
    posix_memalign((void**)&output_data, 64, byte_size_out);
    gsl::span<std::complex<float>> output_data_gsl((std::complex<float>*)output_data, data_size);

/*
        gsl::span<short> input_data(
        reinterpret_cast<short*>(
            source_queue.enqueueMapBuffer(host_input_buf, CL_TRUE, CL_MAP_WRITE, 0, byte_size_in)),
        data_size);

    gsl::span<std::complex<float>> output_data(
        reinterpret_cast<std::complex<float>*>(
            sink_queue.enqueueMapBuffer(host_output_buf, CL_TRUE, CL_MAP_READ, 0, byte_size_out)),
        data_size);
*/
//    gsl::span<std::complex<float>> output_data_gsl(output_data, data_size);


    cl::Kernel source_kernel(program, "source"),
               sink_kernel(program, "sink");
    set_args(source_kernel, device_input_buf, data_size);
    set_args(sink_kernel, device_output_buf, data_size);

    cl::Event source_event, sink_event;
    std::vector<double> timings;
    std::cout << "#  timing(ms)    max_diff(epsrel)\n";

    for (unsigned i = 0; i < repeats; ++i) {
        randomize_data(input_data_gsl);
        //source_queue.enqueueCopyBuffer(host_input_buf, device_input_buf, 0, 0, byte_size_in);
        source_queue.enqueueWriteBuffer(device_input_buf, CL_TRUE, 0, byte_size_in, input_data);

        enqueue(
            std::ref(source_queue),
            std::ref(source_kernel),
            std::ref(source_event));
        enqueue(
            std::ref(sink_queue),
            std::ref(sink_kernel),
            std::ref(sink_event));
        //sink_queue.enqueueCopyBuffer(device_output_buf, host_output_buf, 0, 0, byte_size_out);
        source_queue.finish();
        sink_queue.finish();
        sink_queue.enqueueReadBuffer(device_output_buf, CL_TRUE, 0, byte_size_out, output_data);

        cl_ulong src_start = source_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        cl_ulong src_stop  = source_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        cl_ulong snk_start = sink_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        cl_ulong snk_stop  = sink_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        double src_ms = (src_stop - src_start) / 1e6;
        double snk_ms = (snk_stop - snk_start) / 1e6;
        timings.push_back(snk_ms);





        Errors err = validate_fft_r2c(shape, block_size, input_data_gsl, output_data_gsl);

        //std::cout << seconds << " " << err.abs << " " << err.rel << std::endl;
        std::cout << src_ms << " " << snk_ms << " " << err.abs << " " << err.rel << std::endl;


        cl_int error;

        //clGetProfileDataDeviceIntelFPGA(devices[0], program, true, true, true, 0, nullptr, nullptr, &error);
    cl_int (*get_profile_fn)(cl_device_id, cl_program, cl_bool,cl_bool,cl_bool,size_t, void *,size_t *,cl_int *);

    get_profile_fn = (cl_int (*) (cl_device_id, cl_program, cl_bool,cl_bool,cl_bool,size_t, void *,size_t *,cl_int *))clGetExtensionFunctionAddress("clGetProfileDataDeviceIntelFPGA");

    cl_int status = (cl_int)(*get_profile_fn) ((cl_device_id)devices[0](), (cl_program)program(), false, true, false, 0, NULL, NULL,  NULL);



    }

    double total = 0.0;
    for (double t : timings) total += t;
    std::cout << "# average runtime = " << total / repeats << " ms" << std::endl;



        source_queue.finish();
        sink_queue.finish();
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
