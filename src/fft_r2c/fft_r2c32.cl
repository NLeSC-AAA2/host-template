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

__constant float cos16[16] = {
1.00000000000000000000,
0.98078528040323043058,
0.92387953251128673848,
0.83146961230254523567,
0.70710678118654757274,
0.55557023301960228867,
0.38268343236508983729,
0.19509032201612833135,
0.00000000000000006123,
-0.19509032201612819257,
-0.38268343236508972627,
-0.55557023301960195560,
-0.70710678118654746172,
-0.83146961230254534669,
-0.92387953251128673848,
-0.98078528040323043058
};

__constant float sin16[16] = {
0.00000000000000000000,
0.19509032201612824808,
0.38268343236508978178,
0.55557023301960217765,
0.70710678118654746172,
0.83146961230254523567,
0.92387953251128673848,
0.98078528040323043058,
1.00000000000000000000,
0.98078528040323043058,
0.92387953251128673848,
0.83146961230254545772,
0.70710678118654757274,
0.55557023301960217765,
0.38268343236508989280,
0.19509032201612860891
};


__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_32()
{
    float a[N], b[2*N], x[N];
    for (int n = 0; n < N; n ++) {
        a[n] = (float) read_channel_intel(in_channel);
    }

    fft_16((const float *)a, (float *)b); // 16-point C2C FFT


    //post process to complete the R2C transform
    float2 *z = (float2 *)b;
    float2 *X = (float2 *)x;
    for (int i = 0; i <= N/2; i++) {
        float a, b, c, d;

        a = ( z[i].x + z[(N/2)-i].x ) / 2.0f;
        b = ( z[i].x - z[(N/2)-i].x ) / 2.0f;
        c = ( z[i].y + z[(N/2)-i].y ) / 2.0f;
        d = ( z[i].y - z[(N/2)-i].y ) / 2.0f;


        X[i].x = a + cos16[i]*c - sin16[i]*b;
        X[i].y = d - sin16[i]*c - cos16[i]*b;
        
    }

    X[0].x *= 2.0f;
    X[0].y = 0.0f;

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
