#pragma once
#include <expected>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
namespace mona {
template <typename T>
using result = std::expected<T, std::string>;
/**
 * Represents a boxed parser type that can map to another boxed parser type
 */
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

[[nodiscard]] inline parser<std::string> _alnum(const std::string& error_message) {
    return [error_message](std::istream& ss) -> result<std::string> {
        std::string ret = "";
        while (true) {
            if (std::isalnum(ss.peek())) {
                ret += ss.get();
            } else {
                break;
            }
        }

        if (ret.empty()) return std::unexpected(error_message);
        return ret;
    };
}

[[nodiscard]] inline parser<std::string> _alnum() {
    return _alnum("_alnum parser error");
}

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
        std::streampos reset{is.tellg()};
        auto old_state = is.rdstate();
        auto v1 = a(is);
        if (v1.has_value()) return v1.value();
        auto v2 = b(is);
        if (v2.has_value()) return v2.value();
        is.clear(old_state);
        is.seekg(reset);
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
[[nodiscard]] inline parser<std::pair<T, T>> operator>>(parser<T> a, parser<T> b) noexcept {
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

template <typename Func, typename T>
    requires(!std::is_same_v<void, std::invoke_result_t<Func, typename ::mona::parser<T>::value_type>>)
[[nodiscard]] inline ::mona::parser<std::invoke_result_t<Func, typename ::mona::parser<T>::value_type>> _transform(Func f, parser<T> parser) noexcept {
    return [f, parser](std::istream& is) -> result<std::invoke_result_t<Func, typename ::mona::parser<T>::value_type>> {
        auto v = parser(is);
        if (!v.has_value()) return std::unexpected(std::format("parser transformation failed.\n[TRACE]: \n{}", v.error()));
        return std::invoke(f, v.value());
    };
};

template <typename Func, typename T>
    requires(!std::is_same_v<void, std::invoke_result_t<Func, typename ::mona::result<T>::value_type>>)
[[nodiscard]] inline std::invoke_result_t<Func, typename ::mona::result<T>::value_type> _transform_result(Func f, result<T> result) noexcept {
    return [f, result](std::istream& is) -> ::mona::result<std::invoke_result_t<Func, typename ::mona::result<T>::value_type>> {
        if (result.has_value()) return std::invoke(f, result.value());
        return std::unexpected(std::format("result transformation failed.\n[TRACE]: \n{}", result.error()));
    };
}

template <typename... Ts>
[[nodiscard]] constexpr parser<std::tuple_element_t<0, std::tuple<Ts...>>> _any(parser<Ts> const&... ts) {
    return [=](std::istream& is) -> result<std::tuple_element_t<0, std::tuple<Ts...>>> {
        std::array<parser<std::tuple_element_t<0, std::tuple<Ts...>>>, sizeof...(Ts)> parsers{ts...};
        std::string last_error;
        std::streampos reset{is.tellg()};
        for (auto const& v : parsers) {
            auto val = v(is);
            if (val) {
                return val;
            }
            last_error = val.error();
        }
        is.clear();
        is.seekg(reset);
        return std::unexpected(last_error);
    };
}

template <typename... Ts>
[[nodiscard]] constexpr parser<std::tuple_element_t<0, std::tuple<Ts...>>> _any(const std::string& with_error, parser<Ts> const&... ts) {
    return [=](std::istream& is) -> result<std::tuple_element_t<0, std::tuple<Ts...>>> {
        auto v = _any(ts...)(is);
        return v.has_value() ? v : std::unexpected(with_error);
    };
}
namespace internal {
template <typename T>
T __delegate_sto_numerical_T(const std::string& s) {
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned int>) {
        return std::stoi(s);
    } else if constexpr (std::is_same_v<T, float>) {
        return std::stof(s);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(s);
    } else if constexpr (std::is_same_v<T, long int>) {
        return std::stol(s);
    } else if constexpr (std::is_same_v<T, unsigned long int>) {
        return std::stoul(s);
    } else if constexpr (std::is_same_v<T, unsigned long long int>) {
        return std::stoull(s);
    } else if constexpr (std::is_same_v<T, long long int>) {
        return std::stoll(s);
    }
};
};  // namespace internal
template <typename T>
    requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
[[nodiscard]] inline parser<T> _number_as(const std::string& error) noexcept {
    return [error](std::istream& is) -> result<T> {
        std::string ret{""};
        std::streampos begin{is.tellg()};
        auto digit_sequence = _transform([](const auto& digits) -> std::string {
            std::string ret{""};
            for (const auto& d : digits) {
                ret += d + '0';
            }
            return ret;
        },
                                         _many_or_one(_digit()));
        if (auto v = _match('-')(is))
            if (v.has_value()) ret += v.value();
        if (auto v = digit_sequence(is))
            if (v.has_value()) ret += v.value();
        if (auto v = _match('.')(is))
            if (v.has_value()) ret += v.value();
        if (auto v = digit_sequence(is))
            if (v.has_value()) ret += v.value();

        if (ret.empty()) {
            is.clear();
            is.seekg(begin);
            return std::unexpected("_number_as parser failed to parse numeric");
        }
        try {
            std::stoi(ret);
        } catch (std::exception& e) {
            is.clear();
            is.seekg(begin);
            return std::unexpected("_number_as parser failed to parse numeric");
        }
        return static_cast<T>(internal::__delegate_sto_numerical_T<T>(ret));
    };
};
template <typename Func, typename T>
[[nodiscard]] inline parser<typename std::invoke_result_t<Func, T>::value_type> _and_then(parser<T> p, Func f) {
    using parser_type = std::invoke_result_t<Func, T>;
    return [f, p](std::istream& is) -> result<typename parser_type::value_type> {
        return p(is).and_then([&](const T& value) {
            return std::invoke(f, value)(is);
        });
    };
}
template <typename T>
[[nodiscard]] inline parser<T> _check(parser<T> p) noexcept {
    return [p](std::istream& is) -> result<T> {
        std::streampos reset{is.tellg()};
        auto prev_state = is.rdstate();
        auto v = p(is);
        if (v.has_value()) {
            is.clear(prev_state);
            is.seekg(reset);
            return v.value();
        }
        is.clear(prev_state);
        is.seekg(reset);
        return std::unexpected("_check did not satisfy the parser's condition");
    };
};

};  // namespace mona