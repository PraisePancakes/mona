#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("letter passes") {
    std::stringstream ss;
    ss << "ab";
    auto result = mona::_letter()(ss);
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
};

TEST_CASE("letter fails on non-letter input") {
    std::stringstream ss;
    ss << "1";
    auto result = mona::_letter()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
};

TEST_CASE("letter fails on empty input") {
    std::stringstream ss("");
    auto result = mona::_letter()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.fail());
}

TEST_CASE("letter fails with passed error") {
    std::stringstream ss("");
    const std::string error = "error parsing digit";
    auto result = mona::_letter(error)(ss);
    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}