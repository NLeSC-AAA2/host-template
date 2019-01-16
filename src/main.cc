#include <argagg.hpp>
#include <fmt/format.h>
#include <iostream>
#include <cstdlib>

#define VERSION "0.1.0"
#define N 100
#define M 32*32

#include "cl/cl.hh"

void randomize_data(std::vector<cl_float2> &data)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    for (cl_float2 &z : data) {
        z[0] = std::generate_canonical<float, 16>(gen);
        z[1] = 0.0;
    }
}

int main(int argc, char **argv)
{
    argagg::parser argparser {{
        { "kernel", {"-k", "--kernel"},
          ".aocx file to load kernel from", 1 },
        { "help", {"-h", "--help"},
          "shows this help message", 0 },
        { "version", {"-v", "--version"},
          "shows the software version", 0 }
    }};

    argagg::parser_results args;
    try {
        args = argparser.parse(argc, argv);
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (!args["kernel"]) {
        std::cerr << args.program << ": -k/--kernel option is required" << std::endl;
        return EXIT_FAILURE;
    }

    if (args["help"]) {
        std::cerr << "FPGA OpenCL host code template program -- " << VERSION << std::endl;
        std::cerr << "Usage: " << args.program << " [options]" << std::endl;
        std::cerr << argparser;
        return EXIT_SUCCESS;
    }

    if (args["version"]) {
        std::cerr << "FPGA OpenCL host code template program -- " << VERSION << std::endl;
        return EXIT_SUCCESS;
    }

    try {
        cl::Context context;
        std::vector<cl::Device> devices;
        createContext(context, devices);

        std::string filename = args["kernel"].as<std::string>();
        cl::Program program = get_program(context, devices[0], filename);
        cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);
       
        std::vector<cl_float2> input_data(M * N), output_data(M * N); 
        size_t bs = sizeof(cl_float2) * M * N;
        cl::Buffer input_device(context, CL_MEM_WRITE_ONLY, bs);
        cl::Buffer output_device(context, CL_MEM_READ_ONLY, bs);
        
        queue.enqueueWriteBuffer(input_device, CL_TRUE, 0, bs, input_data.data(), NULL, NULL);
        cl::Kernel inKernel(program, "source");
        set_args(inKernel, input_device, M*N);

        cl::Kernel outKernel(program, "sink");
        set_args(outKernel, output_device, M*N);

        std::vector<cl::Event> wait_list;
        queue.enqueueReadBuffer(output_device, CL_TRUE, 0, bs, output_data.data(), &wait_list, NULL);
        
    } catch (cl::Error const &e) {
        std::cerr << "caught cl::Error: " << e.what() << std::endl;
        std::cerr << errorMessage(e.err()) << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

