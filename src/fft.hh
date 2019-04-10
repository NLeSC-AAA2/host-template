#pragma once
#include <gsl/span>
#include <string>
#include <complex>
#include <vector>

extern void fft_test(std::string const &filename, unsigned fft_size, unsigned block_size, unsigned repeats);

template <typename T>
using complex_span = gsl::span<std::complex<T>>;
using shape_t = std::vector<int>;
