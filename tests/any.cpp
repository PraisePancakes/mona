#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("_any passes on first parser") {
    std::stringstream ss("a123");

    auto result = mona::_any(
        "expected a digit or character",
        mona::_match('a'),
        mona::_match('b'))(ss);

    CHECK(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == '1');
}

TEST_CASE("_any passes on second parser") {
    std::stringstream ss("b123");

    auto result = mona::_any(
        "expected a digit or character",
        mona::_match('a'),
        mona::_match('b'))(ss);

    CHECK(result.has_value());
    CHECK(result.value() == 'b');
    CHECK(ss.peek() == '1');
}

TEST_CASE("_any fails with custom error") {
    std::stringstream ss("c123");

    const std::string error = "expected a or b";

    auto result = mona::_any(
        error,
        mona::_match('a'),
        mona::_match('b'))(ss);

    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == 'c');
}

TEST_CASE("_any works with three parsers") {
    std::stringstream ss("c123");

    auto result = mona::_any(
        "expected a, b, or c",
        mona::_match('a'),
        mona::_match('b'),
        mona::_match('c'))(ss);

    CHECK(result.has_value());
    CHECK(result.value() == 'c');
    CHECK(ss.peek() == '1');
}

TEST_CASE("_any works with many_or_one") {
    std::stringstream ss("aabb123");

    auto result = mona::_many_or_one(
        mona::_any(
            "expected a or b",
            mona::_match('a'),
            mona::_match('b')))(ss);

    CHECK(result.has_value());
    CHECK(result.value().size() == 4);
    CHECK(result.value()[0] == 'a');
    CHECK(result.value()[1] == 'a');
    CHECK(result.value()[2] == 'b');
    CHECK(result.value()[3] == 'b');
    CHECK(ss.peek() == '1');
}

TEST_CASE("_any replaces underlying parser errors") {
    std::stringstream ss("x");

    const std::string error = "custom any error";

    auto result = mona::_any(
        error,
        mona::_match('a'),
        mona::_match('b'))(ss);

    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}