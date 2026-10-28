#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("digit parses zero") {
    std::stringstream ss("0");

    auto result = mona::_digit()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 0);
}

TEST_CASE("digit parses nine") {
    std::stringstream ss("9");

    auto result = mona::_digit()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 9);
}

TEST_CASE("digit parses digits from the middle of input") {
    std::stringstream ss("x5y");

    ss.get();  // Consume 'x'

    auto result = mona::_digit()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 5);
    CHECK(ss.peek() == 'y');
}

TEST_CASE("digit parses only one character") {
    std::stringstream ss("123");

    auto result = mona::_digit()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 1);
    CHECK(ss.peek() == '2');
}

TEST_CASE("digit parses a digit followed by punctuation") {
    std::stringstream ss("7!");

    auto result = mona::_digit()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 7);
    CHECK(ss.peek() == '!');
}

TEST_CASE("digit fails on alphabetic characters") {
    std::stringstream ss("z");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'z');
}

TEST_CASE("digit fails on punctuation") {
    std::stringstream ss(".");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '.');
}

TEST_CASE("digit fails on whitespace") {
    std::stringstream ss(" ");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == ' ');
}

TEST_CASE("digit fails on a negative sign") {
    std::stringstream ss("-");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '-');
}

TEST_CASE("digit fails on a plus sign") {
    std::stringstream ss("+");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '+');
}

TEST_CASE("digit fails on empty input and sets stream failure") {
    std::stringstream ss("");

    auto result = mona::_digit()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.fail());
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("digit fails on empty input with custom error") {
    std::stringstream ss("");
    const std::string error = "expected a digit";

    auto result = mona::_digit(error)(ss);

    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}

TEST_CASE("digit fails at end of input after consuming previous character") {
    std::stringstream ss("a");
    ss.get();
    auto result = mona::_digit()(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("digit parses all decimal digits correctly") {
    for (int i = 0; i <= 9; ++i) {
        std::stringstream ss;
        ss << static_cast<char>('0' + i);

        auto result = mona::_digit()(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == i);
    }
}

TEST_CASE("digit preserves custom error message on alphabetic input") {
    std::stringstream ss("a");
    const std::string error = "expected an ASCII digit";

    auto result = mona::_digit(error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("digit preserves empty custom error message") {
    std::stringstream ss("a");
    const std::string error = "";

    auto result = mona::_digit(error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("digit can parse consecutive digits") {
    std::stringstream ss("123");

    auto first = mona::_digit()(ss);
    auto second = mona::_digit()(ss);
    auto third = mona::_digit()(ss);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());
    REQUIRE(third.has_value());

    CHECK(first.value() == 1);
    CHECK(second.value() == 2);
    CHECK(third.value() == 3);
}

TEST_CASE("digit failure does not consume input on repeated parsing") {
    std::stringstream ss("1a");

    auto first = mona::_digit()(ss);

    REQUIRE(first.has_value());
    CHECK(first.value() == 1);

    auto second = mona::_digit()(ss);

    CHECK_FALSE(second.has_value());
    CHECK(ss.peek() == 'a');
}