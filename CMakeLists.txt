cmake_minimum_required(VERSION 3.28)
set(MONA_VERSION_REGEX "#define MONA_VERSION_.*[ \t]+(.+)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/mona/config/version.hpp" MONA_VERSION REGEX ${MONA_VERSION_REGEX})
list(TRANSFORM MONA_VERSION REPLACE ${MONA_VERSION_REGEX} "\\1")
string(JOIN "." MONA_VERSION ${MONA_VERSION})
set(PROJECT_NAME mona)

project(
    ${PROJECT_NAME} 
    VERSION ${MONA_VERSION} 
    DESCRIPTION "A monadic parser library made in modern C++"
    HOMEPAGE_URL "https://github.com/praispancakes/mona"
    LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS  "*")
message(STATUS  "* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(STATUS  "*")


add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

option(MONA_BUILD_DEV "Build the developement bed" OFF)
if(MONA_BUILD_DEV)
    option(MONA_BUILD_EXAMPLES "Build Mona examples" ON)
    option(MONA_BUILD_TESTS "Build Mona tests" ON)
    option(MONA_BUILD_BENCHMARK "Build Mona benchmarks" ON)
    option(MONA_PROFILE "Build with profiling-friendly settings" ON)
    if(MONA_PROFILE)
        add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:-g>)
    endif()
    if(MONA_BUILD_EXAMPLES)
        add_executable(mona_example examples/main.cpp)
        target_link_libraries(mona_example PRIVATE mona)
    endif()
    if(MONA_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
     if(MONA_BUILD_BENCHMARK)
        add_subdirectory(benchmark)
    endif()
endif()