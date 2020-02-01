#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel short in_channel;
channel float out_channel;

//#include "fft.h"
//#include "twiddlef.h"
//#include "twiddle_32.c"
//#include "notw_32.c"

#include "fft16.h"

#define N 32

 // exp -2j
__constant float twiddle_factor[32] = { 1.0,0.0,
  0.9807852804032304,-0.19509032201612825,
  0.9238795325112867,-0.3826834323650898,
  0.8314696123025452,-0.5555702330196022,
  0.7071067811865476,-0.7071067811865475,
  0.5555702330196023,-0.8314696123025452,
  0.38268343236508984,-0.9238795325112867,
  0.19509032201612833,-0.9807852804032304,
  6.123233995736766e-17,-1.0,
  -0.1950903220161282,-0.9807852804032304,
  -0.3826834323650897,-0.9238795325112867,
  -0.555570233019602,-0.8314696123025455,
  -0.7071067811865475,-0.7071067811865476,
  -0.8314696123025453,-0.5555702330196022,
  -0.9238795325112867,-0.3826834323650899,
  -0.9807852804032304,-0.1950903220161286,
};


__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_32()
{
    float a[N], b[N+2], x[N+2];
    for (int n = 0; n < N; n ++) {
        a[n] = (float) read_channel_intel(in_channel);
    }

    fft_16((const float *)a, (float *)b); // 16-point FFT

    b[N] = 0.0f; //set last entry to zero, otherwise it would be uninitialized
    b[N+1] = 0.0f;

    //post process to complete the R2C transform
    float2 *z = (float2 *)b;
    float2 *X = (float2 *)x;
    for (int i = 0; i <= N/2; i++) {
        float2 Xe, Xo, z1, z2;
        z1 = z[i];
        z2 = z[(N/2)-i];

        //Xe[k] = ((z1.real + z2.real) + (z1.imag - z2.imag)*1j)/2
        Xe.x = ((z1.x + z2.x) * 0.5f);
        Xe.y = ((z1.y - z2.y) * 0.5f);

        //Xo[k] = -1j*((z1.real - z2.real) + (z1.imag + z2.imag)*1j)/2
        Xo.y = -(z1.x - z2.x)*0.5f;
        Xo.x =  (z1.y + z2.y)*0.5f;

        if (i<N/2) {
            //X[k] = Xe[k]+Xo[k]*twiddle_factor[k]

            //complex multiplication of Xo[k] * twiddle_factor[k]
            //c.real = a.real*b.real - a.img*b.img;
            //c.img = a.img*b.real + a.real*b.img;

            X[i].x = Xe.x + (Xo.x*twiddle_factor[(i*2)] - Xo.y*twiddle_factor[(i*2)+1]);
            X[i].y = Xe.y + (Xo.y*twiddle_factor[(i*2)] + Xo.x*twiddle_factor[(i*2)+1]);
        }
        else {
            X[i].x = (Xe.x - Xo.x)*2.0f;
            X[i].y = (Xe.y - Xo.y)*2.0f;
        }
        
    }

    X[0].x *= 2.0f;
    X[0].y = 0.0f;

    //note that we don't write the N/2+1th element (Nyquist frequency) even though we compute it
    //we could store it in the imaginary component of the first element which is always zero
    for (int n = 0; n < 32; n ++) 
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
