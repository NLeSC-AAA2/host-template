#pragma once
#include "fft.hh"

struct Errors
{
    double abs, rel;
};

extern Errors validate_fft(
        shape_t const &shape,
        unsigned block,
        complex_span<float> input,
        complex_span<float> output);
