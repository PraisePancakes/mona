#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("many_or_none parses a single digit") {
    std::stringstream ss("7");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 1);
    CHECK(result.value()[0] == 7);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none parses all digits") {
    std::stringstream ss("0123456789");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 10);

    for (int i = 0; i < 10; ++i) {
        CHECK(result.value()[i] == i);
    }

    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none parses consecutive letters") {
    std::stringstream ss("abcXYZ");

    auto result = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 6);

    CHECK(result.value()[0] == 'a');
    CHECK(result.value()[1] == 'b');
    CHECK(result.value()[2] == 'c');
    CHECK(result.value()[3] == 'X');
    CHECK(result.value()[4] == 'Y');
    CHECK(result.value()[5] == 'Z');

    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none succeeds on empty input") {
    std::stringstream ss("");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
}

TEST_CASE("many_or_none returns empty result when first character fails") {
    std::stringstream ss("abc");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_none returns empty result on punctuation") {
    std::stringstream ss("!");

    auto result = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
    CHECK(ss.peek() == '!');
}

TEST_CASE("many_or_none returns empty result on whitespace") {
    std::stringstream ss(" ");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
    CHECK(ss.peek() == ' ');
}

TEST_CASE("many_or_none stops at first non-digit") {
    std::stringstream ss("123abc");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    CHECK(result.value()[0] == 1);
    CHECK(result.value()[1] == 2);
    CHECK(result.value()[2] == 3);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_none stops at first non-letter") {
    std::stringstream ss("abc123");

    auto result = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    CHECK(result.value()[0] == 'a');
    CHECK(result.value()[1] == 'b');
    CHECK(result.value()[2] == 'c');
    CHECK(ss.peek() == '1');
}

TEST_CASE("many_or_none stops at punctuation") {
    std::stringstream ss("123!456");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(ss.peek() == '!');
}

TEST_CASE("many_or_none preserves remaining input") {
    std::stringstream ss("123abc456");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(ss.peek() == 'a');

    ss.get();

    auto letters = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(letters.has_value());
    REQUIRE(letters.value().size() == 2);
    CHECK(letters.value()[0] == 'b');
    CHECK(letters.value()[1] == 'c');
    CHECK(ss.peek() == '4');
}

TEST_CASE("many_or_none parses from current stream position") {
    std::stringstream ss("abc123");

    ss.get();
    ss.get();
    ss.get();

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    CHECK(result.value()[0] == 1);
    CHECK(result.value()[1] == 2);
    CHECK(result.value()[2] == 3);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none does not rewind previously consumed input") {
    std::stringstream ss("x123a");

    ss.get();

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_none can parse digits followed by letters") {
    std::stringstream ss("123abc");

    auto digits = mona::_many_or_none(mona::_digit())(ss);
    auto letters = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(digits.has_value());
    REQUIRE(letters.has_value());

    REQUIRE(digits.value().size() == 3);
    REQUIRE(letters.value().size() == 3);

    CHECK(digits.value()[0] == 1);
    CHECK(digits.value()[1] == 2);
    CHECK(digits.value()[2] == 3);

    CHECK(letters.value()[0] == 'a');
    CHECK(letters.value()[1] == 'b');
    CHECK(letters.value()[2] == 'c');

    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none can be called repeatedly after no matches") {
    std::stringstream ss("abc123");

    auto digits1 = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(digits1.has_value());
    CHECK(digits1.value().empty());
    CHECK(ss.peek() == 'a');

    auto letters = mona::_many_or_none(mona::_letter())(ss);

    REQUIRE(letters.has_value());
    REQUIRE(letters.value().size() == 3);
    CHECK(ss.peek() == '1');

    auto digits2 = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(digits2.has_value());
    REQUIRE(digits2.value().size() == 3);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_none succeeds when parser reaches EOF") {
    std::stringstream ss("123");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().size() == 3);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}