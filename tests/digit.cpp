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

TEST_CASE("digit fails on non-digit input") {
    std::stringstream ss;
    ss << "a";
    auto result = mona::_digit()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
};

TEST_CASE("digit fails on empty input") {
    std::stringstream ss("");
    auto result = mona::_digit()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.fail());
}

TEST_CASE("digit fails with passed error") {
    std::stringstream ss("");
    const std::string error = "error parsing digit";
    auto result = mona::_digit(error)(ss);
    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}