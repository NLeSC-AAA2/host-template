#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel float2 in_channel, out_channel;

#include "fft.h"
#include "twiddlef.h"
#include "twiddle_32.c"
#include "notw_32.c"

//#include "fftjohan2.h"


void fft(float *input, float *output) {
    //notw_32 (const R * ri, const R * ii, R * ro, R * io, stride is, stride os,
    // INT v, INT ivs, INT ovs)

    notw_32(input, input + 1, output, output + 1, 
            64, // input stride, multipied by 2 because r and i are interleaved
            2,  // output stride
            32, // outer loop iterations
            2,  // outer loop input stride
            64  // outer loop output stride
    );

    //twiddle_32 (R * ri, R * ii, const R * W, stride rs, INT mb, INT me, INT ms)

    twiddle_32(output, output + 1, w_32_32, 
               64, // row stride in output array
               0,  // begin 
               32, // end 
               2 // ?
    );

}


/*
// R0 and R1 are probably even and odd parts of the Real input
// Cr and Ci must denote complex Real and Imaginary components

// rs is the input stride (input real stride)
// csr (complex real stride)
// csi (complex imaginary stride)

// according to Johan's notes
// v is the amount of iterations to perform the FFT
// ivs and ovs are the strides for these iterations
// these can all be 1 in our case

//unnamed(R * R0, R * R1, R * Cr, R * Ci, stride rs, stride csr, stride csi, 1, 1, 1);
//unnamed(input, input+512, output, output+1, 1, 2, 2, 1, 1, 1);

__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_1024_production()
{
    //conversion from short to float should go here

    //actual kernel has 1024 real numbers in, 512 complex numbers out
    float a[1024], b[1024];
    float *a0 = (float *)a;
    float *a1 = (float *)a)+512;

    for (int n = 0; n < 512; n ++) {
        short2 x = read_channel_intel(in_channel);
        a0[n] = (float) x.x; //even 
        a1[n] = (float) x.y; //odd
    }

    fft_1024((float *)a, (float *)b);

    for (int n = 0; n < 1024; n ++)
	    write_channel_intel(out_channel, b[n]);  
}

__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_1024()
{
    //this version is for the test only

    //actual kernel has 1024 real numbers in, 512 complex numbers out
    float2 a[1024], b[1024];
    float *a0 = (float *)a;
    float *a1 = ((float *)a)+512;

    //assume input is 1024 complex numbers with only real numbers
    for (int n = 0; n < 1024; n +=2) {
        float2 x = read_channel_intel(in_channel);
        a0[n] = x.x; //even, ignore y
        x = read_channel_intel(in_channel);
        a1[n] = x.x; //odd, ignore y
    }

    fft_1024((float *)a, (float *)b);

    for (int n = 0; n < 1024; n ++)
	    write_channel_intel(out_channel, b[n]);  
}

*/


__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_1024()
{
    float2 a[1024], b[1024];
    for (int n = 0; n < 1024; n ++)
        a[n] = read_channel_intel(in_channel);
    fft((float *)a, (float *)b);
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
