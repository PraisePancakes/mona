#pragma once
#include <expected>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
namespace mona {
template <typename T>
using result = std::expected<T, std::string_view>;

template <typename>
struct stream_traits;

template <typename CharT, typename Traits>
struct stream_traits<std::basic_istream<CharT, Traits>> {
    using value_type = CharT;
};

template <typename CharT, typename Traits>
struct stream_traits<std::basic_stringstream<CharT, Traits>> {
    using value_type = CharT;
};

template <typename CharT, typename Traits>
struct stream_traits<std::basic_ifstream<CharT, Traits>> {
    using value_type = CharT;
};

// what can an stream_like concept represent?
// we need a concept that can accept some character stream

template <typename T>
concept stream_like = requires(T v) {
    typename stream_traits<T>::value_type;
    v.get();
    v.peek();
    v.unget();
    v.ignore();
    { v.eof() } -> std::convertible_to<bool>;
};

inline auto _digit(std::string_view error) {
    return [error](stream_like auto& is) -> result<int> {
        using value_type = typename stream_traits<std::remove_cvref_t<decltype(is)>>::value_type;
        value_type v = is.get();
        if (!std::isdigit(v)) {
            is.unget();
            return std::unexpected(error);
        }
        return v - '0';
    };
};

inline auto _digit() {
    return _digit("unexpected token in _digit parser");
};

template <typename CharT>
    requires(!std::is_pointer_v<CharT>)
inline auto _match(const CharT char_to_match, std::string_view error)
    requires requires { typename std::char_traits<CharT>::char_type; }
{
    return [error, char_to_match](stream_like auto& is) -> result<CharT> {
        using value_type = typename stream_traits<std::remove_cvref_t<decltype(is)>>::value_type;
        value_type v = is.get();
        if (v != static_cast<value_type>(char_to_match)) {
            is.unget();
            return std::unexpected(error);
        }
        return v;
    };
};

template <typename CharT>
    requires(!std::is_pointer_v<CharT>)
inline auto _match(const CharT c)
    requires requires { typename std::char_traits<CharT>::char_type; }
{
    return _match(c, "unexpected token in _letter parser");
};

inline auto _match(std::string_view string_to_match, std::string_view error) {
    return [error, string_to_match](stream_like auto& is) -> result<std::string> {
        using value_type = typename stream_traits<std::remove_cvref_t<decltype(is)>>::value_type;
        int seen = 0;
        std::string builder{""};
        for (seen = 0; seen < string_to_match.size(); seen++) {
            value_type c = is.get();
            if (c != string_to_match[seen]) {
                while (seen--) is.unget();
                return std::unexpected(error);
            }
            builder += c;
        }
        return builder;
    };
};

inline auto _match(std::string_view s) {
    return _match(s, "unexpected token in _letter parser");
};

inline auto _letter(std::string_view error) {
    return [error](stream_like auto& is) -> result<char> {
        using value_type = typename stream_traits<std::remove_cvref_t<decltype(is)>>::value_type;
        value_type c = is.get();
        if (!std::isalpha(c)) {
            is.unget();
            return std::unexpected(error);
        }
        return c;
    };
};

inline auto _letter() {
    return _letter("unexpected token in _letter parser");
};

};  // namespace mona