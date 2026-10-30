include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)

FetchContent_MakeAvailable(doctest)

add_executable(mona_tests
digit.cpp
match.cpp
letter.cpp
many_or_none.cpp
many_or_one.cpp
either.cpp
any.cpp
both.cpp
sequence.cpp
transform.cpp
number_as.cpp
and_then.cpp
main.cpp
)

target_link_libraries(mona_tests
    PRIVATE
        mona
        doctest::doctest
)
target_compile_options(mona_tests PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
)

target_link_options(mona_tests PRIVATE
    -fsanitize=address
)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

doctest_discover_tests(mona_tests)