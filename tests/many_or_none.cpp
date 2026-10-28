#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("many_or_none passes") {
    std::stringstream ss;
    ss << "01234";
    auto result = mona::_many_or_none(mona::_digit())(ss);
    CHECK(result.value().size() == 5);
    for (int i = 0; i < result.value().size(); i++) {
        CHECK(result.value()[i] == i);
    }
    CHECK(ss.peek() == std::char_traits<char>::eof());
};

TEST_CASE("many_or_none passes on empty") {
    std::stringstream ss;
    ss << "01234";
    auto result = mona::_many_or_none(mona::_letter())(ss);
    CHECK(result.value().size() == 0);
    CHECK(ss.peek() == '0');
};

TEST_CASE("many_or_none passes on some substring") {
    std::stringstream ss;
    ss << "012abc";
    auto result = mona::_many_or_none(mona::_digit())(ss);
    CHECK(result.has_value());
    CHECK(result.value().size() == 3);
    for (int i = 0; i < result.value().size(); i++) {
        CHECK(result.value()[i] == i);
    }
    CHECK(ss.peek() == 'a');
};
