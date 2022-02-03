add_subdirectory(utils)

add_library(Benchmark
        Benchmark.h Benchmark.cpp
)
target_link_libraries(Benchmark PUBLIC StringFinder OpenMP::OpenMP_CXX Timer)

add_library(ExternStringFinder
        ExternStringFinder.h ExternStringFinder.cpp
)
target_link_libraries(ExternStringFinder PUBLIC pthread Timer TSQueue Buffer ESFMetaFile)

add_library(StringFinder
        InteractiveFinder.h InteractiveFinder.cpp
        StringFinder.h StringFinder.cpp
)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX Timer)