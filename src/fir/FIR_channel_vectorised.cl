#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>
#include "FIR-1024x16-channel.h"

constant int NR_CHANNEL_CHUNKS = (NR_CHANNELS + VECTOR_SIZE - 1) / VECTOR_SIZE;
constant int TAPS_CHANNELS_MULT = NR_TAPS * NR_CHANNEL_CHUNKS;

channel char16 in_channel;
channel short16 fft_channel;

short16
computeOutput
( const short16 * restrict filterWeightsCache
, short16 * restrict inputShiftReg
, short16 * restrict outputShiftReg
, unsigned weightOffset
, short16 new
)
{
    short16 result = 0;

    for (unsigned tap = 0; tap < NR_TAPS; tap++) {
        result += inputShiftReg[tap] * filterWeightsCache[weightOffset + tap];
    }

    for (unsigned tap = NR_TAPS; tap < TAPS_CHANNELS_MULT; tap++) {
        outputShiftReg[tap - NR_TAPS] = inputShiftReg[tap];
    }

    for (unsigned tap = 1; tap < NR_TAPS; tap++) {
        outputShiftReg[TAPS_CHANNELS_MULT - NR_TAPS + tap - 1] = inputShiftReg[tap];
    }

    outputShiftReg[TAPS_CHANNELS_MULT - 1] = new;
    return result;
}

kernel void
__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
FIR_filter()
{
    short16 inputShiftReg[NR_CHANNEL_CHUNKS][NR_TAPS];
    short16 filterWeightsCache[NR_CHANNEL_CHUNKS][NR_TAPS];

    int i = 0;
    #pragma loop_coalesce 2
    for (int chan = 0; chan < NR_CHANNEL_CHUNKS; chan++) {
        for (int tap = 0; tap < NR_TAPS; tap++) {
            inputShiftReg[chan][tap] = 0;
            filterWeightsCache[chan][tap] = vload16(i++, filterWeights);
        }
    }

    int chan = 0;
    #pragma ivdep safelen(NR_CHANNEL_CHUNKS)
    while (true) {
        short16 result = 0;
        #pragma unroll
        for (int tap = 0; tap < NR_TAPS; tap++) {
            result += inputShiftReg[chan][tap] * filterWeightsCache[chan][tap];
        }

        #pragma unroll
        for (int tap = 1; tap < NR_TAPS; tap++) {
            inputShiftReg[chan][tap - 1] = inputShiftReg[chan][tap];
        }

        inputShiftReg[chan][NR_TAPS - 1] = convert_short16(read_channel_intel(in_channel));

        write_channel_intel(fft_channel, result);
        chan = (chan + 1) % NR_CHANNEL_CHUNKS;
    }
}

kernel void
__attribute__((max_global_work_dim(0)))
source(global const char16 * restrict data, int inputSamples)
{
    int chunkCount = (inputSamples + VECTOR_SIZE -1) / VECTOR_SIZE;
    for (int i = 0; i < chunkCount; i++) {
        write_channel_intel(in_channel, data[i]);
    }
}

kernel void
__attribute__((max_global_work_dim(0)))
sink(global short16 * restrict data, int inputSamples)
{
    int chunkCount = (inputSamples + VECTOR_SIZE -1) / VECTOR_SIZE;
    for (int i = 0; i < chunkCount; i++) {
        data[i] = read_channel_intel(fft_channel);
    }
}
