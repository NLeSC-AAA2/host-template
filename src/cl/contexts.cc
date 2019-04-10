#include "contexts.hh"
#include <iostream>
#include <cstring>
#include <cstdlib>

void createContext(cl::Context &context, std::vector<cl::Device> &devices)
{
  const char *platformName = getenv("PLATFORM");

#if defined __linux__
  if (platformName == 0)
#endif
  platformName = "Intel(R) FPGA SDK for OpenCL(TM)";
  //platformName = Intel(R) OpenCL";
  //platformName = "NVIDIA CUDA";
  //platformName = "AMD Accelerated Parallel Processing";

  cl_device_type type = CL_DEVICE_TYPE_DEFAULT;

  const char *deviceType = getenv("TYPE");

  if (deviceType != 0) {
    if (strcmp(deviceType, "GPU") == 0)
      type = CL_DEVICE_TYPE_GPU;
    else if (strcmp(deviceType, "CPU") == 0)
      type = CL_DEVICE_TYPE_CPU;
    else if (strcmp(deviceType, "ACCELERATOR") == 0)
      type = CL_DEVICE_TYPE_ACCELERATOR;
    else
      std::cerr << "Unrecognized device type: " << deviceType;
  }

  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (cl::Platform &platform : platforms) {
    std::clog << "Platform name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    std::clog << "Platform version: " << platform.getInfo<CL_PLATFORM_VERSION>() << std::endl;
    std::clog << "Platform extensions: " << platform.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;
  }

  for (cl::Platform &platform : platforms) {
    if (strcmp(platform.getInfo<CL_PLATFORM_NAME>().c_str(), platformName) == 0) {
      platform.getDevices(type, &devices);

      for (cl::Device &device : devices) {
        std::clog
            << "Device: " << device.getInfo<CL_DEVICE_NAME>() << ", "
		    << "mem: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / (1024 * 1024)
            << " MB, max alloc: "
            << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / (1024 * 1024)
            << " MB"
//#if defined CL_DEVICE_TOPOLOGY_AMD
//		      ", PCIe bus 0x" << std::hex << std::setiosflags (std::ios::uppercase) << std::setfill('0') << std::setw(2) << (device.getInfo<CL_DEVICE_TOPOLOGY_AMD>().pcie.bus & 255) << std::dec
//#endif
	        << std::endl;
      }

      context = cl::Context(devices);
      return;
    }
  }

  std::cerr << "Platform not found: \"" << platformName << '"' << std::endl;
  exit(1);
}

