configure_file(files/_buffer.test.txt files/_buffer.test.txt)

add_executable(FileChunkTest FileChunkTest.cpp)
target_link_libraries(FileChunkTest gtest_main FileChunk)
