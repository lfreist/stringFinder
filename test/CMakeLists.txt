add_executable(StringFinderTest StringFinderTest.cpp)
target_link_libraries(StringFinderTest StringFinder gtest_main)

add_executable(WallTimerTest WallTimerTest.cpp)
target_link_libraries(WallTimerTest Timer gtest_main)

add_executable(TSQTest ThreadSafeQueueTest.cpp)
target_link_libraries(TSQTest TSQueue FileChunk gtest_main)

add_executable(ESFMetaFileTest ESFMetaFileTest.cpp)
target_link_libraries(ESFMetaFileTest gtest_main ESFMetaFile)

add_executable(InputParserTest InputParserTest.cpp)
target_link_libraries(InputParserTest gtest_main InputParser)

add_executable(ExternStringFinderTest ExternStringFinderTest.cpp)
target_link_libraries(ExternStringFinderTest ExternStringFinder gtest_main)