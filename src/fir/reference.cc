#include "fir.hh"

namespace fir
{

std::vector<cl_short>
fir_reference(const FilterWeights& weights, const std::vector<cl_char>& input)
{
    std::vector<cl_short> result;
    InputBuffer inputBuffer(FilterWeightDims);

    result.reserve(input.size());
    std::fill_n(inputBuffer.data(), inputBuffer.num_elements(), 0);

    int chan = 0;
    for (auto val : input) {
        cl_short sum = 0;

        for (int tap = 0; tap < NR_TAPS; tap++) {
            sum += inputBuffer[chan][tap] * weights[chan][tap];
        }

        for (int tap = 1; tap < NR_TAPS; tap++) {
            inputBuffer[chan][tap-1] = inputBuffer[chan][tap];
        }
        inputBuffer[chan][NR_TAPS-1] = val;

        result.push_back(sum);

        chan = (chan + 1) % NR_CHANNELS;
    }

    return result;
}
}
