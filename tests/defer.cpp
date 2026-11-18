#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("_defer parser") {
    SUBCASE("_defers parser construction") {
        int constructions = 0;

        auto _deferred = mona::_defer([&]() {
            ++constructions;
            return mona::_match('a');
        });

        CHECK(constructions == 0);

        std::stringstream ss{"a"};

        CHECK(_deferred(ss).has_value());
        CHECK(constructions == 1);
    }

    SUBCASE("constructs parser on every invocation") {
        int constructions = 0;

        auto _deferred = mona::_defer([&]() {
            ++constructions;
            return mona::_match('a');
        });

        std::stringstream ss{"aa"};

        CHECK(_deferred(ss).has_value());
        CHECK(_deferred(ss).has_value());
        CHECK(constructions == 2);
    }

    SUBCASE("propagates parser failure") {
        auto _deferred = mona::_defer([] {
            return mona::_match('a');
        });

        std::stringstream ss{"b"};

        CHECK_FALSE(_deferred(ss).has_value());
    }
}

TEST_CASE("_defer recursive parentheses") {
    mona::parser<std::string> expression;
    expression = mona::_either(mona::_match("a"), mona::_transform(
                                                      [](auto tup) -> std::string {
                                                          auto [l, e, r] = tup;
                                                          return std::string(1, l) + e + std::string(1, r);
                                                      },
                                                      mona::_sequence(
                                                          mona::_match('('),
                                                          mona::_defer([&]() { return expression; }),
                                                          mona::_match(')'))));

    SUBCASE("parses a simple expression") {
        std::stringstream ss{"a"};
        auto result = expression(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "a");
    }

    SUBCASE("parses one nested expression") {
        std::stringstream ss{"(a)"};
        auto result = expression(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "(a)");
    }

    SUBCASE("parses deeply nested expressions") {
        std::stringstream ss{"((((a))))"};
        auto result = expression(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "((((a))))");
    }
}