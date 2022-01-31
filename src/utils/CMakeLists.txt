add_library(Timer
        Timer.cpp Timer.h
)

add_library(Buffer
        Buffer.h Buffer.cpp
)
target_link_libraries(Buffer zstd)

add_library(TSQueue
        ThreadSafeQueue.h ThreadSafeQueue.cpp
        Buffer.h Buffer.cpp
        )

add_library(ESFCompressor
        ESFCompressor.h
        )
target_link_libraries(ESFCompressor zstd Buffer)
