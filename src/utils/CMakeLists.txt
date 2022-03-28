add_library(Timer
        Timer.cpp Timer.h
)

add_library(Buffer
        FileChunk.h FileChunk.cpp
)
target_link_libraries(Buffer zstd)

add_library(TSQueue
        ThreadSafeQueue.h ThreadSafeQueue.cpp
        FileChunk.h FileChunk.cpp
        )

add_library(ESFCompressor
        ESFCompressor.h
        )
target_link_libraries(ESFCompressor zstd Buffer)

add_library(ESFMetaFile
        ESFMetaFile.h ESFMetaFile.cpp
        )

add_library(InputParser InputParser.h)
set_target_properties(InputParser PROPERTIES LINKER_LANGUAGE CXX)
