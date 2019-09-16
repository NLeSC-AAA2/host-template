#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "arg-vector.hh"

using namespace testing;

TEST (ArgAgg, ParseVector)
{
    using argagg::convert::arg;
    auto x = arg<std::vector<int>>("32,12,24");
    EXPECT_THAT(x, ElementsAre(32, 12, 24));
    auto y = arg<std::vector<int>>("20x15");
    EXPECT_THAT(y, ElementsAre(20, 15));
}
