#include <doctest/doctest.h>

#include <mona/mona.hpp>
#include <sstream>
#include <string>

TEST_CASE("_symbol_ex parser") {
    SUBCASE("parses an ordinary character") {
        std::istringstream is{"abc"};

        auto parser = mona::_symbol_ex("unexpected character", '\n', '\r');

        auto result = parser(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'a');
        CHECK(is.peek() == 'b');
    }

    SUBCASE("rejects an excluded character") {
        std::istringstream is{"abc"};

        auto parser = mona::_symbol_ex("unexpected character", 'a', 'b');

        auto result = parser(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "unexpected character");
        CHECK(is.peek() == 'a');
    }

    SUBCASE("rejects newline when excluded") {
        std::istringstream is{"\nabc"};

        auto parser = mona::_symbol_ex("newline not allowed", '\n');

        auto result = parser(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "newline not allowed");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("rejects carriage return when excluded") {
        std::istringstream is{"\rabc"};

        auto parser = mona::_symbol_ex("carriage return not allowed", '\r');

        auto result = parser(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "carriage return not allowed");
        CHECK(is.peek() == '\r');
    }

    SUBCASE("accepts newline when not excluded") {
        std::istringstream is{"\nabc"};

        auto parser = mona::_symbol_ex("unexpected character", ' ');

        auto result = parser(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == '\n');
    }

    SUBCASE("rejects characters from a larger exclusion list") {
        std::istringstream is{"x"};

        auto parser = mona::_symbol_ex(
            "excluded character",
            ' ', '\t', '\n', '\r', '\f', '\v');

        auto result = parser(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'x');
    }

    SUBCASE("uses the default error message") {
        std::istringstream is{" "};

        auto parser = mona::_symbol_ex(' ');

        auto result = parser(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "unexpected token in _symbol parser");
        CHECK(is.peek() == ' ');
    }

    SUBCASE("fails on empty input") {
        std::istringstream is{""};

        auto parser = mona::_symbol_ex("unexpected EOF", '\n');

        auto result = parser(is);

        CHECK_FALSE(result.has_value());
        CHECK(result.error() == "unexpected EOF");
    }

    SUBCASE("fails on EOF after consuming all input") {
        std::istringstream is{"a"};
        is.get();

        auto parser = mona::_symbol_ex("unexpected EOF", '\n');

        auto result = parser(is);

        CHECK_FALSE(result.has_value());
        CHECK(result.error() == "unexpected EOF");
    }
}
