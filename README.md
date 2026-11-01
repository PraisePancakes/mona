# Mona: a modern parser-combinator library
* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Examples](#examples)
        * [Primitives](#primative-parsers)
        * [Combinators](#combination-parsers)
        * [Transformations](#transformations)
        * [Binds](#binds)
* [Usage](#usage)
* [Contributing](#contributions)

# Introduction
To understand the purpose of this library, let's first establish a basic understanding of monadic architecture.

A monad (sometimes jokingly described as a monoid in the category of endofunctors) is an abstract algebraic structure that provides a way of composing computations while preserving some underlying computational context. This composition of computations are produced through two crucial operations; a constructor that wraps a value and some flat mapping that operates on the contained value(s) and returns another wrapped value.

Now, I'm no mathematician, and frankly, I haven't studied much abstract algebra myself. Fortunately, understanding the practical applications of monads doesn't require an extensive mathematical background. For a more approachable introduction, check out this [ELI5](https://www.reddit.com/r/explainlikeimfive/comments/vjaato/eli5_what_are_monads_in_programming/) on the topic.

## Motivation
I started `mona` out of a desire to simplify a problem I kept encountering across my pet-projects: parsing. Whether I was interpreting arithmetic expressions, processing command-line arguments, or experimenting with language design, I found myself writing similar parsing logic over and over again. So to enforce some reusability, I decided to make this combinator as a standalone project so that I don't have to rewrite this combinator again. 

# Examples
Let's begin with some simple examples to, hopefully, drill down this idea of composable parsers.

## Primative parsers
`mona` defines some out of the box **primitive** parsers that contribute as the basis of all combinations:

we have a primitive `_digit` parser (similarly we also have an `_alnum` parser that parses alpha-numeric characters and a `_letter` parser that parses letters):
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("0");
    auto result = mona::_digit()(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> 0
    else std::cout << result.error() << std::endl;
    return 0;
}
```
before we move on, errors can also be passed to these primitives to better help debug parsing errors. If no error is passed, `mona` uses a standard error definition for a failed parse state.

*[NOTE]* these primitives will move the stream position to the last successful position. If no successful positions were found, it resets the state of the stream and the position of the stream.
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("a");
    auto result = mona::_digit("failed to parse digit")(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; 
    else std::cout << result.error() << std::endl; // prints -> failed to parse digit
    return 0;
}
```
next, lets look at the primitve `_match` parser:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("a");
    auto result = mona::_match('a', "failed to match a")(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> a
    else std::cout << result.error() << std::endl; 
    return 0;
}
```
you can also `_match` strings:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("match some of this");
    auto result = mona::_match("match", "failed to match anything")(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> match
    else std::cout << result.error() << std::endl; 
    return 0;
}
```
next, lets look at `_number_as`, which takes as a template argument any integral/floating-point type:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("12.34");
    auto result = mona::_number_as<double>("invalid number")(ss); // parse as double
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> 12.34
    else std::cout << result.error() << std::endl; 
    return 0;
}
```
## Combination parsers
Now that we got the primitives out the way, let the fun begin! Lets look at some **combinators**:

`mona` provides a `_many_or_none` combinator, this combinator does **NOT** return an error value, instead it returns an empty object on failure:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("0123456789");
    auto result = mona::_many_or_none(mona::_digit())(ss);
    for (int i = 0; i < result.value().size(); ++i) std::cout << result.value()[i];  //prints -> 0123456789
    
    // ss is now peeking on an EOF
    return 0;
}
   
```
`mona` also provides the previous combinator's counter part `_many_or_one` which **DOES** return an error value, if no parsers succeed:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("abc");
    auto digit_parser = mona::_digit();
    auto result = mona::_many_or_one(digit_parser, "failed to parse many digits")(ss);
    if (!result.has_value()) std::cout << result.error() << std::endl; //prints -> failed to parse many digits
    return 0;
}
```

`_either` which succeeds on the basis that either of two parsers, with the same underlying type, pass:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("a");
    auto result = mona::_either(mona::_match('a'), mona::_match('2'))(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> a
    return 0;
}
```
similarly, you can write the above code like this:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("a");
    auto result = (mona::_match('a') || mona::_match('2'))(ss);
    if(result.has_value()) std::cout << result.value() << std::endl; // prints -> a
    return 0;
}
```
next, `mona` provides `_both` which succeeds when two parsers with the same underlying type pass in succession, `_both` returns a pair of both successful values:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
   std::stringstream ss("12");
    auto result = mona::_both(mona::_match('1'), mona::_match('2'))(ss);
    if(result.has_value()) {
        auto [one, two] = result.value();
        std::cout << one << " : " << two << std::endl; // prints ->  1 : 2
    }
    return 0;
}
```
similarly, you can write the above code like this:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("12");
    auto result = (mona::_match('1') >> mona::_match('2'))(ss);
    if(result.has_value()) {
        auto [one, two] = result.value();
        std::cout << one << " : " << two << std::endl; // prints ->  1 : 2
    return 0;
}
```
next, we introduce the `_sequence` combinator, which takes a variadic list of distinct or similar parsers and succeeds if all passed parsers succeed in sequence:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("12.34");
    auto result = mona::_sequence(
        mona::_digit(),
        mona::_digit(),
        mona::_match('.'),
        mona::_digit(),
        mona::_digit())(ss);

    if(!result.has_value()) return 1;
    auto [a, b, dot, c, d] = result.value();
    std::cout << a << b << dot << c << d << std::endl; // prints -> 12.34
    return 0;
}
```
*[NOTE]* `mona` does implement an operator for sequencing, however since candidate operators are binary and left associative, This is up to debate and some updates may be made in the future such that `operator|` works on more than two parsers and simply returns the result as a flattened tuple, rather than a nested tuple. For now, I left the binary operator to strictly work on two parsers at a time:

```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("1a");
    auto result = (mona::_digit() | mona::_letter())(ss);
    if(!result.has_value()) return 1;
    auto [digit, letter] = result.value();
    std::cout << digit << " : " << letter <<  std::endl; // prints -> 1 : a
    return 0;
}
```

with failure in the sequence:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("a2.34");
    auto result = mona::_sequence(
        mona::_digit("first digit error"),
        mona::_digit(),
        mona::_match('.'),
        mona::_digit(),
        mona::_digit())(ss);

    if (!result.has_value()) {
        std::cout << result.error() << std::endl;
        return 1;
    } 
     /* prints ->
        parser _sequence failed.
        [TRACE]: 
        first digit error
        unexpected token in _digit parser
        unexpected token in _match parser
        unexpected token in _digit parser
        unexpected token in _digit parser
     */
    return 0;
}
```

`mona` also provides an `_any` combinator, which on success, short-circuits to the first successful parser of some list of parsers with the same underlying type:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
    std::stringstream ss("c123");
    auto result = mona::_any(
        "expected a, b, or c",
        mona::_match('a'),
        mona::_match('b'),
        mona::_match('c'))(ss);
    if(!result.has_value()) return 1;
    std::cout << result.value() << std::endl; // prinst -> c
    return 0;
}
```

## Transformations
`mona` provides some utility functions for transformations:

`_transform` ,which in some functional languages acts like `fmap`, applies the functorial mapping, `fmap : (T -> U) -> F(T) -> F(U)` :
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
     std::stringstream ss("123abc");
      auto result = mona::_transform(
           [](const std::vector<int>& digits) {
                int value = 0;
                for (int digit : digits) {
                    value = value * 10 + digit;
                }
                return value + 1;
            },
         mona::_many_or_one(mona::_digit()))(ss);

       if(!result.has_value()) return -1;
       std::cout << result.value() << std::endl; // prints -> 124
    return 0;
}
```

`_transform` also works on result, which behaves like `_transform(parser)` but rather than operating in the parser space, it operates in the result space of some mapping, `fmap: (T -> U) -> Result(T) -> Result(U)` :
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
      mona::result<int> input = 42;
        auto output = mona::_transform(
            [](int x) { return x * 2; },
            input);
        if(!output.has_value()) return -1;
        std::cout << output.value() << std::endl; // prints -> 84
    return 0;
}
```

## Binds
To resemble a conventional functional paradigm, `mona` provides a flat_map like function that takes the form `flat_map: F(T) -> F(F(U)) -> F(U)`, to mimic this idea, here is the `_and_then` utility:
```c++
#include <iostream>
#include <mona/mona.hpp>
#include <sstream>
int main() {
     std::istringstream is("5a");
    auto p = mona::_and_then(_digit(), [](int value) { return _match(value == 5 ? 'a' : 'b'); });
    auto result = p(is);
    if(!result.has_value()) return -1;
    std::cout << result.value() << std::endl; // prints -> a
    static_assert(std::is_same_v<decltype(p), parser<char>>);
    return 0;
}

```

# Usage

`mona` is a header-only library, simply #include <mona/mona.hpp> at the top of your file and start coding!

## Requirements
`mona` is built on a compiler that supports at least C++23.
Recommended Compiler Specs. include `Clang 20.1.2 x86_64` and `GCC 14.2.0 x86_64`.
This project requires `CMake` version 3.28 or later.
# Building
 You can use `mona` from a CMake project by simpling linking an existing target to the `mona::mona` alias. The library can be fetched using `add_subdirectory()` or `FetchContent_Declare()`

 **Building Tests/Examples/Benchmarks**

 To build tests/examples/benchmarks navigate to the CMakeLists.txt file in the root directory of `mona` and locate 
 `option(MONA_BUILD_EXAMPLES "Build Mona examples" OFF)`
 `option(MONA_BUILD_TESTS "Build Mona tests" OFF)`
 `option(MONA_BUILD_BENCHMARK "Build Mona benchmarks" OFF)`
 you can then switch from `OFF` to `ON` which will build the options.

# Contributions
`mona` is an open source library, contributions are not only welcomed but encouraged. Feel free to create an issue or submit a pull request from a new branch.
I will gladly review it and give my feedback.












