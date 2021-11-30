add_library(Timer
        Timer.cpp Timer.hpp
)

add_library(StringFinder
        InteractiveFinder.hpp InteractiveFinder.cpp
        StringFinder.hpp StringFinder.cpp
        Timer.hpp Timer.cpp
)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX)

add_library(Benchmark
        Benchmark.hpp Benchmark.cpp
        StringFinder.hpp StringFinder.cpp
        Timer.hpp Timer.cpp
)
target_link_libraries(Benchmark PUBLIC OpenMP::OpenMP_CXX)

add_library(ExternStringFinder
        ExternStringFinder.hpp ExternStringFinder.cpp
)