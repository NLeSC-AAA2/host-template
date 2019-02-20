#pragma once
#include "fft.hh"
#include <fftw3.h>

#include <gsl/span>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <cassert> 

namespace TripleA2 {
    template <typename Config, typename InputNum, typename OutputNum=InputNum>
    using FFTImplementation
        = std::function<void
            ( Config
            , gsl::span<InputNum>
            , gsl::span<OutputNum> )>;

    enum TypeId {
        FLOAT32, FLOAT64, COMPLEX64, COMPLEX128
    };

    extern std::map<std::string, TypeId> type_map;

    using complex64 = std::complex<float>;

//    template <typename Config>
//    void complex_fft
//        ( Config cfg
//        , gsl::span<complex64> in
//        , gsl::span<complex64> out )
//    {
//        auto shape = cfg["shape"].as<std::vector<int>>();
//        size_t s = 1;
//        for (auto x : shape) s *= x;
//
//        size_t data_size = in.size();
//        assert(in.size() == out.size());
//        assert(in.size() % s == 0);
//        size_t block = in.size() / s;
//
//        fftwf_plan plan = fftwf_plan_many_dft(
//            shape.size(), shape.data(), block,
//            reinterpret_cast<fftwf_complex *>(in.data()), NULL, 1, s,
//            reinterpret_cast<fftwf_complex *>(out.data()), NULL, 1, s,
//            FFTW_FORWARD, FFTW_ESTIMATE);
//        fftwf_execute(plan);
//    }
}

struct Errors
{
    double abs, rel;
};

extern Errors validate_fft(
        shape_t const &shape,
        unsigned block,
        complex_span<float> input,
        complex_span<float> output);
