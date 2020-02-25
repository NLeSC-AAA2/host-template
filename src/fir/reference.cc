#include "fir.hh"

namespace fir
{

std::vector<cl_short>
fir_reference(const std::vector<cl_char>& input)
{
    std::vector<cl_short> result;
    cl_char inputBuffer[NR_CHANNELS][NR_TAPS];

    result.reserve(input.size());
    for (int chan = 0; chan < NR_CHANNELS; chan++) {
        for (int tap = 0; tap < NR_TAPS; tap++) {
            inputBuffer[chan][tap] = 0;
        }
    }

    int chan = 0;
    for (auto val : input) {
        cl_short sum = 0;

        for (int tap = 0; tap < NR_TAPS; tap++) {
            sum += inputBuffer[chan][tap] * filterWeights[chan][tap];
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
