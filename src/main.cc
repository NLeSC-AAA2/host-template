#include <argagg.hpp>
#include <fmt/format.h>
#include <iostream>
#include <cstdlib>

#define VERSION "0.1.0"

int main(int argc, char **argv)
{
    argagg::parser argparser {{
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

    if (args["help"]) {
        std::cerr << "FPGA OpenCL host code template program -- " << VERSION << std::endl;
        std::cerr << "Usage: " << argv[0] << " [options]" << std::endl;
        std::cerr << argparser;
        return EXIT_SUCCESS;
    }

    if (args["version"]) {
        std::cerr << "FPGA OpenCL host code template program -- " << VERSION << std::endl;
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

