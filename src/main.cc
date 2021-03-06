#include <fmt/format.h>
#include <iostream>
#include <cstdlib>

#include "cl/cl.hh"
#include "command.hh"

#define VERSION "0.1.0"
namespace TripleA2
{
    template<>
    std::unique_ptr<std::map<std::string, CommandPair>>
    Register<CommandPair>::s_dir;
}

int main(int argc, char **argv)
{
    bool reportHelp = false;
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help" ) {
            reportHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            std::cerr << "FPGA OpenCL host code template program -- "
                      << VERSION << std::endl;
            return EXIT_SUCCESS;
        }
    }

    if (argc < 2) {
        std::cerr << "No mandatory subcommand given!" << std::endl;
        return EXIT_FAILURE;
    }

    std::string command = argv[1];

    argagg::parser_results args;
    try {
        auto command_pair = TripleA2::CommandRegistry::get(command);
        if (reportHelp) {
            std::cerr << "FPGA OpenCL host code template program -- " << VERSION << std::endl;
            std::cerr << "Usage: " << argv[0] << " " << command << " [options]" << std::endl;
            std::cerr << command_pair.second;
            return EXIT_SUCCESS;
        }

        args = command_pair.second.parse(argc - 1, &argv[1]);
        try {
            command_pair.first(args);
        } catch (cl::Error const &e) {
            std::cerr << "caught cl::Error: " << e.what() << std::endl;
            std::cerr << errorMessage(e.err()) << std::endl;
            return EXIT_FAILURE;
        } catch (std::exception const &e) {
            std::cerr << argv[0] << " " << args.program << ": " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    } catch (std::out_of_range const &e) {
        std::cerr << "Unsupported subcommand: " << command << std::endl;
        std::cerr << "Supported commands:" << std::endl;
        for (auto& pair : TripleA2::CommandRegistry::get()) {
            std::cerr << "  " << pair.first << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
