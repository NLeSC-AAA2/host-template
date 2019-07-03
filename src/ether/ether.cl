#pragma OPENCL EXTENSION cl_intel_channels : enable

#include <ihc_apint.h>

//channel ulong4 udp_chan;

__attribute__((max_global_work_dim(0)))
__kernel void
sendUDP(__global const ulong4 *packetChunks, unsigned chunkCount)
{
  for (unsigned i = 0; i < chunkCount; i ++)
    ;//write_channel_intel(udp_chan, packetChunks[i]);
}
