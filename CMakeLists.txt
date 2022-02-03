cmake_minimum_required(VERSION 3.9)
project(StringFinder C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
# set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

find_package(OpenMP)

# GTEST
add_subdirectory(third_party/googletest/)
include_directories(third_party/googletest/googletest/include)

# StringFinder Project
add_subdirectory(src)

add_executable(StringFinderInteractiveMain StringFinderInteractiveMain.cpp)
target_link_libraries(StringFinderInteractiveMain PUBLIC StringFinder)

add_executable(BenchmarkMain BenchmarkMain.cpp)
target_link_libraries(BenchmarkMain PUBLIC Benchmark)

add_executable(ExternStringFinderMain ExternStringFinderMain.cpp)
target_link_libraries(ExternStringFinderMain PUBLIC ExternStringFinder Timer)

add_executable(ESFCompressorMain ESFCompressorMain.cpp)
target_link_libraries(ESFCompressorMain PUBLIC ESFCompressor ESFMetaFile)

configure_file(test/_stringFinderTest test/_stringFinderTest)
configure_file(test/_externFinderTest test/_externFinderTest)
configure_file(test/_bufferTest test/_bufferTest)
configure_file(test/_esfMetaFileTest test/_esfMetaFileTest)

add_subdirectory(test)
include(CTest)
add_test(InteractiveFinderTest test/InteractiveFinderTest)
add_test(StringFinderTest test/StringFinderTest)
add_test(WallTimerTest test/WallTimerTest)
# add_test(ExternStringFinderTest test/ExternStringFinderTest)
add_test(StringTest test/StringTest)
add_test(TSQTest test/TSQTest)
