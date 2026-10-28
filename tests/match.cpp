#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("match passes") {
    std::stringstream ss;
    ss << "1";
    auto result = mona::_match('1')(ss);
    CHECK(result.value() == '1');
};

TEST_CASE("match fails") {
    std::stringstream ss;
    ss << "1";
    auto result = mona::_match('a')(ss);
    CHECK_FALSE(result.has_value());
};

TEST_CASE("match fails with error") {
    std::stringstream ss;
    ss << "1";
    std::string error = "error, tried to match 'a' but got '1' instead";
    auto result = mona::_match('a', error)(ss);
    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}