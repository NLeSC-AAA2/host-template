#pragma once
#include <cmath>
#include <vector>

inline std::pair<size_t,size_t> median_indices(size_t count)
{
    if (count % 2 == 0) return { (count/2) - 1, count/2 };
    return { count/2, count/2 };
}

template<typename T>
struct StatisticalSummary
{
public:
    template <typename V>
    StatisticalSummary(const std::vector<V>& values)
        : StatisticalSummary(std::vector<V>(values))
    {}

    template <typename V>
    StatisticalSummary(std::vector<V>&& values)
    {
        std::pair<size_t,size_t> medianPair, lower, upper;

        size_t half = values.size()/2;

        if (values.empty()) {
            throw std::domain_error("Empty vector has no summary!");
        } else if (values.size() > 1) {
            medianPair = median_indices(values.size());
            lower = median_indices(half);
            upper = lower;

            upper.first += half + (values.size() % 2);
            upper.second += half + (values.size() % 2);
        } else {
            medianPair = lower = upper = {0, 0};
        }

        std::sort(values.begin(), values.end());

        double total = 0;
        double M = 0.0;
        double S = 0.0;
        int k = 1;
        for (auto val : values) {
            double v = val.count(); // static_cast<double>(val);
            total += v;
            double tmpM = M;
            M += (v - tmpM) / k;
            S += (v - tmpM) * (v - M);
            k++;
        }

        min = values.front();
        lowerQuantile = (values[lower.first] + values[lower.second])/2.0;
        median = (values[medianPair.first] + values[medianPair.second])/2.0;
        mean = total / values.size();
        upperQuantile = (values[upper.first] + values[upper.second])/2.0;
        max = values.back();
        stdDev = sqrt(S / (k-1));
    }

    T min;
    T lowerQuantile;
    T median;
    T mean;
    T upperQuantile;
    T max;
    T stdDev;
};
