#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("many_or_one parses a single digit") {
    std::stringstream ss("7");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 1);
    CHECK(result.value()[0] == 7);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_one parses all digits") {
    std::stringstream ss("0123456789");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 10);

    for (int i = 0; i < 10; ++i) {
        CHECK(result.value()[i] == i);
    }

    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("many_or_one parses consecutive letters") {
    std::stringstream ss("abcXYZ");

    auto result = mona::_many_or_one(mona::_letter())(ss);

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

TEST_CASE("many_or_one stops at first nonmatching character") {
    std::stringstream ss("123abc456");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    CHECK(result.value()[0] == 1);
    CHECK(result.value()[1] == 2);
    CHECK(result.value()[2] == 3);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_one fails on empty input") {
    std::stringstream ss("");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
}

TEST_CASE("many_or_one fails when first character is a letter") {
    std::stringstream ss("abc123");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_one fails when first character is punctuation") {
    std::stringstream ss("!");

    auto result = mona::_many_or_one(mona::_letter())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '!');
}

TEST_CASE("many_or_one fails on whitespace at start") {
    std::stringstream ss(" 123");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == ' ');
}

TEST_CASE("many_or_one preserves unmatched digit boundary") {
    std::stringstream ss("123!");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(ss.peek() == '!');
}

TEST_CASE("many_or_one parses from current stream position") {
    std::stringstream ss("abc123!");

    ss.get();
    ss.get();
    ss.get();

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    CHECK(result.value()[0] == 1);
    CHECK(result.value()[1] == 2);
    CHECK(result.value()[2] == 3);
    CHECK(ss.peek() == '!');
}

TEST_CASE("many_or_one does not rewind previously consumed input") {
    std::stringstream ss("x123a");

    ss.get();

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_one parses digits followed by letters") {
    std::stringstream ss("123abc");

    auto digits = mona::_many_or_one(mona::_digit())(ss);
    auto letters = mona::_many_or_one(mona::_letter())(ss);

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

TEST_CASE("many_or_one fails without consuming input before a valid parser") {
    std::stringstream ss("abc123");

    auto digits = mona::_many_or_one(mona::_digit())(ss);

    CHECK_FALSE(digits.has_value());
    CHECK(ss.peek() == 'a');

    auto letters = mona::_many_or_one(mona::_letter())(ss);

    REQUIRE(letters.has_value());
    REQUIRE(letters.value().size() == 3);
    CHECK(letters.value()[0] == 'a');
    CHECK(letters.value()[1] == 'b');
    CHECK(letters.value()[2] == 'c');
    CHECK(ss.peek() == '1');
}

TEST_CASE("many_or_none succeeds when there are no matches") {
    std::stringstream ss("abc");

    auto result = mona::_many_or_none(mona::_digit())(ss);

    REQUIRE(result.has_value());
    CHECK(result.value().empty());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_one fails when there are no matches") {
    std::stringstream ss("abc");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("many_or_one succeeds when the final match reaches EOF") {
    std::stringstream ss("123");

    auto result = mona::_many_or_one(mona::_digit())(ss);

    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    CHECK(result.value()[0] == 1);
    CHECK(result.value()[1] == 2);
    CHECK(result.value()[2] == 3);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}