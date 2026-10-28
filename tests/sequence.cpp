#include <doctest/doctest.h>

#include <mona/mona.hpp>
#include <sstream>
TEST_CASE("_sequence succeeds with a single parser") {
    std::stringstream ss("7");

    auto result = mona::_sequence(mona::_digit())(ss);

    CHECK(result.has_value());

    auto [i] = result.value();
    CHECK(i == 7);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_sequence succeeds with repeated parser types") {
    std::stringstream ss("123");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit(),
        mona::_digit())(ss);

    CHECK(result.has_value());

    auto [a, b, c] = result.value();
    CHECK(a == 1);
    CHECK(b == 2);
    CHECK(c == 3);
}

TEST_CASE("_sequence succeeds with alternating parser types") {
    std::stringstream ss("a1b2c3");

    auto result = mona::_sequence(
        mona::_letter(),
        mona::_digit(),
        mona::_letter(),
        mona::_digit(),
        mona::_letter(),
        mona::_digit())(ss);

    CHECK(result.has_value());

    auto [a, i0, b, i1, c, i2] = result.value();

    CHECK(a == 'a');
    CHECK(i0 == 1);
    CHECK(b == 'b');
    CHECK(i1 == 2);
    CHECK(c == 'c');
    CHECK(i2 == 3);
}

TEST_CASE("_sequence succeeds and leaves remaining input untouched") {
    std::stringstream ss("42abc");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit())(ss);

    CHECK(result.has_value());

    auto [a, b] = result.value();
    CHECK(a == 4);
    CHECK(b == 2);
    CHECK(ss.peek() == 'a');
}

TEST_CASE("_sequence succeeds with whitespace between matches") {
    std::stringstream ss("1 2");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_match(' '),
        mona::_digit())(ss);

    CHECK(result.has_value());

    auto [a, space, b] = result.value();

    CHECK(a == 1);
    CHECK(space == ' ');
    CHECK(b == 2);
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_sequence fails when the first parser fails") {
    std::stringstream ss("abc");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("_sequence fails when the second parser fails") {
    std::stringstream ss("1!");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("_sequence fails when the last parser fails") {
    std::stringstream ss("1.a!");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_match('.'),
        mona::_letter(),
        mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("_sequence fails when input ends before the sequence completes") {
    std::stringstream ss("1.");
    auto result = mona::_sequence(mona::_digit(), mona::_match('.'), mona::_letter())(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("_sequence fails on empty input") {
    std::stringstream ss("");
    auto result = mona::_sequence(mona::_digit(), mona::_letter())(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_sequence restores input after failure following several matches") {
    std::stringstream ss("12ab!");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit(),
        mona::_letter(),
        mona::_letter(),
        mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("_sequence can be retried after a failure") {
    std::stringstream ss("1a");

    auto failed = mona::_sequence(
        mona::_digit(),
        mona::_digit())(ss);

    CHECK_FALSE(failed.has_value());
    CHECK(ss.peek() == '1');

    auto succeeded = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK(succeeded.has_value());

    auto [i, c] = succeeded.value();
    CHECK(i == 1);
    CHECK(c == 'a');
}

TEST_CASE("_sequence succeeds when starting after an existing prefix") {
    std::stringstream ss("x1a");

    CHECK(mona::_match('x')(ss).has_value());

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK(result.has_value());

    auto [i, c] = result.value();
    CHECK(i == 1);
    CHECK(c == 'a');
}

TEST_CASE("_sequence parses a simple decimal number") {
    std::stringstream ss("12.34");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit(),
        mona::_match('.'),
        mona::_digit(),
        mona::_digit())(ss);

    CHECK(result.has_value());

    auto [a, b, dot, c, d] = result.value();

    CHECK(a == 1);
    CHECK(b == 2);
    CHECK(dot == '.');
    CHECK(c == 3);
    CHECK(d == 4);
}

TEST_CASE("_sequence fails when punctuation is incorrect") {
    std::stringstream ss("12,34");

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit(),
        mona::_match('.'),
        mona::_digit(),
        mona::_digit())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
}

TEST_CASE("_sequence parses a delimited identifier") {
    std::stringstream ss("a=b");

    auto result = mona::_sequence(
        mona::_letter(),
        mona::_match('='),
        mona::_letter())(ss);

    CHECK(result.has_value());

    auto [left, equals, right] = result.value();

    CHECK(left == 'a');
    CHECK(equals == '=');
    CHECK(right == 'b');
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_sequence restores the original nonzero stream position") {
    std::stringstream ss("xyz1!");

    CHECK(mona::_match('x')(ss).has_value());
    CHECK(mona::_match('y')(ss).has_value());

    auto result = mona::_sequence(
        mona::_digit(),
        mona::_letter())(ss);

    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'z');
}