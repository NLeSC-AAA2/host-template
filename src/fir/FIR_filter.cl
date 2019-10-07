#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

constant int NR_TAPS = 16;
constant int VECTOR_SIZE = 16;
constant int NR_CHANNELS = 1024;
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
FIR_filter
(constant const short * restrict filterWeights, int chunkCount)
{
    short16 inputShiftReg[TAPS_CHANNELS_MULT];
    short16 inputShiftReg2[TAPS_CHANNELS_MULT];
    short16 filterWeightsCache[TAPS_CHANNELS_MULT];

    for (int i = 0; i < TAPS_CHANNELS_MULT; i++) {
        inputShiftReg[i] = 0;
        filterWeightsCache[i] = vload16(i, filterWeights);
    }

    for (int i = 0; i < chunkCount; i++) {
        const unsigned channelIdx = i % NR_CHANNEL_CHUNKS;
        const unsigned tapOffset = (channelIdx * NR_TAPS);
        const bool odd = i & 1;
        short16 result = 0;
        short16 new = convert_short16(read_channel_intel(in_channel));

        if (!odd) result = computeOutput(filterWeightsCache, inputShiftReg, inputShiftReg2, tapOffset, new);
        else result = computeOutput(filterWeightsCache, inputShiftReg2, inputShiftReg, tapOffset, new);

        write_channel_intel(fft_channel, result);
    }
}

kernel void
__attribute__((max_global_work_dim(0)))
source(global const char16 * restrict data, int count)
{
    for (int i = 0; i < count; i++) {
        write_channel_intel(in_channel, data[i]);
    }
}

kernel void
__attribute__((max_global_work_dim(0)))
sink(global short16 * restrict data, int count)
{
    for (int i = 0; i < count; i++) {
        data[i] = read_channel_intel(fft_channel);
    }
}
