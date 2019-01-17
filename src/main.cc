#include <argagg.hpp>
#include <fmt/format.h>
#include <iostream>
#include <cstdlib>
#include "cl/cl.hh"

#define VERSION "0.1.0"
#define N 10
#define M 32*32

extern void fft_test(std::string const &filename, unsigned fft_size, unsigned repeats);

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
        std::string filename = args["kernel"].as<std::string>();
        fft_test(filename, M, N);
    } catch (cl::Error const &e) {
        std::cerr << "caught cl::Error: " << e.what() << std::endl;
        std::cerr << errorMessage(e.err()) << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

