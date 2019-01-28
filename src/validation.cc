#include "fft.hh"
#include <array>
#include <gsl/span>
#include <fftw3.h>
#include <iostream>

float validate_fft(
        shape_t const &shape,
        unsigned block,
        complex_span<float> input,
        complex_span<float> output)
{
    size_t s = 1;
    for (auto x : shape) s *= x;
    std::vector<std::complex<float>> ground_truth(s * block);

    fftwf_plan plan = fftwf_plan_many_dft(
        shape.size(), shape.data(), block,
        reinterpret_cast<fftwf_complex *>(input.data()), NULL, 1, s,
        reinterpret_cast<fftwf_complex *>(ground_truth.data()), NULL, 1, s,
        FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);

    double max_diff = 0.0;
    for (size_t i = 0; i < s*block; ++i) {
        double diff = std::abs(ground_truth[i] - output[i]) / std::abs(ground_truth[i]);
        max_diff = (diff > max_diff ? diff : max_diff);
    }
    return max_diff;
}

