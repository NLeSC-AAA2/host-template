#include <complex>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <omp.h>
#include <sys/types.h>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include "sizes.h"
#include "common.h"

#define CHECKALL false

std::string errorMessage(cl_int error)
{
  switch (error) {
  case CL_SUCCESS:                            return "Success!";
  case CL_DEVICE_NOT_FOUND:                   return "Device not found.";
  case CL_DEVICE_NOT_AVAILABLE:               return "Device not available";
  case CL_COMPILER_NOT_AVAILABLE:             return "Compiler not available";
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
  case CL_OUT_OF_RESOURCES:                   return "Out of resources";
  case CL_OUT_OF_HOST_MEMORY:                 return "Out of host memory";
  case CL_PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
  case CL_MEM_COPY_OVERLAP:                   return "Memory copy overlap";
  case CL_IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
  case CL_BUILD_PROGRAM_FAILURE:              return "Program build failure";
  case CL_MAP_FAILURE:                        return "Map failure";
  case CL_INVALID_VALUE:                      return "Invalid value";
  case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
  case CL_INVALID_PLATFORM:                   return "Invalid platform";
  case CL_INVALID_DEVICE:                     return "Invalid device";
  case CL_INVALID_CONTEXT:                    return "Invalid context";
  case CL_INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
  case CL_INVALID_COMMAND_QUEUE:              return "Invalid command queue";
  case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
  case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
  case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
  case CL_INVALID_SAMPLER:                    return "Invalid sampler";
  case CL_INVALID_BINARY:                     return "Invalid binary";
  case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
  case CL_INVALID_PROGRAM:                    return "Invalid program";
  case CL_INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
  case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
  case CL_INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
  case CL_INVALID_KERNEL:                     return "Invalid kernel";
  case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
  case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
  case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
  case CL_INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
  case CL_INVALID_WORK_DIMENSION:             return "Invalid work dimension";
  case CL_INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
  case CL_INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
  case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
  case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
  case CL_INVALID_EVENT:                      return "Invalid event";
  case CL_INVALID_OPERATION:                  return "Invalid operation";
  case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
  case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
  case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
  case CL_INVALID_GLOBAL_WORK_SIZE:           return "Invalid global work size";
#if defined CL_INVALID_PROPERTY
  case CL_INVALID_PROPERTY:                   return "Invalid property";
#endif
#if defined CL_INVALID_IMAGE_DESCRIPTOR
  case CL_INVALID_IMAGE_DESCRIPTOR:           return "Invalid image descriptor";
#endif
#if defined CL_INVALID_COMPILER_OPTIONS
  case CL_INVALID_COMPILER_OPTIONS:           return "Invalid compiler options";
#endif
#if defined CL_INVALID_LINKER_OPTIONS
  case CL_INVALID_LINKER_OPTIONS:             return "Invalid linker options";
#endif
#if defined CL_INVALID_DEVICE_PARTITION_COUNT
  case CL_INVALID_DEVICE_PARTITION_COUNT:     return "Invalid device partition count";
#endif
  default:                                    std::stringstream str;
    str << "Unknown (" << error << ')';
    return str.str();
  }
}

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

  const char *deviceName = getenv("DEVICE");

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
	std::clog << "Device: " << device.getInfo<CL_DEVICE_NAME>() << ", "
		      "mem: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / (1024 * 1024) << " MB, max alloc: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / (1024 * 1024) << " MB"
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

