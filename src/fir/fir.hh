#ifndef FIR_HH
#define FIR_HH

#include <boost/multi_array.hpp>

#include "../cl/cl.hh"

namespace fir
{
constexpr int NR_TAPS = 16;
constexpr int VECTOR_SIZE = 16;
constexpr int NR_CHANNELS = 1024;

typedef boost::multi_array<cl_short, 2> FilterWeights;
typedef boost::multi_array<cl_char, 2> InputBuffer;
const auto FilterWeightDims = boost::extents[NR_CHANNELS][NR_TAPS];

std::vector<cl_short>
fir_reference(const FilterWeights&, const std::vector<cl_char>&);
}
#endif
