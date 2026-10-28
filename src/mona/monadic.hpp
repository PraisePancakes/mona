#pragma once
#include <expected>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
namespace mona {
template <typename T>
using result = std::expected<T, std::string>;

template <typename T>
struct parser : std::function<result<T>(std::istream&)> {
    using std::function<result<T>(std::istream&)>::function;
    using value_type = T;
    constexpr parser(parser<T> (*ptr)()) : parser(ptr()) {}
};

[[nodiscard]] inline parser<int> _digit(const std::string& error) noexcept {
    return [error](std::istream& is) -> result<int> {
        char v = is.get();
        if (!std::isdigit(v)) {
            is.unget();
            return std::unexpected(error);
        }
        return v - '0';
    };
};

[[nodiscard]] inline parser<int> _digit() noexcept {
    return _digit("unexpected token in _digit parser");
};

[[nodiscard]] inline parser<char> _match(const char char_to_match, const std::string& error) noexcept {
    return [error, char_to_match](std::istream& is) -> result<char> {
        const char v = is.get();
        if (v != char_to_match) {
            is.unget();
            return std::unexpected(error);
        }
        return v;
    };
};

[[nodiscard]] inline parser<char> _match(const char c) noexcept {
    return _match(c, "unexpected token in _match parser");
};

[[nodiscard]] inline parser<std::string> _match(const std::string& string_to_match, const std::string& error) noexcept {
    return [error, string_to_match](std::istream& is) -> result<std::string> {
        const auto start = is.tellg();
        std::string builder{""};
        for (char expected : string_to_match) {
            const auto c = is.get();
            if (c == std::char_traits<char>::eof() || c != expected) {
                is.clear();
                if (start != std::istream::pos_type(std::char_traits<char>::eof())) is.seekg(start);
                return std::unexpected(error);
            }
            builder += c;
        }
        return builder;
    };
};

[[nodiscard]] inline parser<std::string> _match(const std::string& s) noexcept {
    return _match(s, "unexpected token in _letter parser");
};

[[nodiscard]] inline parser<char> _letter(const std::string& error) noexcept {
    return [error](std::istream& is) -> result<char> {
        const char c = is.get();
        if (!std::isalpha(c)) {
            is.unget();
            return std::unexpected(error);
        }
        return c;
    };
};

[[nodiscard]] inline parser<char> _letter() noexcept {
    return _letter("unexpected token in _letter parser");
};

template <typename T>
[[nodiscard]] inline parser<std::vector<T>> _many_or_none(parser<T> p) noexcept {
    return [p](std::istream& is) -> result<std::vector<T>> {
        std::vector<T> res;
        auto result = p(is);
        while (result.has_value()) {
            res.push_back(result.value());
            result = p(is);
        };
        return res;
    };
};

template <typename T>
[[nodiscard]] inline parser<std::vector<T>> _many_or_one(parser<T> p, const std::string& error) noexcept {
    return [p, error](std::istream& is) -> result<std::vector<T>> {
        std::streampos starting_pos{is.tellg()};
        auto result = p(is);
        std::vector<T> res;
        while (result.has_value()) {
            res.push_back(result.value());
            result = p(is);
        }
        if (res.empty()) {
            is.seekg(starting_pos);
            return std::unexpected(error);
        };
        return res;
    };
};

template <typename T>
[[nodiscard]] inline parser<std::vector<T>> _many_or_one(parser<T> p) noexcept {
    return _many_or_one(p, "parser _many_or_one failed due to no sequence matching");
};

template <typename T>
[[nodiscard]] inline parser<T> _either(parser<T> a, parser<T> b) noexcept {
    return [a, b](std::istream& is) -> result<T> {
        auto v1 = a(is);
        if (v1.has_value()) return v1.value();
        auto v2 = b(is);
        if (v2.has_value()) return v2.value();
        return std::unexpected(std::format("parser _either failed, neither parser accepts the sequence.\n[TRACE]: \n{} \n{}", v1.error(), v2.error()));
    };
};

template <typename T>
[[nodiscard]] inline parser<T> operator||(parser<T> a, parser<T> b) noexcept {
    return _either(a, b);
};

template <typename T>
[[nodiscard]] inline parser<std::pair<T, T>> _both(parser<T> a, parser<T> b) noexcept {
    return [a, b](std::istream& is) -> result<std::pair<T, T>> {
        std::streampos pos{is.tellg()};
        auto v1 = a(is);
        auto v2 = b(is);
        if (v1.has_value() && v2.has_value()) return std::make_pair(v1.value(), v2.value());
        std::string err{""};
        err += (v1.has_value() ? "" : v1.error()) + "\n" + (v2.has_value() ? "" : v2.error());
        is.seekg(pos);
        return std::unexpected(std::format("parser _either failed, neither parser accepts the sequence.\n[TRACE]: \n{}", err));
    };
}

template <typename T>
[[nodiscard]] inline parser<std::pair<T, T>> operator&&(parser<T> a, parser<T> b) noexcept {
    return _both(a, b);
};

template <typename... Parsers>
    requires(std::is_default_constructible_v<typename Parsers::value_type> && ...)
[[nodiscard]] inline auto _sequence(Parsers&&... ps) noexcept
    -> parser<std::tuple<std::remove_cvref_t<typename Parsers::value_type>...>> {
    using tuple_type = std::tuple<std::remove_cvref_t<typename Parsers::value_type>...>;
    using result_type = result<tuple_type>;
    return [xt = std::make_tuple(std::forward<Parsers>(ps)...)](std::istream& is) -> result_type {
        std::streampos initial{is.tellg()};
        bool has_error{false};
        std::string error{""};
        tuple_type res = std::apply([&has_error, &error, &is](auto&&... args) {
            return std::tuple_cat([&has_error, &error, &is](auto&& arg) {
                decltype(arg(is)) v = std::forward<decltype(arg)>(arg)(is);
                if (v.has_value()) return std::make_tuple(v.value());
                has_error = true;
                error += v.error();
                return std::make_tuple(typename decltype(arg(is))::value_type{});
            }(args)...);
        },
                                    xt);
        if (!has_error) return result_type{res};
        is.clear();
        is.seekg(initial);
        return std::unexpected(std::format("parser _sequence failed.\n[TRACE]: \n{}", error));
    };
};
template <typename T, typename U>
    requires(std::is_default_constructible_v<typename T::value_type> && std::is_default_constructible_v<typename U::value_type>)
[[nodiscard]] inline auto operator|(T&& a, U&& b) noexcept {
    return _sequence(std::forward<T>(a), std::forward<U>(b));
};

};  // namespace mona