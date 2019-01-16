# host-template

Collection of host code snippets and main executable. This can be used to create testing and benchmarking executables for Intel FPGA OpenCL applications.

## Building

Prepare the environment to contain the OpenCL Altera or Intel FPGA SDK.

```
module load prun
module load prun-slurm
module load opencl-altera/18.1.0.222
```

Build this host code using `make`.

```
make
```

## Running

To run in the emulator

```
export CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1
prun ./build/host-template 1
```

