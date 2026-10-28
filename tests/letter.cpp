#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("letter parses lowercase letter") {
    std::stringstream ss("a");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
}

TEST_CASE("letter parses uppercase letter") {
    std::stringstream ss("Z");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'Z');
}

TEST_CASE("letter parses first lowercase letter") {
    std::stringstream ss("abc");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
}

TEST_CASE("letter parses last lowercase letter") {
    std::stringstream ss("z");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'z');
}

TEST_CASE("letter parses first uppercase letter") {
    std::stringstream ss("A");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'A');
}

TEST_CASE("letter parses last uppercase letter") {
    std::stringstream ss("Z");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'Z');
}

TEST_CASE("letter consumes only one character") {
    std::stringstream ss("abc");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
}

TEST_CASE("letter leaves following digit untouched") {
    std::stringstream ss("a1");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == '1');
}

TEST_CASE("letter leaves following punctuation untouched") {
    std::stringstream ss("a!");

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == '!');
}

TEST_CASE("letter parses from current stream position") {
    std::stringstream ss("1ab");

    ss.get();  // Consume '1'

    auto result = mona::_letter()(ss);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(ss.peek() == 'b');
}

TEST_CASE("letter fails on digit") {
    std::stringstream ss("1");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("letter fails on punctuation") {
    std::stringstream ss("!");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '!');
}

TEST_CASE("letter fails on whitespace") {
    std::stringstream ss(" ");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == ' ');
}

TEST_CASE("letter fails on newline") {
    std::stringstream ss("\n");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '\n');
}

TEST_CASE("letter fails on underscore") {
    std::stringstream ss("_");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '_');
}

TEST_CASE("letter fails on mixed input starting with digit") {
    std::stringstream ss("1abc");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("letter fails on empty input") {
    std::stringstream ss("");

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.fail());
}

TEST_CASE("letter fails at end of stream") {
    std::stringstream ss("a");

    ss.get();  // Consume the only character

    auto result = mona::_letter()(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.fail());
}

TEST_CASE("letter fails on empty input with custom error") {
    std::stringstream ss("");
    const std::string error = "expected a letter";

    auto result = mona::_letter(error)(ss);

    CHECK_FALSE(result.has_value());
    CHECK(result.error() == error);
}

TEST_CASE("letter preserves custom error on digit input") {
    std::stringstream ss("7");
    const std::string error = "expected a letter";

    auto result = mona::_letter(error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == '7');
}

TEST_CASE("letter preserves custom error on punctuation") {
    std::stringstream ss("@");
    const std::string error = "invalid identifier";

    auto result = mona::_letter(error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == '@');
}

TEST_CASE("letter preserves empty custom error") {
    std::stringstream ss("1");
    const std::string error = "";

    auto result = mona::_letter(error)(ss);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == error);
    CHECK(ss.peek() == '1');
}

TEST_CASE("letter parses consecutive lowercase letters") {
    std::stringstream ss("abc");

    auto first = mona::_letter()(ss);
    auto second = mona::_letter()(ss);
    auto third = mona::_letter()(ss);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());
    REQUIRE(third.has_value());

    CHECK(first.value() == 'a');
    CHECK(second.value() == 'b');
    CHECK(third.value() == 'c');
}

TEST_CASE("letter parses consecutive uppercase letters") {
    std::stringstream ss("XYZ");

    auto first = mona::_letter()(ss);
    auto second = mona::_letter()(ss);
    auto third = mona::_letter()(ss);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());
    REQUIRE(third.has_value());

    CHECK(first.value() == 'X');
    CHECK(second.value() == 'Y');
    CHECK(third.value() == 'Z');
}

TEST_CASE("letter fails after parsing letters before a digit") {
    std::stringstream ss("ab1");

    auto first = mona::_letter()(ss);
    auto second = mona::_letter()(ss);
    auto third = mona::_letter()(ss);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());
    CHECK(first.value() == 'a');
    CHECK(second.value() == 'b');

    CHECK_FALSE(third.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("letter parses all lowercase ASCII letters") {
    for (char c = 'a'; c <= 'z'; ++c) {
        std::stringstream ss;
        ss << c;

        auto result = mona::_letter()(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == c);
    }
}

TEST_CASE("letter parses all uppercase ASCII letters") {
    for (char c = 'A'; c <= 'Z'; ++c) {
        std::stringstream ss;
        ss << c;

        auto result = mona::_letter()(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == c);
    }
}

TEST_CASE("letter works after digit in sequence") {
    std::stringstream ss("1a");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    REQUIRE(result.has_value());

    auto [digit, letter] = result.value();

    CHECK(digit == 1);
    CHECK(letter == 'a');
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("letter failure in sequence preserves original input") {
    std::stringstream ss("1!");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}