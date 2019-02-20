#include <gtest/gtest.h>
#include "register.hh"

using TripleA2::Register;

Register<int> answer("The Answer", 42);
Register<std::string> hello("Message", "Hello, World!");

TEST(Register, Register) {
    EXPECT_EQ(Register<int>::get("The Answer"), 42);
    EXPECT_EQ(Register<std::string>::get("Message"), "Hello, World!");
}

