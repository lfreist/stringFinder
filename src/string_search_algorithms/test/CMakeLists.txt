configure_file(files/_search.test.txt files/_search.test.txt)

add_executable(strstrSearchTest using_strstr_test.cpp)
target_link_libraries(strstrSearchTest strstrSearch gtest_main)