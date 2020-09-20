cmake_minimum_required(VERSION 3.10)

project(sclap_test)

set(CMAKE_CXX_STANDARD 11)

include_directories(../)

add_subdirectory(lib/googletest)

set(BINARY ${CMAKE_PROJECT_NAME})

add_executable(${BINARY} tests.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${BINARY} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif()

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC gtest)