
#include <cmath>
#include <limits>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("number_as parses integers") {
    SUBCASE("parses a single digit") {
        std::stringstream ss("7");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 7);
    }

    SUBCASE("parses multiple digits") {
        std::stringstream ss("12345");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 12345);
    }

    SUBCASE("parses an unsigned integer") {
        std::stringstream ss("42");

        auto result = mona::_number_as<unsigned int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 42u);
    }

    SUBCASE("parses a long integer") {
        std::stringstream ss("123456");

        auto result =
            mona::_number_as<long long>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 123456LL);
    }
}

TEST_CASE("number_as parses floating point numbers") {
    SUBCASE("parses a decimal") {
        std::stringstream ss("12.34");

        auto result =
            mona::_number_as<double>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == doctest::Approx(12.34));
    }

    SUBCASE("parses a float") {
        std::stringstream ss("3.5");

        auto result =
            mona::_number_as<float>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == doctest::Approx(3.5f));
    }

    SUBCASE("parses an integer as a floating point number") {
        std::stringstream ss("42");

        auto result =
            mona::_number_as<double>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == doctest::Approx(42.0));
    }
}

TEST_CASE("number_as parses signs") {
    SUBCASE("parses a negative integer") {
        std::stringstream ss("-42");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == -42);
    }

    SUBCASE("parses a negative decimal") {
        std::stringstream ss("-12.5");

        auto result =
            mona::_number_as<double>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == doctest::Approx(-12.5));
    }

    SUBCASE("parses a positive number without an explicit sign") {
        std::stringstream ss("42");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 42);
    }
}

TEST_CASE("number_as stops at nonnumeric input") {
    SUBCASE("leaves trailing letters unread") {
        std::stringstream ss("123abc");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 123);
        CHECK(ss.peek() == 'a');
    }

    SUBCASE("leaves trailing whitespace unread") {
        std::stringstream ss("456 rest");

        auto result = mona::_number_as<int>("invalid number")(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == 456);
        CHECK(ss.peek() == ' ');
    }
}

TEST_CASE("number_as rejects invalid input") {
    SUBCASE("rejects letters") {
        std::stringstream ss("abc");

        auto result = mona::_number_as<int>("invalid number")(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == 'a');
    }

    SUBCASE("rejects empty input") {
        std::stringstream ss("");

        auto result = mona::_number_as<int>("invalid number")(ss);

        CHECK_FALSE(result.has_value());
    }

    SUBCASE("rejects input beginning with a decimal point") {
        std::stringstream ss(".5");

        auto result =
            mona::_number_as<double>("invalid number")(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == '.');
    }

    SUBCASE("rejects a lone minus sign") {
        std::stringstream ss("-");

        auto result = mona::_number_as<int>("invalid number")(ss);

        CHECK_FALSE(result.has_value());
        CHECK(ss.peek() == '-');
    }
}
