#ifndef MONA_CONFIG_MACRO_H
#define MONA_CONFIG_MACRO_H

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define MONA_STR(arg) #arg
#define MONA_XSTR(arg) MONA_STR(arg)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#ifndef MONA_NDEBUG
#define MONA_ASSERT(condition, message)                                        \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define MONA_ASSERT(condition, message) \
    do {                                \
    } while (false)
#endif