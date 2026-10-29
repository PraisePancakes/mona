
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "doctest.h"

TEST_CASE("transform with primitive parsers") {
    SUBCASE("letter to string") {
        std::stringstream ss("a");

        auto result = mona::_transform(
            [](char c) { return std::string(1, c); },
            mona::_letter())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "a");
    }

    SUBCASE("letter to uppercase") {
        std::stringstream ss("hello");

        auto result = mona::_transform(
            [](char c) {
                return static_cast<char>(std::toupper(c));
            },
            mona::_letter())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 'H');
    }

    SUBCASE("digit to integer arithmetic") {
        std::stringstream ss("7");

        auto result = mona::_transform(
            [](int n) { return n * n; },
            mona::_digit())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 49);
    }

    SUBCASE("matched string to its length") {
        std::stringstream ss("hello");

        auto result = mona::_transform(
            [](const std::string& s) { return s.size(); },
            mona::_match(std::string("hello")))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 5);
    }
}

TEST_CASE("transform with either") {
    auto parser = mona::_either(
        mona::_match('a'),
        mona::_match('b'));

    auto transform = mona::_transform(
        [](char c) {
            return c == 'a' ? std::string("first")
                            : std::string("second");
        },
        parser);

    SUBCASE("first alternative") {
        std::stringstream ss("a");

        auto result = transform(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "first");
    }

    SUBCASE("second alternative") {
        std::stringstream ss("b");

        auto result = transform(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "second");
    }

    SUBCASE("neither alternative matches") {
        std::stringstream ss("x");

        auto result = transform(ss);

        CHECK_FALSE(result.has_value());
    }
}

TEST_CASE("transform with many_or_none") {
    SUBCASE("transforms a vector of letters into a string") {
        std::stringstream ss("abc123");

        auto result = mona::_transform(
            [](const std::vector<char>& chars) {
                return std::string(chars.begin(), chars.end());
            },
            mona::_many_or_none(mona::_letter()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "abc");
        CHECK(ss.peek() == '1');
    }

    SUBCASE("accepts an empty sequence") {
        std::stringstream ss("123");

        auto result = mona::_transform(
            [](const std::vector<char>& chars) {
                return chars.size();
            },
            mona::_many_or_none(mona::_letter()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 0);
        CHECK(ss.peek() == '1');
    }
}

TEST_CASE("transform with many_or_one") {
    SUBCASE("transforms a nonempty sequence") {
        std::stringstream ss("123abc");

        auto result = mona::_transform(
            [](const std::vector<int>& digits) {
                int value = 0;
                for (int digit : digits) {
                    value = value * 10 + digit;
                }
                return value;
            },
            mona::_many_or_one(mona::_digit()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 123);
        CHECK(ss.peek() == 'a');
    }

    SUBCASE("rejects an empty sequence") {
        std::stringstream ss("abc");

        auto result = mona::_transform(
            [](const std::vector<int>& digits) {
                return digits.size();
            },
            mona::_many_or_one(mona::_digit()))(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == 'a');
    }
}

TEST_CASE("transform with both") {
    SUBCASE("combines two letters") {
        std::stringstream ss("ab");

        auto result = mona::_transform(
            [](const std::pair<char, char>& chars) {
                return std::string{chars.first, chars.second};
            },
            mona::_both(
                mona::_letter(),
                mona::_letter()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "ab");
    }

    SUBCASE("transforms two digits") {
        std::stringstream ss("42");

        auto result = mona::_transform(
            [](const std::pair<int, int>& digits) {
                return digits.first * 10 + digits.second;
            },
            mona::_both(
                mona::_digit(),
                mona::_digit()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 42);
    }

    SUBCASE("fails if the second parser fails") {
        std::stringstream ss("a1");

        auto result = mona::_transform(
            [](const std::pair<char, char>& chars) {
                return std::string{chars.first, chars.second};
            },
            mona::_both(
                mona::_letter(),
                mona::_letter()))(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == 'a');
    }
}

TEST_CASE("transform with sequence") {
    SUBCASE("transforms a letter followed by a digit") {
        std::stringstream ss("a7");

        auto result = mona::_transform(
            [](const std::tuple<char, int>& parsed) {
                return std::string(1, std::get<0>(parsed)) + std::to_string(std::get<1>(parsed));
            },
            mona::_sequence(
                mona::_letter(),
                mona::_digit()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "a7");
    }

    SUBCASE("transforms a three-parser sequence") {
        std::stringstream ss("x42");

        auto result = mona::_transform(
            [](const std::tuple<char, int, int>& parsed) {
                return std::string(1, std::get<0>(parsed)) + std::to_string(
                                                                 std::get<1>(parsed) * 10 + std::get<2>(parsed));
            },
            mona::_sequence(
                mona::_letter(),
                mona::_digit(),
                mona::_digit()))(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "x42");
    }

    SUBCASE("transforms the pipe sequence operator") {
        std::stringstream ss("b5");

        auto result = mona::_transform(
            [](const auto& parsed) {
                return std::string(1, std::get<0>(parsed)) + std::to_string(std::get<1>(parsed));
            },
            mona::_letter() | mona::_digit())(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "b5");
    }

    SUBCASE("sequence failure propagates") {
        std::stringstream ss("aX");

        auto result = mona::_transform(
            [](const std::tuple<char, int>& parsed) {
                return std::get<0>(parsed);
            },
            mona::_sequence(
                mona::_letter(),
                mona::_digit()))(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == 'a');
    }
}

TEST_CASE("transform composes with transform") {
    SUBCASE("transforms a value twice") {
        std::stringstream ss("8");

        auto parser = mona::_transform(
            [](int n) { return n + 2; },
            mona::_digit());

        auto result = mona::_transform(
            [](int n) { return std::to_string(n * 3); },
            parser)(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "30");
    }

    SUBCASE("transforms a repeated parse") {
        std::stringstream ss("abc");

        auto letters = mona::_many_or_one(mona::_letter());

        auto uppercase = mona::_transform(
            [](const std::vector<char>& chars) {
                std::string result(chars.begin(), chars.end());

                for (char& c : result) {
                    c = static_cast<char>(std::toupper(c));
                }

                return result;
            },
            letters);

        auto result = mona::_transform(
            [](const std::string& s) {
                return "[" + s + "]";
            },
            uppercase)(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "[ABC]");
    }
}
