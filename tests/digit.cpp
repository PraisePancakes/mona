#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("digit passes") {
    std::stringstream ss;
    ss << "1";
    auto result = mona::_digit()(ss);
    CHECK(result.value() == 1);
};

TEST_CASE("digit fails") {
    std::stringstream ss;
    ss << "a";
    auto result = mona::_digit()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
};