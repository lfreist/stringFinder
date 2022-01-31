// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>
#include <algorithm>

#include "../src/StringFinder.h"

using std::string;

// ____________________________________________________________________________
TEST(StringFinderTest, readFile) {
  StringFinder sf;
  ASSERT_EQ(sf._data.size(), 0);
  sf.readFile(string("test/_stringFinderTest"));
  ASSERT_EQ(sf._data.size(), 5);
  sf.readFile(string("test/_stringFinderTest"));
  ASSERT_EQ(sf._data.size(), 5);
  sf.readFile(string("test/_stringFinderTest"), true);
  ASSERT_EQ(sf._data.size(), 10);
}

// ____________________________________________________________________________
TEST(StringDataTest, findExpression) {
  StringFinder sf;
  sf.readFile(string("test/_stringFinderTest"));
  {
    vector<const string *> result = sf.find("NotInFile", true);
    ASSERT_EQ(result.size(), 0);
  }
  {
    vector<const string *> result = sf.find("l", false);
    ASSERT_EQ(result.size(), 5);
    ASSERT_STREQ(
        result[0]->c_str(),
        "LinE with uppEr casE E");
    ASSERT_STREQ(
        result[1]->c_str(),
        "Line without upper case e");
    ASSERT_STREQ(
        result[2]->c_str(),
        "Lin3 without that charact3r");
    ASSERT_STREQ(
        result[3]->c_str(),
        "Line with word eureka in lower case");
    ASSERT_STREQ(
        result[4]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("E", false);
    ASSERT_EQ(result.size(), 4);
    ASSERT_STREQ(
        result[0]->c_str(),
        "LinE with uppEr casE E");
    ASSERT_STREQ(
        result[1]->c_str(),
        "Line without upper case e");
    ASSERT_STREQ(
        result[2]->c_str(),
        "Line with word eureka in lower case");
    ASSERT_STREQ(
        result[3]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("e", false);
    ASSERT_EQ(result.size(), 4);
    ASSERT_STREQ(
        result[0]->c_str(),
        "LinE with uppEr casE E");
    ASSERT_STREQ(
        result[1]->c_str(),
        "Line without upper case e");
    ASSERT_STREQ(
        result[2]->c_str(),
        "Line with word eureka in lower case");
    ASSERT_STREQ(
        result[3]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("E", true);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(
        result[0]->c_str(),
        "LinE with uppEr casE E");
    ASSERT_STREQ(
        result[1]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("e", true);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(
        result[0]->c_str(),
        "Line without upper case e");
    ASSERT_STREQ(
        result[1]->c_str(),
        "Line with word eureka in lower case");
  }
  {
    vector<const string *> result = sf.find("EurEka", true);
    ASSERT_EQ(result.size(), 1);
    ASSERT_STREQ(
        result[0]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("EurEka", false);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(
        result[0]->c_str(),
        "Line with word eureka in lower case");
    ASSERT_STREQ(
        result[1]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = sf.find("eureka", true);
    ASSERT_EQ(result.size(), 1);
    ASSERT_STREQ(
        result[0]->c_str(),
        "Line with word eureka in lower case");
  }
  {
    vector<const string *> result = sf.find("eureka", false);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(
        result[0]->c_str(),
        "Line with word eureka in lower case");
    ASSERT_STREQ(
        result[1]->c_str(),
        "LinE with word EurEka with uppEr casE E");
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
