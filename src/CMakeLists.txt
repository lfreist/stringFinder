add_library(Timer
        Timer.cpp Timer.h
)

add_library(StringFinder
        InteractiveFinder.h InteractiveFinder.cpp
        StringFinder.h StringFinder.cpp
        Timer.h Timer.cpp
)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX)

add_library(Benchmark
        Benchmark.h Benchmark.cpp
        StringFinder.h StringFinder.cpp
        Timer.h Timer.cpp
)
target_link_libraries(Benchmark PUBLIC OpenMP::OpenMP_CXX)

add_library(String
        String.h String.cpp
)

#add_library(ExternStringFinder
#        ExternStringFinder.h ExternStringFinder.cpp
#        String.h String.cpp
#        Timer.h Timer.cpp
#)

add_library(TSQueue
        ThreadSafeQueue.h ThreadSafeQueue.cpp
        String.h String.cpp
)
