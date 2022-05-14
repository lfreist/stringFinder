add_library(Timer Timer.cpp Timer.h)

add_library(FileChunk FileChunk.h FileChunk.cpp)
target_link_libraries(FileChunk zstd)

add_library(TSQueue ThreadSafeQueue.h ThreadSafeQueue.cpp)
target_link_libraries(TSQueue FileChunk)

add_library(ESFCompressor ESFCompressor.h)
target_link_libraries(ESFCompressor zstd FileChunk)
set_target_properties(ESFCompressor PROPERTIES LINKER_LANGUAGE CXX)

add_library(ESFMetaFile ESFMetaFile.h ESFMetaFile.cpp)

add_library(InputParser InputParser.h)
target_link_libraries(InputParser StringFinder)
set_target_properties(InputParser PROPERTIES LINKER_LANGUAGE CXX)
