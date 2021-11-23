add_library(StringFinder
        InteractiveFinder.hpp InteractiveFinder.cpp
        StringFinder.hpp StringFinder.cpp
        Timer.hpp Timer.cpp
)

find_package(OpenMP)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX)

add_library(Benchmark
        Benchmark.hpp Benchmark.cpp
        StringFinder.hpp StringFinder.cpp
        Timer.hpp Timer.cpp
)