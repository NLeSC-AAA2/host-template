#include "binaries.hh"
#include <iostream>
#include <fstream>
#include <utility>

cl::Program createProgramFromBinaries(
        cl::Context &context, std::vector<cl::Device> &devices, const std::string &name)
{
  std::ifstream ifs(name, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  cl::Program::Binaries binaries{std::vector<unsigned char>(str.begin(), str.end())};
  return cl::Program(context, devices, binaries);
} 

