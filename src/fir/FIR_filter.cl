#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

constant int NR_TAPS = 16;
constant int VECTOR_SIZE = 16;
constant int NR_CHANNELS = 1024;
constant int NR_CHANNEL_CHUNKS = (NR_CHANNELS + VECTOR_SIZE - 1) / VECTOR_SIZE;
constant int TAPS_CHANNELS_MULT = NR_TAPS * NR_CHANNEL_CHUNKS;

channel char16 in_channel;
channel short16 fft_channel;

kernel void
__attribute__((max_global_work_dim(0)))
FIR_filter
(constant const short * restrict filterWeights, int chunkCount)
{
    short16 inputShiftReg[TAPS_CHANNELS_MULT];
    short16 filterWeightsCache[TAPS_CHANNELS_MULT];

    #pragma loop_coalesce 2
    for (int i = 0; i < NR_TAPS; i++) {
        for (int j = 0; j < NR_CHANNEL_CHUNKS; j++) {
            inputShiftReg[(j * NR_TAPS) + i] = 0;
            filterWeightsCache[(j * NR_TAPS) + i] = vload16(j*NR_TAPS, filterWeights);
        }
    }

    for (int channelIdx = 0; channelIdx < NR_CHANNEL_CHUNKS; channelIdx++) {
        short16 result = 0;

        #pragma unroll
        for (unsigned tap = 0; tap < NR_TAPS; tap++) {
            short16 v = filterWeightsCache[(channelIdx * VECTOR_SIZE) + tap];
            result += inputShiftReg[(channelIdx * VECTOR_SIZE) + tap] * v;
        }

        #pragma unroll
        for (unsigned i = 1; i < NR_TAPS; i++) {
            inputShiftReg[(channelIdx * VECTOR_SIZE) + (i-1)] = inputShiftReg[(channelIdx * VECTOR_SIZE) + i];
        }

        inputShiftReg[(channelIdx * VECTOR_SIZE) + (NR_TAPS-1)] = convert_short16(read_channel_intel(in_channel));

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
