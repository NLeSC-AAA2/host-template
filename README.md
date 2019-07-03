# host-template

Collection of host code snippets and main executable. This can be used to create testing and benchmarking executables for Intel FPGA OpenCL applications.

## Building

Prepare the environment to contain the OpenCL Altera or Intel FPGA SDK.

```
module load prun
module load prun-slurm
module load opencl-altera/latest
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

To run on FPGA

```
prun -native "-p fat20 -w node504" -np 1 -t 1:00:00 build/host-template
```

Compile an OpenCL kernel for emulation: use `Makefile` in `device` folder.
