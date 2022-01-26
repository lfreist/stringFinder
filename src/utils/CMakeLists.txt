add_library(Timer
        Timer.cpp Timer.h
)

add_library(Buffer
        Buffer.h Buffer.cpp
)

add_library(TSQueue
        ThreadSafeQueue.h ThreadSafeQueue.cpp
        Buffer.h Buffer.cpp
        )
