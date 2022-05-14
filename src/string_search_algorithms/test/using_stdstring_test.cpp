// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>
// TODO: implement, this is still using_strstr_test

#include <string>
#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

#include "../using_stdstring.h"

using std::string;
using namespace stdstring_alg;

TEST(stdstringSearchTest, findAllPerLine) {
  std::string filepath("files/_search.test.txt");
  std::ifstream file(filepath);
  string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  {
    int matches = findAllPerLine("keyword", content);
    ASSERT_EQ(matches, 4);
  }
  {
    int matches = findAllPerLine("eureka", content);
    ASSERT_EQ(matches, 0);
  }
}

TEST(stdstringSearchTest, findAll) {
  // TODO: fix file path here
  std::string filepath("files/_search.test.txt");
  std::ifstream file(filepath);
  string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  {
    int matches = findAll("keyword", content);
    ASSERT_EQ(matches, 9);
  }
  {
    int matches = findAll("eureka", content);
    ASSERT_EQ(matches, 0);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
