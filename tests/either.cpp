#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("either passes") {
    std::stringstream ss;
    ss << "abba123";
    auto result = mona::_many_or_one(mona::_either(mona::_match('a'), mona::_match('b')))(ss);
    CHECK(result.has_value());
    CHECK(result.value().size() == 4);
    CHECK(result.value()[0] == 'a');
    CHECK(result.value()[1] == 'b');
    CHECK(result.value()[2] == 'b');
    CHECK(result.value()[3] == 'a');
    CHECK(ss.peek() == '1');
};

TEST_CASE("either fails") {
    std::stringstream ss;
    ss << "a";
    auto result = mona::_either(mona::_match('1'), mona::_match('2'))(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
    std::cout << result.error() << std::endl;
};

TEST_CASE("either operator| passes") {
    std::stringstream ss;
    ss << "a";
    auto result = (mona::_match('a') | mona::_match('2'))(ss);
    CHECK(result.has_value());
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("either operator| fails") {
    std::stringstream ss;
    ss << "a";
    auto result = (mona::_match('1') | mona::_match('2'))(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}
