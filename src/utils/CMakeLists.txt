add_library(ZstdWrapper ZstdWrapper.h)
target_link_libraries(ZstdWrapper zstd)
set_target_properties(ZstdWrapper PROPERTIES LINKER_LANGUAGE CXX)

add_library(Timer Timer.cpp Timer.h)

add_library(FileChunk FileChunk.h FileChunk.cpp)

add_library(TSQueue INTERFACE ThreadSafeQueue.h)

add_library(ESFCompressor ESFCompressor.h)
target_link_libraries(ESFCompressor zstd FileChunk)
set_target_properties(ESFCompressor PROPERTIES LINKER_LANGUAGE CXX)

add_library(SFMetaFile SFMetaFile.h SFMetaFile.cpp)

add_library(Task Task.cpp Task.h)
target_link_libraries(Task FileChunk Timer TSQueue)

add_subdirectory(readers)