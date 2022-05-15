configure_file(files/_chunk.test.txt files/_chunk.test.txt)
configure_file(files/_chunk.test.zst files/_chunk.test.zst)

add_executable(FileChunkTest FileChunkTest.cpp)
target_link_libraries(FileChunkTest gtest_main FileChunk)
