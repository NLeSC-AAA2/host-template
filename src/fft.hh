#pragma once
#include <gsl/span>
#include <string>
#include <complex>
#include <vector>

extern void randomize_data(gsl::span<std::complex<float>> data);
extern void fft_test(std::string const &filename, unsigned fft_size, unsigned block_size, unsigned repeats);

template <typename T>
using complex_span = gsl::span<std::complex<T>>;
using shape_t = std::vector<int>;

extern bool validate_fft(
        shape_t const &shape,
        unsigned block,
        complex_span<float> input,
        complex_span<float> output,
        float epsrel);

