#include <doctest/doctest.h>

#include <mona/mona.hpp>
#include <sstream>
#include <string>

TEST_CASE("_symbols_ex parser") {
    SUBCASE("parses consecutive symbols") {
        std::istringstream is{"hello123!@"};

        auto parser = mona::_symbols_ex('\n', '\r');
        auto res = parser(is);

        REQUIRE(res.has_value());
        CHECK(res.value() == "hello123!@");
        CHECK(mona::_eof()(is).value());
    }

    SUBCASE("stops at an excluded character") {
        std::istringstream is{"hello world"};

        auto parser = mona::_symbols_ex(' ');
        auto res = parser(is);

        REQUIRE(res.has_value());
        CHECK(res.value() == "hello");
        CHECK(is.peek() == ' ');
    }

    SUBCASE("stops at newline") {
        std::istringstream is{"hello\nworld"};

        auto parser = mona::_symbols_ex('\n');
        auto res = parser(is);

        REQUIRE(res.has_value());
        CHECK(res.value() == "hello");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("supports multiple excluded characters") {
        std::istringstream is{"hello\tworld\n"};

        auto parser = mona::_symbols_ex(' ', '\t', '\n', '\r');
        auto res = parser(is);

        REQUIRE(res.has_value());
        CHECK(res.value() == "hello");
        CHECK(is.peek() == '\t');
    }

    SUBCASE("allows characters not in the exclusion list") {
        std::istringstream is{"./../././///..//test./../"};

        auto parser = mona::_symbols_ex(' ', '\t', '\n', '\r');
        auto res = parser(is);

        REQUIRE(res.has_value());
        CHECK(res.value() == "./../././///..//test./../");
        CHECK(mona::_eof()(is).value());
    }

    SUBCASE("uses custom error message") {
        std::istringstream is{" "};

        auto parser = mona::_symbols_ex("custom error", ' ');
        auto res = parser(is);

        REQUIRE_FALSE(res.has_value());
        CHECK(res.error() == "custom error");
        CHECK(is.peek() == ' ');
    }

    SUBCASE("uses default error message") {
        std::istringstream is{"\n"};

        auto parser = mona::_symbols_ex('\n');
        auto res = parser(is);

        REQUIRE_FALSE(res.has_value());
        CHECK(res.error() == "_symbols parser error");
        CHECK(is.peek() == '\n');
    }

    SUBCASE("fails on empty input") {
        std::istringstream is{""};

        auto parser = mona::_symbols_ex('\n');
        auto res = parser(is);

        REQUIRE_FALSE(res.has_value());
        CHECK(res.error() == "_symbols parser error");
    }

    SUBCASE("restores stream position when first character is excluded") {
        std::istringstream is{"\tremaining"};

        auto parser = mona::_symbols_ex('\t');
        auto res = parser(is);

        REQUIRE_FALSE(res.has_value());
        CHECK(is.peek() == '\t');
    }

    SUBCASE("preserves custom error when first character is excluded") {
        std::istringstream is{"\rremaining"};

        auto parser = mona::_symbols_ex("invalid input", '\r');
        auto res = parser(is);

        REQUIRE_FALSE(res.has_value());
        CHECK(res.error() == "invalid input");
        CHECK(is.peek() == '\r');
    }
}