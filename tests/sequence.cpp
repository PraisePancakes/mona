#include <doctest/doctest.h>

#include <mona/mona.hpp>
#include <sstream>

TEST_CASE("_sequence succeeds ") {
    std::stringstream ss("1a");
    auto result = mona::_sequence(mona::_digit(), mona::_letter())(ss);
    CHECK(result.has_value());
    auto [i, c] = result.value();
    CHECK(i == 1);
    CHECK(c == 'a');
}

TEST_CASE("_sequence succeeds on more than 2 ") {
    std::stringstream ss("1.a3");
    auto result = mona::_sequence(mona::_digit(), mona::_match('.'), mona::_letter(), mona::_digit())(ss);
    CHECK(result.has_value());
    auto [i0, d, c, i1] = result.value();
    CHECK(i0 == 1);
    CHECK(d == '.');
    CHECK(c == 'a');
    CHECK(i1 == 3);
}

TEST_CASE("_sequence fails") {
    std::stringstream ss("1.a3");
    auto result = mona::_sequence(mona::_digit(), mona::_match('.'), mona::_letter(), mona::_letter())(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == '1');
   
}
