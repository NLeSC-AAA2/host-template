#pragma once
#include <argagg.hpp>
#include <sstream>

namespace argagg {
namespace convert {
    template <>
    std::vector<int> arg(char const *x) {
        std::istringstream ss(x);
        std::vector<int> result;
        while (!ss.eof()) {
            int i;
            ss >> i;
            result.push_back(i);
            ss.get();
        }
        return result;
    }
}} // namespace argagg::convert

