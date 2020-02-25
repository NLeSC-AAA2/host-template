#include <argagg.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../cl/cl.hh"
#include "command.hh"
#include "fir.hh"

namespace fir
{

using namespace TripleA2;

void fir(const argagg::parser_results& args)
{
    if (!args["kernel"]) {
        throw std::runtime_error("-k/--kernel option is required");
    }

    if (!args["inputs"]) {
        throw std::runtime_error("-c/--count option is required");
    }

    std::string const &kernelFile = args["kernel"].as<std::string>();
    int inputSamples = ((args["inputs"].as<int>() + VECTOR_SIZE - 1) / VECTOR_SIZE) * VECTOR_SIZE;
    int outputSize = inputSamples * sizeof(cl_short);
    std::vector<cl_char> inputData(inputSamples);

    unsigned char v = 0;
    for (int i = 0; i < inputSamples; i++) {
        inputData[i] = v++;
    }

    cl::Context context;
    std::vector<cl::Device> devices;
    createContext(context, devices);

    cl::Program program = get_program(context, devices[0], kernelFile);
    cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    auto inputFlags = CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY;
    auto outputFlags = CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY;

    cl::Buffer input(context, inputFlags, inputSamples);
    cl::Buffer output(context, outputFlags, outputSize);

    {
        void *result = queue.enqueueMapBuffer(input, CL_TRUE, CL_MAP_WRITE, 0, inputSamples);
        cl_char *inputVals = static_cast<cl_char*>(result);

        for (int i = 0; i < inputSamples; i++) {
            inputVals[i] = inputData[i];
        }

        queue.enqueueUnmapMemObject(input, inputVals);
        queue.finish();
    }

    Kernel source(program, "source");
    Kernel sink(program, "sink");

    source(input, inputSamples);
    sink(output, inputSamples);

    auto outputData = fir_reference(inputData);

    source.finish();
    sink.finish();

    {
        void *result = queue.enqueueMapBuffer(output, CL_TRUE, CL_MAP_READ, 0, outputSize);
        bool correct = true;
        cl_short* outputVals = static_cast<cl_short*>(result);
        for (int i = 0; i < inputSamples; i++) {
            if (outputVals[i] != outputData.at(i)) {
                correct = false;
                std::cerr << "Mismatch at index " << i << "! Expected: "
                          << outputData[i] << " Got: " << outputVals[i]
                          << std::endl;
            }
        }

        if (correct) std::cout << "Correct!" << std::endl;

        queue.enqueueUnmapMemObject(output, outputVals);
        queue.finish();
    }
}

argagg::parser argparser {{
    { "kernel", {"-k", "--kernel"},
        ".aocx file to load kernel from", 1 },
    { "inputs", {"-c", "--count"},
        "number of inputs to run through the filter", 1 },
}};

CommandRegistry firCommand("fir", std::make_pair(&fir, argparser));
}
