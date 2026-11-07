#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"
using namespace mona;
TEST_CASE("_eof parser") {
    SUBCASE("succeeds on empty input") {
        std::istringstream is{""};
        auto result = _eof()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == true);
    }

    SUBCASE("succeeds when positioned at EOF") {
        std::istringstream is{"hello"};
        is.seekg(5);

        auto result = _eof()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == true);
    }

    SUBCASE("fails when input remains") {
        std::istringstream is{"hello"};
        auto result = _eof()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "not eof");
    }

    SUBCASE("does not consume input") {
        std::istringstream is{"hello"};
        auto result = _eof()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(is.peek() == 'h');
    }

    SUBCASE("detects EOF after consuming all input") {
        std::istringstream is{"abc"};

        is.get();
        is.get();
        is.get();

        auto result = _eof()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == true);
    }

    SUBCASE("fails when positioned before the end") {
        std::istringstream is{"abc"};
        is.get();

        auto result = _eof()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "not eof");
        CHECK(is.peek() == 'b');
    }
}