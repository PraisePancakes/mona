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
};

TEST_CASE("match passes on string") {
    std::stringstream ss;
    ss << "hi there";
    auto result = mona::_match("hi there")(ss);
    CHECK(result.has_value());
    CHECK(result.value() == "hi there");
    CHECK(ss.peek() == std::char_traits<char>::eof());
};

TEST_CASE("match passes with partial string matching") {
    std::stringstream ss;
    ss << "hi there";
    auto result = mona::_match("hi ")(ss);
    CHECK(result.has_value());
    CHECK(result.value() == "hi ");
    CHECK(ss.peek() == 't');
}
