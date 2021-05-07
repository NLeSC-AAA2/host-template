#include <array>
#include <complex>
#include <gsl/span>
#include <fftw3.h>
#include <iostream>
#include <numeric>

#include "validation.hh"

using namespace TripleA2;

std::map<std::string, TripleA2::TypeId> TripleA2::type_map = 
        { {"float32", FLOAT32}
        , {"float64", FLOAT64}
        , {"complex64", COMPLEX64}
        , {"complex128", COMPLEX128}
        };


// Register<FFTImplementation<Config, complex64, complex64>>
//     ("fft", complex_fft);

Errors validate_fft
    ( shape_t const &shape
    , unsigned block
    , unsigned veclen
    , complex_span<float> input
    , complex_span<float> output )
{
    size_t s = veclen;
    for (auto x : shape) s *= x;
    std::vector<std::complex<float>> ground_truth(s * block);

    fftwf_plan plan = fftwf_plan_many_dft(
        shape.size(), shape.data(), block,
        reinterpret_cast<fftwf_complex *>(input.data()), NULL, veclen, s,
        reinterpret_cast<fftwf_complex *>(ground_truth.data()), NULL, veclen, s,
        FFTW_FORWARD, FFTW_ESTIMATE);

    for (unsigned k = 0; k < veclen; ++k) {
        fftwf_execute_dft(
            plan,
            reinterpret_cast<fftwf_complex*>(input.data()) + k,
            reinterpret_cast<fftwf_complex*>(ground_truth.data()) + k);
    }

    Errors max = {0.0, 0.0};
    for (size_t i = 0; i < ground_truth.size(); ++i) {
        double abs_err = std::abs(ground_truth[i] - output[i]);
        double rel_err = abs_err / std::abs(ground_truth[i]);
        max.abs = std::max(abs_err, max.abs);
        max.rel = std::max(rel_err, max.rel);
    }
    return max;
}

