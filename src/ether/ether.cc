#include <argagg.hpp>
#include <fstream>
#include <iostream>

#include "../cl/cl.hh"
#include "command.hh"

constexpr unsigned CHAN_BYTE_WIDTH = 32;

namespace
{

using namespace TripleA2;

void ether(const argagg::parser_results& args)
{
    if (!args["kernel"]) {
        throw std::runtime_error("-k/--kernel option is required");
    }

    if (!args["packet"]) {
        throw std::runtime_error("-p/--packet option is required");
    }

    std::string const &kernelFile = args["kernel"].as<std::string>();
    std::string const &packetFile = args["packet"].as<std::string>();

    cl::Context context;
    std::vector<cl::Device> devices;
    createContext(context, devices);

    cl::Program program = get_program(context, devices[0], kernelFile);
    cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    std::ifstream file(packetFile, std::ios::binary | std::ios::ate);
    std::streamsize packetSize = file.tellg();
    cl_uint chunkCount = (packetSize + CHAN_BYTE_WIDTH - 1) / CHAN_BYTE_WIDTH;
    cl_uint paddedPacketSize = chunkCount * CHAN_BYTE_WIDTH;
    file.seekg(0, std::ios::beg);

    cl::Buffer device_buf(context, CL_MEM_READ_ONLY, paddedPacketSize);
    cl::Buffer host_buf(context,
            CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, paddedPacketSize);

    void *buffer = queue.enqueueMapBuffer(host_buf, CL_TRUE, CL_MAP_WRITE, 0, paddedPacketSize);
    if (!file.read(static_cast<char*>(buffer), packetSize))
    { throw std::runtime_error("failed to read packet file"); }

    cl::Event event;
    Kernel kernel(program, "sendUDP");

    queue.enqueueCopyBuffer(host_buf, device_buf, 0, 0, paddedPacketSize);

    kernel(device_buf, chunkCount);
    kernel.finish();

    queue.enqueueUnmapMemObject(host_buf, buffer);
}

argagg::parser argparser {{
    { "kernel", {"-k", "--kernel"},
        ".aocx file to load kernel from", 1 },
    { "packet", {"-p", "--packet"},
        "The file to load packet data from", 1 },
}};

CommandRegistry etherCommand("ether", std::make_pair(&ether, argparser));
}
