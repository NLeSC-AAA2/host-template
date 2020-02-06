#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel short in_channel;
channel float out_channel;

//#include "fft.h"
//#include "twiddlef.h"
//#include "twiddle_32.c"
//#include "notw_32.c"

//#include "fft512_reorder.h"
#include "fft512_loop.h"

#include "sincos512.h"

#define N 1024









__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_1024()
{
    float a[N], b[N], x[N];
    for (int n = 0; n < N; n ++) {
        a[n] = (float) read_channel_intel(in_channel);
    }

    fft((const float *)a, (float *)b); // 512-point FFT

    //post process to complete the R2C transform
    float2 *z = (float2 *)b;
    float2 *X = (float2 *)x;
    #pragma ivdep
    for (int i = 1; i < N/2; i++) {
        float a, b, c, d;

        a = z[i].x + z[(N/2)-i].x;
        b = z[i].x - z[(N/2)-i].x;
        c = z[i].y + z[(N/2)-i].y;
        d = z[i].y - z[(N/2)-i].y;


        X[i].x = 0.5 * (a + cos512[i]*c - sin512[i]*b);
        X[i].y = 0.5 * (d - sin512[i]*c - cos512[i]*b);

/*
        a = ( z[i].x + z[(N/2)-i].x ) / 2.0f;
        b = ( z[i].x - z[(N/2)-i].x ) / 2.0f;
        c = ( z[i].y + z[(N/2)-i].y ) / 2.0f;
        d = ( z[i].y - z[(N/2)-i].y ) / 2.0f;

        X[i].x = a + cos512[i]*c - sin512[i]*b;
        X[i].y = d - sin512[i]*c - cos512[i]*b;
*/

    }

    X[0].x = z[0].x + cos512[0]*z[0].y - sin512[0]*z[0].x;
    X[0].y = 0.0f;

    for (int n = 0; n < N; n ++) 
        write_channel_intel(out_channel, x[n]);
}



__attribute__((max_global_work_dim(0)))
__kernel void source(__global const volatile short *in, unsigned count)
{
  for (unsigned i = 0; i < count; i ++)
    write_channel_intel(in_channel, in[i]);
}

__attribute__((max_global_work_dim(0)))
__kernel void sink(__global float *out, unsigned count)
{
  for (unsigned i = 0; i < count; i ++)
    out[i] = read_channel_intel(out_channel);
}

// vim:ft=c
