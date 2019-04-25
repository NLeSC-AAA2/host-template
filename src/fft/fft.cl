#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

channel float2 in_channel, out_channel;


__constant struct command {
  uint5_t even, odd /*: 5 */;
  float2 weight;
}
recipe[] =
{
  {  0, 16, (float2) (1,0) },
  {  8, 24, (float2) (1,0) },
  {  4, 20, (float2) (1,0) },
  { 12, 28, (float2) (1,0) },
  {  2, 18, (float2) (1,0) },
  { 10, 26, (float2) (1,0) },
  {  6, 22, (float2) (1,0) },
  { 14, 30, (float2) (1,0) },
  {  1, 17, (float2) (1,0) },
  {  9, 25, (float2) (1,0) },
  {  5, 21, (float2) (1,0) },
  { 13, 29, (float2) (1,0) },
  {  3, 19, (float2) (1,0) },
  { 11, 27, (float2) (1,0) },
  {  7, 23, (float2) (1,0) },
  { 15, 31, (float2) (1,0) },

  {  0,  8, (float2) (1,0) },
  {  4, 12, (float2) (1,0) },
  {  2, 10, (float2) (1,0) },
  {  6, 14, (float2) (1,0) },
  {  1,  9, (float2) (1,0) },
  {  5, 13, (float2) (1,0) },
  {  3, 11, (float2) (1,0) },
  {  7, 15, (float2) (1,0) },
  { 16, 24, (float2) (0,-1) },
  { 20, 28, (float2) (0,-1) },
  { 18, 26, (float2) (0,-1) },
  { 22, 30, (float2) (0,-1) },
  { 17, 25, (float2) (0,-1) },
  { 21, 29, (float2) (0,-1) },
  { 19, 27, (float2) (0,-1) },
  { 23, 31, (float2) (0,-1) },

  {  0,  4, (float2) (1,0) },
  {  2,  6, (float2) (1,0) },
  {  1,  5, (float2) (1,0) },
  {  3,  7, (float2) (1,0) },
  { 16, 20, (float2) (0.707107f,-0.707107f) },
  { 18, 22, (float2) (0.707107f,-0.707107f) },
  { 17, 21, (float2) (0.707107f,-0.707107f) },
  { 19, 23, (float2) (0.707107f,-0.707107f) },
  {  8, 12, (float2) (0,-1) },
  { 10, 14, (float2) (0,-1) },
  {  9, 13, (float2) (0,-1) },
  { 11, 15, (float2) (0,-1) },
  { 24, 28, (float2) (-0.707107f,-0.707107f) },
  { 26, 30, (float2) (-0.707107f,-0.707107f) },
  { 25, 29, (float2) (-0.707107f,-0.707107f) },
  { 27, 31, (float2) (-0.707107f,-0.707107f) },

  {  0,  2, (float2) (1,0) },
  {  1,  3, (float2) (1,0) },
  { 16, 18, (float2) (0.92388f,-0.382683f) },
  { 17, 19, (float2) (0.92388f,-0.382683f) },
  {  8, 10, (float2) (0.707107f,-0.707107f) },
  {  9, 11, (float2) (0.707107f,-0.707107f) },
  { 24, 26, (float2) (0.382683f,-0.92388f) },
  { 25, 27, (float2) (0.382683f,-0.92388f) },
  {  4,  6, (float2) (0,-1) },
  {  5,  7, (float2) (0,-1) },
  { 20, 22, (float2) (-0.382683f,-0.92388f) },
  { 21, 23, (float2) (-0.382683f,-0.92388f) },
  { 12, 14, (float2) (-0.707107f,-0.707107f) },
  { 13, 15, (float2) (-0.707107f,-0.707107f) },
  { 28, 30, (float2) (-0.92388f,-0.382683f) },
  { 29, 31, (float2) (-0.92388f,-0.382683f) },

  {  0,  1, (float2) (1,0) },
  { 16, 17, (float2) (0.980785f,-0.19509f) },
  {  8,  9, (float2) (0.92388f,-0.382683f) },
  { 24, 25, (float2) (0.83147f,-0.55557f) },
  {  4,  5, (float2) (0.707107f,-0.707107f) },
  { 20, 21, (float2) (0.55557f,-0.83147f) },
  { 12, 13, (float2) (0.382683f,-0.92388f) },
  { 28, 29, (float2) (0.19509f,-0.980785f) },
  {  2,  3, (float2) (0,-1) },
  { 18, 19, (float2) (-0.19509f,-0.980785f) },
  { 10, 11, (float2) (-0.382683f,-0.92388f) },
  { 26, 27, (float2) (-0.55557f,-0.83147f) },
  {  6,  7, (float2) (-0.707107f,-0.707107f) },
  { 22, 23, (float2) (-0.83147f,-0.55557f) },
  { 14, 15, (float2) (-0.92388f,-0.382683f) },
  { 30, 31, (float2) (-0.980785f,-0.19509f) },
};


uint5_t bit_reverse_32(uint5_t n)
{
  return ((n & 0x01) << 4) | ((n & 0x02) << 2) | ((n & 0x04)) | ((n & 0x08) >> 2) | ((n & 0x10) >> 4);
}


typedef float2 Matrix[32][32];

__kernel
__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
void do_fft_32x32()
{
#pragma max_concurrency 2
  while (true) {
    Matrix a __attribute__((doublepump));

#pragma loop_coalesce 2
    for (int n = 0; n < 32; n ++)
      for (int i = 0; i < 32; i ++)
	a[n][i] = read_channel_intel(in_channel);

#pragma unroll 1
    for (int dir = 0; dir < 2; dir ++) {
#pragma ivdep safelen(640)
      for (int idx = 0; idx < 5 * 16; idx ++) {
	struct command command = recipe[idx];

#pragma ivdep
	for (int n = 0; n < 32; n ++) {
	  float2 even   = dir == 0 ? a[n][command.even] : a[command.even][n];
	  float2 odd    = dir == 0 ? a[n][command.odd ] : a[command.odd ][n];
	  float2 weight = command.weight;
	  float2 prod   = (float2) (weight.x * odd.x + -weight.y * odd.y, weight.x * odd.y + weight.y * odd.x);
	  * (dir == 0 ? &a[n][command.even] : &a[command.even][n]) = even + prod;
	  * (dir == 0 ? &a[n][command.odd ] : &a[command.odd ][n]) = even - prod;
	}
      }
    }

#pragma loop_coalesce 2
    for (int n = 0; n < 32; n ++)
      for (int i = 0; i < 32; i ++)
	write_channel_intel(out_channel, a[bit_reverse_32(n)][bit_reverse_32(i)]);
  }
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
