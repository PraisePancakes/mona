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
inline auto _match(const CharT char_to_match, std::string_view error) {
    return [error, char_to_match](stream_like auto& is) -> result<char> {
        using value_type = typename stream_traits<std::remove_cvref_t<decltype(is)>>::value_type;
        static_assert(std::is_same_v<value_type, CharT>, "ill-formed character type conversion, char_to_match of type CharT does not match the input stream's value_type");
        value_type v = is.get();
        if (v != static_cast<value_type>(char_to_match)) {
            is.unget();
            return std::unexpected(error);
        }
        return v;
    };
};

template <typename CharT>
inline auto _match(const CharT c) {
    return _match(c, "unexpected token in _letter parser");
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