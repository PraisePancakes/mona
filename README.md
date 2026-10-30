# Mona: a modern parser-combinator library
* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Examples](#examples)
* [Usage](#usage)
* [Contributing](#contributions)

# Introduction
To understand the purpose of this library, let's first establish a basic understanding of monadic architecture.

A monad (sometimes jokingly described as a monoid in the category of endofunctors) is an abstract algebraic structure that provides a way of composing computations while preserving some underlying computational context. This composition of computations are produced through two crucial operations; a constructor that wraps a value and some flat mapping that operates on the contained value(s) and returns another wrapped value.

Now, I'm no mathematician, and frankly, I haven't studied much abstract algebra myself. Fortunately, understanding the practical applications of monads doesn't require an extensive mathematical background. For a more approachable introduction, check out this [ELI5](https://www.reddit.com/r/explainlikeimfive/comments/vjaato/eli5_what_are_monads_in_programming/) on the topic.

## Motivation


# Examples
Let's begin with some simple examples to, hopefully, drill down this idea of "composable-yet-unchanged" structures.


