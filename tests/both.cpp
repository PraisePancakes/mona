#include <doctest/doctest.h>

#include <mona/mona.hpp>
#include <sstream>

TEST_CASE("_both succeeds when both parsers match") {
    std::stringstream ss("12");
    auto result = mona::_both(mona::_match('1'), mona::_match('2'))(ss);
    REQUIRE(result.has_value());
    CHECK(result->first == '1');
    CHECK(result->second == '2');
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_both succeeds with operator&&") {
    std::stringstream ss("ab");
    auto result = (mona::_match('a') && mona::_match('b'))(ss);
    REQUIRE(result.has_value());
    CHECK(result->first == 'a');
    CHECK(result->second == 'b');
}

TEST_CASE("_both fails when the first parser fails") {
    std::stringstream ss("ab");
    auto result = mona::_both(mona::_match('x'), mona::_match('b'))(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
    CHECK_FALSE(result.error().empty());
}

TEST_CASE("_both fails when the second parser fails") {
    std::stringstream ss("ac");
    auto result = mona::_both(mona::_match('a'), mona::_match('b'))(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
    CHECK_FALSE(result.error().empty());
}

TEST_CASE("_both includes both errors when both parsers fail") {
    std::stringstream ss("x");
    auto result = mona::_both(mona::_match('a', "expected a"), mona::_match('b', "expected b"))(ss);
    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().find("expected a") != std::string::npos);
    CHECK(result.error().find("expected b") != std::string::npos);
}

TEST_CASE("_both works with multi-character parsers") {
    std::stringstream ss("hello!");

    auto result = mona::_both(mona::_match("hel"), mona::_match("lo!"))(ss);

    REQUIRE(result.has_value());
    CHECK(result->first == "hel");
    CHECK(result->second == "lo!");
    CHECK(ss.peek() == std::char_traits<char>::eof());
}

TEST_CASE("_both rolls back after a partial second-parser match") {
    std::stringstream ss("abc");
    auto result = mona::_both(mona::_match("a"), mona::_match("xy"))(ss);
    CHECK_FALSE(result.has_value());
    CHECK(ss.peek() == 'a');
}

TEST_CASE("_both handles empty input") {
    std::stringstream ss("");
    auto result = mona::_both(mona::_match('a'), mona::_match('b'))(ss);
    CHECK_FALSE(result.has_value());
}