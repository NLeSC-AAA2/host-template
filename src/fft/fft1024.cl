#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel float2 in_channel, out_channel;

//#include "fft.h"
//#include "twiddlef.h"
//#include "twiddle_32.c"
//#include "notw_32.c"

//#include "fft1024_2.h"
#include "fft1024_4_ununrolled.h"



__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_1024()
{
    float2 a[1024], b[1024];
    for (int n = 0; n < 1024; n ++)
        a[n] = read_channel_intel(in_channel);
    fft((const float *)a, (float *)b);
    for (int n = 0; n < 1024; n ++)
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
