#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"
using namespace mona;
TEST_CASE("_symbol parser") {
    SUBCASE("parses ordinary characters") {
        std::istringstream is{"a"};
        auto result = _symbol()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'a');
    }

    SUBCASE("parses punctuation") {
        std::istringstream is{"!@#$"};
        auto result = _symbol()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == '!');
        CHECK(is.peek() == '@');
    }

    SUBCASE("parses whitespace other than line endings") {
        std::istringstream is{" \t"};
        auto result = _symbol()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == ' ');
        CHECK(is.peek() == '\t');
    }

    SUBCASE("rejects newline") {
        std::istringstream is{"\n"};
        auto result = _symbol()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "unexpected token in _symbol parser");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("rejects carriage return") {
        std::istringstream is{"\r"};
        auto result = _symbol()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "unexpected token in _symbol parser");
        CHECK(is.peek() == '\r');
    }

    SUBCASE("uses custom error message") {
        std::istringstream is{"\n"};
        auto result = _symbol("expected symbol")(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "expected symbol");
    }
}

TEST_CASE("_symbols parser") {
    SUBCASE("parses consecutive symbols") {
        std::istringstream is{"!@#$"};
        auto result = _symbols()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "!@#$");
        CHECK(is.eof());
    }

    SUBCASE("stops at newline") {
        std::istringstream is{"hello\nworld"};
        auto result = _symbols()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("stops at carriage return") {
        std::istringstream is{"hello\rworld"};
        auto result = _symbols()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(is.peek() == '\r');
    }

    SUBCASE("accepts whitespace") {
        std::istringstream is{"hello world\t!"};
        auto result = _symbols()(is);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello world\t!");
        CHECK(is.eof());
    }

    SUBCASE("fails on newline at beginning") {
        std::istringstream is{"\nhello"};
        auto result = _symbols()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "_symbols parser error");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("fails on carriage return at beginning") {
        std::istringstream is{"\rhello"};
        auto result = _symbols()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "_symbols parser error");
        CHECK(is.peek() == '\r');
    }

    SUBCASE("fails on empty input") {
        std::istringstream is{""};
        auto result = _symbols()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "_symbols parser error");
    }

    SUBCASE("uses custom error message") {
        std::istringstream is{"\n"};
        auto result = _symbols("expected symbols")(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "expected symbols");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("preserves position when parsing fails") {
        std::istringstream is{"\nhello"};
        auto starting_pos = is.tellg();

        auto result = _symbols()(is);

        REQUIRE_FALSE(result.has_value());
        CHECK(is.tellg() == starting_pos);
    }
}