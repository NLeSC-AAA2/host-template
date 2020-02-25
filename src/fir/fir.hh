#ifndef FIR_HH
#define FIR_HH

#include "../cl/cl.hh"
#include "FIR-1024x16-host.hh"

namespace fir
{
constexpr int VECTOR_SIZE = 16;
std::vector<cl_short> fir_reference(const std::vector<cl_char>&);
}
#endif
