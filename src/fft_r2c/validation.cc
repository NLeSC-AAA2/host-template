#include <array>
#include <complex>
#include <gsl/span>
#include <fftw3.h>
#include <iostream>
#include <numeric>

#include "validation.hh"

using namespace TripleA2;


// Register<FFTImplementation<Config, complex64, complex64>>
//     ("fft", complex_fft);

Errors validate_fft_r2c
    ( shape_t const &shape
    , unsigned block
    , gsl::span<short> input
    , complex_span<float> output )
{
    size_t s = 1;
    for (auto x : shape) s *= x;

    std::vector<std::complex<float>> ground_truth((s/2) * block);

    std::vector<float> input_data(input.begin(), input.end());

    //std::cout << input_data.size() << " " << ground_truth.size() << std::endl;
    //for (auto i = input_data.begin(); i != input_data.end(); ++i)
    //    std::cout << *i << ' ';
    //std::cout << std::endl;

    fftwf_plan plan = fftwf_plan_many_dft_r2c(shape.size(), shape.data(), block,
                          input_data.data(), NULL, 1, s,
                          reinterpret_cast<fftwf_complex *>(ground_truth.data()), NULL, 1, s/2,
                          FFTW_ESTIMATE);

    fftwf_execute(plan);

    Errors max = {0.0, 0.0};
    for (size_t i = 0; i < (s/2)*block; ++i) {
        //std::cout << i << ground_truth[i] << " " << output[i] << std::endl;

        double abs_err = std::abs(ground_truth[i] - output[i]);
        double rel_err = abs_err / std::abs(ground_truth[i]);
        max.abs = std::max(abs_err, max.abs);
        max.rel = std::max(rel_err, max.rel);

        //std::cout << i << " " << ground_truth[i] << " " << output[i] << "\t" << abs_err << "\t" << rel_err << std::endl;

    }
    return max;
}

