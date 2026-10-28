#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("match passes on a different character") {
    std::stringstream ss("a");

    auto result = mona::_match('a')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match passes on digit") {
    std::stringstream ss("7");

    auto result = mona::_match('7')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == '7');
}

TEST_CASE("match passes on punctuation") {
    std::stringstream ss("!");

    auto result = mona::_match('!')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == '!');
}

TEST_CASE("match passes on whitespace") {
    std::stringstream ss(" ");

    auto result = mona::_match(' ')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == ' ');
}

TEST_CASE("match passes on newline") {
    std::stringstream ss("\n");

    auto result = mona::_match('\n')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == '\n');
}

TEST_CASE("match consumes only one character") {
    std::stringstream ss("abc");

    auto result = mona::_match('a')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
}

TEST_CASE("match fails on different character") {
    std::stringstream ss("1");

    auto result = mona::_match('a')(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("match fails on empty input") {
    std::stringstream ss("");

    auto result = mona::_match('a')(ss);

    CHECK_FALSE(result.has_value());
}

TEST_CASE("match fails when expected character is uppercase") {
    std::stringstream ss("a");

    auto result = mona::_match('A')(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("match fails when expected character is lowercase") {
    std::stringstream ss("A");

    auto result = mona::_match('a')(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'A');
}

TEST_CASE("match fails without consuming input") {
    std::stringstream ss("abc");

    auto result = mona::_match('x')(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("match returns custom error on character mismatch") {
    std::stringstream ss("1");
    const std::string error = "expected letter";

    auto result = mona::_match('a', error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == '1');
}

TEST_CASE("match preserves empty custom error") {
    std::stringstream ss("1");
    const std::string error = "";

    auto result = mona::_match('a', error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
}

TEST_CASE("match returns custom error on empty input") {
    std::stringstream ss("");
    const std::string error = "expected a character";

    auto result = mona::_match('a', error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
}

TEST_CASE("match passes on a single-character string") {
    std::stringstream ss("a");

    auto result = mona::_match("a")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "a");
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match passes on an empty string") {
    std::stringstream ss("abc");

    auto result = mona::_match("")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("match passes on string at beginning of input") {
    std::stringstream ss("hello world!");

    auto result = mona::_match("hello")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "hello");
    CHECK(ss.peek() == ' ');
}

TEST_CASE("match passes on entire input") {
    std::stringstream ss("hello");

    auto result = mona::_match("hello")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "hello");
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match preserves whitespace in string") {
    std::stringstream ss("hello   world!");

    auto result = mona::_match("hello   world")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "hello   world");
    CHECK(ss.peek() == '!');
}

TEST_CASE("match preserves punctuation in string") {
    std::stringstream ss("a=b+c;");

    auto result = mona::_match("a=b+c;")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "a=b+c;");
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match fails when first character differs") {
    std::stringstream ss("hello");

    auto result = mona::_match("world")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match fails when last character differs") {
    std::stringstream ss("hellx");

    auto result = mona::_match("hello")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match fails when input is shorter than expected string") {
    std::stringstream ss("hel");

    auto result = mona::_match("hello")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match fails when input has a partial prefix") {
    std::stringstream ss("hell");

    auto result = mona::_match("hello")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match fails when whitespace differs") {
    std::stringstream ss("hello world");

    auto result = mona::_match("hello  world")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match fails on empty input for nonempty string") {
    std::stringstream ss("");

    auto result = mona::_match("hello")(ss);

    CHECK_FALSE(result.has_value());
}

TEST_CASE("match returns custom error on string mismatch") {
    std::stringstream ss("hello");
    const std::string error = "expected world";

    auto result = mona::_match("world", error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match returns custom error on partial string mismatch") {
    std::stringstream ss("helx");
    const std::string error = "expected hello";

    auto result = mona::_match("hello", error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == 'h');
}

TEST_CASE("match returns custom error when input ends early") {
    std::stringstream ss("hel");
    const std::string error = "incomplete input";

    auto result = mona::_match("hello", error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
}

TEST_CASE("match character from current stream position") {
    std::stringstream ss("xyzabc");

    ss.get();
    ss.get();
    ss.get();

    auto result = mona::_match('a')(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
}

TEST_CASE("match string from current stream position") {
    std::stringstream ss("prefix hello suffix");

    ss.seekg(7);

    auto result = mona::_match("hello")(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == "hello");
    CHECK(ss.peek() == ' ');
}

TEST_CASE("match failure preserves nonzero starting position") {
    std::stringstream ss("xyzabc");

    ss.seekg(3);

    auto result = mona::_match("xyz")(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("match works with digit parser in sequence") {
    std::stringstream ss("1.");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_match('.'))(ss);

    REQUIRE(result.has_value());

    auto [digit, dot] = result.value();

    CHECK(digit == 1);
    CHECK(dot == '.');
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match works with letter parser in sequence") {
    std::stringstream ss("name=value");

    auto result = mona::_sequence(
        mona::_letter(),
        mona::_letter(),
        mona::_letter(),
        mona::_letter(),
        mona::_match('='),
        mona::_letter(),
        mona::_letter(),
        mona::_letter(),
        mona::_letter(),
        mona::_letter())(ss);

    REQUIRE(result.has_value());
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("match failure rolls back entire sequence") {
    std::stringstream ss("1x");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_match('.'))(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}