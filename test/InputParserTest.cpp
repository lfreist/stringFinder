// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>

#include "../src/utils/InputParser.h"

using std::string;

// _____________________________________________________________________________________________________________________
TEST(InputParserTest, searchQuery) {
  {
    const string query("SEARCH keyword");
    bool success = false;
    auto result = dynamic_cast<SearchCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_searchPattern, "keyword");
    ASSERT_TRUE(result->_targetFile.empty());
    ASSERT_EQ(result->_limit, -1);
  }
  {
    const string query("SEARCH keyword LIMIT 5");
    bool success = false;
    auto result = dynamic_cast<SearchCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_searchPattern, "keyword");
    ASSERT_TRUE(result->_targetFile.empty());
    ASSERT_EQ(result->_limit, 5);
  }
  {
    const string query("SEARCH keyword IN file");
    bool success = false;
    auto result = dynamic_cast<SearchCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_searchPattern, "keyword");
    ASSERT_EQ(result->_targetFile, "file");
    ASSERT_EQ(result->_limit, -1);
  }
  {
    const string query("SEARCH keyword IN file LIMIT 23");
    bool success = false;
    auto result = dynamic_cast<SearchCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_searchPattern, "keyword");
    ASSERT_EQ(result->_targetFile, "file");
    ASSERT_EQ(result->_limit, 23);
  }
}

// _____________________________________________________________________________________________________________________
TEST(InputParserTest, loadQuery) {
  {
    const string query("LOAD path/to/file");
    bool success = false;
    auto result = dynamic_cast<LoadCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_filepath, "path/to/file");
    ASSERT_TRUE(result->_alias.empty());
  }
  {
    const string query("LOAD path/to/file.txt AS fileA");
    bool success = false;
    auto result = dynamic_cast<LoadCommand *>(parse(query.begin(), query.end(), success));
    ASSERT_TRUE(success);
    ASSERT_EQ(result->_filepath, "path/to/file.txt");
    ASSERT_EQ(result->_alias, "fileA");
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}