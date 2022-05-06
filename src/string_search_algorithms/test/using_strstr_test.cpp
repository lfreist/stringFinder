// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <iostream>

#include <gtest/gtest.h>

#include "../using_strstr.h"

using std::string;
using namespace strstr_alg;

TEST(strstrSearchTest, findNextNewLine) {
  const string filepath("src/string_search_algorithms/test/files/_search.test.txt");
  FILE* file = fopen(filepath.c_str(), "r");
  if (file == nullptr) {
    std::cout << "reading file " << filepath << " failed..." << std::endl;
    exit(1);
  }
  char content[350];
  size_t read_bytes = fread(content, sizeof(char), 341, file);
  int match = findNextNewLine(content, 0);
  ASSERT_EQ(match, 23);
  match = findNextNewLine(content, 25);
  ASSERT_EQ(match, 83);
  match = findNextNewLine(content, 130);
  ASSERT_EQ(match, 197);
  match = findNextNewLine(content, 294);
  ASSERT_EQ(match, 308);
  match = findNextNewLine(content, 309);
  ASSERT_EQ(match, -1);
}

TEST(strstrSearchTest, findNext) {
  std::string filepath("src/string_search_algorithms/test/files/_search.test.txt");
  FILE* file = fopen(filepath.c_str(), "r");
  if (file == nullptr) {
    std::cout << "reading file " << filepath << " failed..." << std::endl;
    exit(-1);
  }
  const char pattern[] = "keyword";
  char content[350];
  size_t read_bytes = fread(content, sizeof(char), 341, file);
  {
    int match = findNext(pattern, content, 0);
    ASSERT_EQ(match, 16);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 50);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 76);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 130);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 153);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 198);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 240);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 271);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, 278);
    match = findNext(pattern, content, match+1);
    ASSERT_EQ(match, -1);
  }
  {
    int match = findNext("eureka", content, 0);
    ASSERT_EQ(match, -1);
  }
}

TEST(strstrSearchTest, findAllPerLine) {
  std::string filepath("src/string_search_algorithms/test/files/_search.test.txt");
  FILE* file = fopen(filepath.c_str(), "r");
  if (file == nullptr) {
    std::cout << "lol: reading file " << filepath << " failed..." << std::endl;
    exit(-1);
  }
  char content[350];
  size_t read_bytes = fread(content, sizeof(char), 341, file);
  {
    int matches = findAllPerLine("keyword", content);
    ASSERT_EQ(matches, 4);
  }
  {
    int matches = findAllPerLine("eureka", content);
    ASSERT_EQ(matches, 0);
  }
}

TEST(strstrSearchTest, findAll) {
  std::string filepath("src/string_search_algorithms/test/files/_search.test.txt");
  FILE* file = fopen(filepath.c_str(), "r");
  if (file == nullptr) {
    std::cout << "lol: reading file " << filepath << " failed..." << std::endl;
    exit(-1);
  }
  char content[350];
  size_t read_bytes = fread(content, sizeof(char), 341, file);
  {
    int matches = findAll("keyword", content);
    ASSERT_EQ(matches, 9);
  }
  {
    int matches = findAllPerLine("eureka", content);
    ASSERT_EQ(matches, 0);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
