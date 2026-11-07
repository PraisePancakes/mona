#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"
using namespace mona;
TEST_CASE("_letters parser") {
    SUBCASE("parses consecutive letters") {
        std::istringstream is{"hello"};
        auto result = _letters()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(is.peek() == std::char_traits<char>::eof());
    }

    SUBCASE("stops at the first non-letter") {
        std::istringstream is{"hello123world"};
        auto result = _letters()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(is.peek() == '1');
    }

    SUBCASE("stops at whitespace") {
        std::istringstream is{"hello world"};
        auto result = _letters()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(is.peek() == ' ');
    }

    SUBCASE("fails when input starts with a non-letter") {
        std::istringstream is{"123hello"};
        auto result = _letters()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "_letters parser error");
        CHECK(is.peek() == '1');
    }

    SUBCASE("fails on empty input") {
        std::istringstream is{""};
        auto result = _letters()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "_letters parser error");
    }

    SUBCASE("uses custom error message") {
        std::istringstream is{"123"};
        auto result = _letters("expected letters")(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "expected letters");
        CHECK(is.peek() == '1');
    }
}