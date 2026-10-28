#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("many_or_one passes") {
    std::stringstream ss;
    ss << "012ab";
    auto result = mona::_many_or_one(mona::_digit())(ss);
    CHECK(result.value().size() == 3);
    for (int i = 0; i < result.value().size(); i++) {
        CHECK(result.value()[i] == i);
    }
    CHECK(ss.peek() == 'a');
};

TEST_CASE("many_or_one fails") {
    std::stringstream ss;
    ss << "012ab";
    auto result = mona::_many_or_one(mona::_letter())(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '0');
};
