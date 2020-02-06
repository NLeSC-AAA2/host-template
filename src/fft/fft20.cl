#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel float2 in_channel, out_channel;

#include "fftjohan20_4_5.h"
//#include "fftjohan20_2_2_5.h"

__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_20()
{
    float2 a[20], b[20];
    for (int n = 0; n < 20; n ++)
	    a[n] = read_channel_intel(in_channel);
    fft((float const *)a, (float *)b);
    for (int n = 0; n < 20; n ++)
	    write_channel_intel(out_channel, b[n]);  
}

__attribute__((max_global_work_dim(0)))
__kernel void source(__global const volatile float2 *in, unsigned count)
{
  for (unsigned i = 0; i < count; i ++)
    write_channel_intel(in_channel, in[i]);
}

__attribute__((max_global_work_dim(0)))
__kernel void sink(__global float2 *out, unsigned count)
{
  for (unsigned i = 0; i < count; i ++)
    out[i] = read_channel_intel(out_channel);
}

// vim:ft=c
