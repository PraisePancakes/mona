
#include <mona/mona.hpp>
#include <sstream>
#include <string>

#include "doctest.h"

TEST_CASE("_any accepts matching parsers") {
    SUBCASE("matches the first parser") {
        std::stringstream ss("a");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_match('b'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'a');
    }

    SUBCASE("matches the second parser") {
        std::stringstream ss("b");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_match('b'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'b');
    }

    SUBCASE("matches the last parser") {
        std::stringstream ss("c");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_match('b'),
            mona::_match('c'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'c');
    }
}

TEST_CASE("_any respects parser ordering") {
    SUBCASE("returns the first successful result") {
        std::stringstream ss("a");

        auto result = mona::_any(
            mona::_letter(),
            mona::_match('a'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'a');
    }

    SUBCASE("does not try later parsers after success") {
        std::stringstream ss("a");

        auto first = mona::_match('a');
        auto second = mona::_match('a');

        auto result = mona::_any(first, second)(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'a');
        CHECK(ss.peek() == std::char_traits<char>::eof());
    }
}

TEST_CASE("_any fails when no parser matches") {
    SUBCASE("returns the last parser error") {
        std::stringstream ss("x");

        auto result = mona::_any(
            mona::_match('a', "expected a"),
            mona::_match('b', "expected b"),
            mona::_match('c', "expected c"))(ss);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "expected c");
    }

    SUBCASE("fails with a single parser") {
        std::stringstream ss("x");

        auto result = mona::_any(
            mona::_match('a', "expected a"))(ss);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "expected a");
    }
}

TEST_CASE("_any supports custom errors") {
    SUBCASE("returns the custom error on total failure") {
        std::stringstream ss("x");

        auto result = mona::_any(
            "no alternatives matched",
            mona::_match('a', "expected a"),
            mona::_match('b', "expected b"))(ss);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == "no alternatives matched");
    }

    SUBCASE("preserves the successful result") {
        std::stringstream ss("b");

        auto result = mona::_any(
            "no alternatives matched",
            mona::_match('a'),
            mona::_match('b'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'b');
    }
}

TEST_CASE("_any restores input after failed alternatives") {
    SUBCASE("later parser can match after an earlier failure") {
        std::stringstream ss("b");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_match('b'))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'b');
        CHECK(ss.peek() == std::char_traits<char>::eof());
    }

    SUBCASE("input remains available after total failure") {
        std::stringstream ss("x");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_match('b'))(ss);

        REQUIRE_FALSE(result.has_value());

        ss.clear();
        CHECK(ss.peek() == 'x');
    }
}

TEST_CASE("_any works with different parser implementations") {
    SUBCASE("letter alternatives") {
        std::stringstream ss("z");

        auto result = mona::_any(
            mona::_match('a'),
            mona::_letter())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'z');
    }

    SUBCASE("digit alternatives") {
        std::stringstream ss("7");

        auto result = mona::_any(
            mona::_match('0'),
            mona::_digit())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 7);
    }
}