cl::Program createProgramFromBinaries(cl::Context &context, std::vector<cl::Device> &devices, const std::string &name) 
{
  std::ifstream ifs(name, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  cl::Program::Binaries binaries(devices.size(), std::make_pair(str.c_str(), str.length()));
  return cl::Program(context, devices, binaries);
} 

typedef float MatrixA[MAT_A_HEIGHT][MAT_A_WIDTH];
typedef float MatrixB[MAT_B_WIDTH][MAT_B_HEIGHT];
typedef float MatrixC[MAT_C_HEIGHT][MAT_C_WIDTH];

void computeAll(MatrixA& matA, MatrixB& matB, MatrixC& res) {
    for(int r = 0; r < MAT_C_HEIGHT ; r++){
        for(int c = 0 ; c < MAT_C_WIDTH ; c++){
            float dot = 0.0f;
            for(int k = 0 ; k < MAT_A_WIDTH ; k++){
                dot+=matA[r][k] * matB[c][k];
            }
            res[r][c] = dot;
        }
    }
}

void checkAll(MatrixC& ref, MatrixC& res, float eps) {
    int count = 0;
    for(int r = 0; r < MAT_C_HEIGHT ; r++){
        for(int c = 0 ; c < MAT_C_WIDTH ; c++){
           float diff = ref[r][c] - res[r][c];
           if(diff > eps || diff < -eps){
             std::cout << "Diff on c[" << r << "][" << c << "] diff: " << diff << " ref: " << ref[r][c] << " res: " << res[r][c] << std::endl;
             count++;
             if(count >= 30) {
                return;
            }
           }
        }
    }
    if(count == 0) std::cout << "All the same!" << std::endl;
}

void randomize_array(float* array, const int size){
    std::random_device rd;
    std::mt19937 gen(rd());
    for(int i = 0; i < size ; i++){
        array[i] = std::generate_canonical<float, 16>(gen);
    }
} 

#undef CL_MEM_HOST_READ_ONLY  // 17.1 does not accept them, at least not in the emulator
#define CL_MEM_HOST_READ_ONLY	0
#undef CL_MEM_HOST_WRITE_ONLY
#define CL_MEM_HOST_WRITE_ONLY	0

int main(int argc, char **argv)
{
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " file.aocx" << std::endl;
    exit(1);
  }

  try {
    cl::Context             context;
    std::vector<cl::Device> devices;
    createContext(context, devices);

    std::string filename(argv[1]);
    //cl::Program program(createProgramFromBinaries(context, devices, argv[1]));
    cl::Program program = get_program(context, devices[0], filename);
    printf("Done loading!");
    cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    cl::Buffer matrixAbufferHost(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(MatrixA));
    cl::Buffer matrixAbufferDevice(context, CL_MEM_READ_ONLY, sizeof(MatrixA));
    cl::Buffer matrixBbufferHost(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(MatrixB));
    cl::Buffer matrixBbufferDevice(context, CL_MEM_READ_ONLY, sizeof(MatrixB));
    cl::Buffer matrixCbufferHost(context, CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(MatrixC));
    cl::Buffer matrixCbufferDevice(context, CL_MEM_WRITE_ONLY, sizeof(MatrixC));

    cl::Kernel inKernel(program, "matrix_mult");
    inKernel.setArg(0, matrixAbufferDevice);
    inKernel.setArg(1, matrixBbufferDevice);
    inKernel.setArg(2, matrixCbufferDevice);
    inKernel.setArg(3, (unsigned int) MAT_C_HEIGHT);
    inKernel.setArg(4, (unsigned int) MAT_C_WIDTH);
    inKernel.setArg(5, (unsigned int) NR_VECTORS_K);
/*
      cl::Kernel compute(program, "matrix_mult");
      compute.setArg(0, (unsigned int) MAT_C_HEIGHT);
      compute.setArg(1, (unsigned int) MAT_C_WIDTH);
      compute.setArg(2, (unsigned int) NR_VECTORS_K);

      cl::Kernel outKernel(program, "write");
      outKernel.setArg(0, matrixCbufferDevice);
      outKernel.setArg(1, (unsigned int) MAT_C_HEIGHT);
      outKernel.setArg(2, (unsigned int)  MAT_C_WIDTH);
*/
    std::vector<cl::Event> hostToDeviceDone(1), computeDone(1), deviceToHostDone(1);
    queue.enqueueMarker(&computeDone[0]);

    MatrixC c;
    {
        MatrixA &matrixA = * (MatrixA *) queue.enqueueMapBuffer(matrixAbufferHost, CL_TRUE, CL_MAP_WRITE, 0, sizeof(MatrixA));
        MatrixB &matrixB = * (MatrixB *) queue.enqueueMapBuffer(matrixBbufferHost, CL_TRUE, CL_MAP_WRITE, 0, sizeof(MatrixB));
        randomize_array((float*)matrixA, sizeof(MatrixA)/sizeof(float));

        randomize_array((float*)matrixB, sizeof(MatrixB)/sizeof(float));
        if(CHECKALL) {
            computeAll(matrixA, matrixB, c);
        }
        queue.enqueueUnmapMemObject(matrixAbufferHost, matrixA);
        queue.enqueueUnmapMemObject(matrixBbufferHost, matrixB);
    }
    printf("Transfer data done!");

    {
        queue.enqueueCopyBuffer(matrixAbufferHost, matrixAbufferDevice, 0, 0, sizeof(MatrixA), &computeDone);
        queue.enqueueCopyBuffer(matrixBbufferHost, matrixBbufferDevice, 0, 0, sizeof(MatrixB), 0, &hostToDeviceDone[0]);
        //  queueA.enqueueTask(inKernel, &hostToDeviceDone);
        // queueB.enqueueTask(compute, &hostToDeviceDone);
        queue.enqueueTask(inKernel, &hostToDeviceDone, &computeDone[0]);
        queue.enqueueCopyBuffer(matrixCbufferDevice, matrixCbufferHost, 0, 0, sizeof(MatrixC), &computeDone, &deviceToHostDone[0]);
    }

    MatrixC &matrixC = * (MatrixC *) queue.enqueueMapBuffer(matrixCbufferHost, CL_TRUE, CL_MAP_READ, 0, sizeof(MatrixC), &deviceToHostDone);

    if(CHECKALL) {
        checkAll(c, matrixC,0.001);
    }

    queue.enqueueUnmapMemObject(matrixCbufferHost, matrixC);
    computeDone[0].wait();
    cl_ulong start = computeDone[0].getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong stop  = computeDone[0].getProfilingInfo<CL_PROFILING_COMMAND_END>();
    double seconds = (stop - start) / 1e9;
    double gflops = FLOPS / 1e9;
    cl_int error;
    cl_int err = clGetProfileDataDeviceIntelFPGA(devices[0](), program(), true, true, true, 0, nullptr, nullptr, &error);
    if (err != CL_SUCCESS) {
        if(err == CL_INVALID_DEVICE){
            std::cerr << "CL_INVALID_DEVICE" << std::endl;
        } else if(err == CL_INVALID_PROGRAM){
            std::cerr << "CL_INVALID_PROGRAM" << std::endl;
        }
        std::cerr << "Error getting profiling data: " << err << std::endl;
        //exit(EXIT_FAILURE);
    }

    std::cout << 0 << ": runtime = " << seconds << " s" << std::endl;
    std::cout <<  gflops << "GFLOPS" << std::endl;
  } catch (cl::Error &error) {
      std::cerr << "caught cl::Error: " << error.what() << ": " << errorMessage(error.err()) << std::endl;
  }

  return 0;
}
