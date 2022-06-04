cmake_minimum_required(VERSION 3.9)
project(StringFinder C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

# --- OPENMP -----------------------------------------------------------------------------------------------------------
find_package(OpenMP REQUIRED)
# ----------------------------------------------------------------------------------------------------------------------

# --- BOOST ------------------------------------------------------------------------------------------------------------
find_package(Boost 1.74 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIR})
# ----------------------------------------------------------------------------------------------------------------------

# --- GTEST ------------------------------------------------------------------------------------------------------------
add_subdirectory(third_party/googletest/)
include_directories(third_party/googletest/googletest/include)
# ----------------------------------------------------------------------------------------------------------------------

# --- STRINGFINDER -----------------------------------------------------------------------------------------------------
add_subdirectory(src)

# ___ Executables ______________________________________________________________________________________________________
add_executable(ESFCompressorMain ESFCompressorMain.cpp)
target_link_libraries(ESFCompressorMain PUBLIC ESFCompressor ESFMetaFile boost_program_options)

add_executable(StringFinderMain StringFinderMain.cpp)
target_link_libraries(StringFinderMain PUBLIC StringFinder boost_program_options OpenMP::OpenMP_CXX)

add_executable(ExternStringFinderMain ExternStringFinderMain.cpp)
target_link_libraries(ExternStringFinderMain PUBLIC ExternStringFinder Timer boost_program_options)

add_executable(stringSearchBenchmark stringSearchBenchmark.cpp)
target_link_libraries(stringSearchBenchmark PUBLIC Timer boost_program_options)
# ______________________________________________________________________________________________________________________

# ___ Tests ____________________________________________________________________________________________________________
add_subdirectory(test)
add_subdirectory(src/string_search_algorithms/test)
add_subdirectory(src/utils/test)

include(CTest)
add_test(StringFinderTest test/StringFinderTest)
add_test(WallTimerTest test/WallTimerTest)
add_test(TSQTest test/TSQTest)
add_test(InputParserTest test/InputParserTest)

add_test(strstrSearchTest src/string_search_algorithms/test/strstrSearchTest)

add_test(FileChunkTest src/utils/test/FileChunkTest)
# ______________________________________________________________________________________________________________________