#pragma once

#include "common.hh"

#include "binaries.hh"
#include "contexts.hh"
#include "errors.hh"

#undef CL_MEM_HOST_READ_ONLY  // 17.1 does not accept them, at least not in the emulator
#define CL_MEM_HOST_READ_ONLY   0
#undef CL_MEM_HOST_WRITE_ONLY
#define CL_MEM_HOST_WRITE_ONLY  0
