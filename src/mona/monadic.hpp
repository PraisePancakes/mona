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
    return [error](stream_like auto& is) -> result<int>
               requires stream_like<std::remove_cvref_t<decltype(is)>>
    {
        char c = is.get();
        if (!std::isdigit(c)) {
            is.unget();
            return std::unexpected(error);
        }
        return c - '0';
    };
}

};  // namespace mona