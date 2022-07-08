add_subdirectory(readers)

add_library(ZstdWrapper ZstdWrapper.h)
target_link_libraries(ZstdWrapper zstd)
set_target_properties(ZstdWrapper PROPERTIES LINKER_LANGUAGE CXX)

add_library(Timer Timer.cpp Timer.h)

add_library(FileChunk FileChunk.h FileChunk.cpp)

add_library(TSQueue INTERFACE ThreadSafeQueue.h)

add_library(ESFCompressor ESFCompressor.h)
target_link_libraries(ESFCompressor zstd FileChunk)
set_target_properties(ESFCompressor PROPERTIES LINKER_LANGUAGE CXX)

add_library(ESFMetaFile ESFMetaFile.h ESFMetaFile.cpp exceptions.h)

#add_library(InputParser InputParser.h)
#target_link_libraries(InputParser StringFinder)
#set_target_properties(InputParser PROPERTIES LINKER_LANGUAGE CXX)

add_library(Task Task.cpp Task.h)
target_link_libraries(Task FileChunk Timer TSQueue)
