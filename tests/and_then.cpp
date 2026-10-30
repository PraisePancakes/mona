#include <doctest/doctest.h>

#include <expected>
#include <functional>
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
#include <string>
#include <type_traits>

using namespace mona;

TEST_CASE("mona::_and_then: successful parser composition") {
    std::istringstream is("5a");

    auto p = mona::_and_then(_digit(), [](int value) { return _match(value == 5 ? 'a' : 'b'); });

    auto result = p(is);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'a');
    CHECK(is.peek() == std::char_traits<char>::eof());
}

TEST_CASE("mona::_and_then: propagates first parser failure") {
    std::istringstream is("abc");

    bool invoked = false;

    auto p = mona::_and_then(
        _digit(),
        [&invoked](int value) {
            invoked = true;
            return _match('a');
        });

    auto result = p(is);

    CHECK_FALSE(result.has_value());
    CHECK_FALSE(invoked);
    CHECK(is.peek() == 'a');
}

TEST_CASE("mona::_and_then: propagates second parser failure") {
    std::istringstream is("5b");

    auto p = mona::_and_then(_digit(), [](int value) {
        return _match('a');
    });

    auto result = p(is);

    CHECK_FALSE(result.has_value());
    CHECK(is.peek() == 'b');
}

TEST_CASE("mona::_and_then: chains multiple dependent parsers") {
    std::istringstream is("5ab");

    auto p = mona::_and_then(_digit(), [](int value) { return mona::_and_then(_match('a'),
                                                                              [](char first) { return _match(first == 'a' ? 'b' : 'c'); }); });

    auto result = p(is);

    REQUIRE(result.has_value());
    CHECK(result.value() == 'b');
    CHECK(is.peek() == std::char_traits<char>::eof());
}

TEST_CASE("mona::_and_then: preserves parser abstraction") {
    auto p = mona::_and_then(
        _digit(),
        [](int value) {
            return _match('a');
        });

    static_assert(std::is_same_v<decltype(p), mona::parser<char>>);
}