#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::istringstream is("5a");
    auto p = mona::_and_then(mona::_digit(), [](int value) { return mona::_match(value == 5 ? 'a' : 'b'); });
    auto result = p(is);
    if (!result.has_value()) return -1;
    std::cout << result.value() << std::endl;  // prints -> a
    static_assert(std::is_same_v<decltype(p), mona::parser<char>>);
    return 0;
}