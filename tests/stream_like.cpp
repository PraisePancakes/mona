#include <fstream>
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>

#include "doctest.h"

TEST_CASE("stream_like") {
    static_assert(mona::stream_like<std::istream>);
    static_assert(!mona::stream_like<std::iostream>);
    static_assert(mona::stream_like<std::stringstream>);
    static_assert(!mona::stream_like<std::ostream>);
    static_assert(mona::stream_like<std::ifstream>);
};