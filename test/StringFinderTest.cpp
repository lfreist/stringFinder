// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>

#include "../src/InternStringFinder.h"

using std::string;

// _____________________________________________________________________________________________________________________
TEST(StringFinderTest, testConstructor) {
  const string filepath("test/_stringFinder.test.txt");
  bool verbose = true;
  unsigned nThreads = 4;
  {  // default argument values
    InternStringFinder string_finder(filepath);
    ASSERT_NE(string_finder._fileDataMap.find(filepath), string_finder._fileDataMap.end());
    ASSERT_EQ(string_finder._verbose, false);
    ASSERT_EQ(string_finder._nThreads, 1);
    ASSERT_EQ(string_finder._fileDataMap[filepath].size(), 5);
  }
  {  // also provide values for optional arguments
    InternStringFinder string_finder(filepath, verbose, nThreads);
    ASSERT_NE(string_finder._fileDataMap.find(filepath), string_finder._fileDataMap.end());
    ASSERT_EQ(string_finder._verbose, true);
    ASSERT_EQ(string_finder._nThreads, 4);
    ASSERT_EQ(string_finder._fileDataMap[filepath].size(), 5);
  }
  {  // constructor without filepath
    InternStringFinder string_finder(verbose, nThreads);
    ASSERT_TRUE(string_finder._fileDataMap.empty());
    ASSERT_EQ(string_finder._verbose, true);
    ASSERT_EQ(string_finder._nThreads, 4);
  }
}

// _____________________________________________________________________________________________________________________
TEST(StringFinderTest, testReadFile) {
  const string filepath("test/_stringFinder.test.txt");
  InternStringFinder string_finder(false, 1);
  string_finder.reset();
  string_finder.readFile(filepath);
  ASSERT_NE(string_finder._fileDataMap.find(filepath), string_finder._fileDataMap.end());
  ASSERT_EQ(string_finder._fileDataMap.size(), 1);
  ASSERT_EQ(string_finder._fileDataMap[filepath].size(), 5);
  // append file
  const string filepath2("test/_stringFinder2.test.txt");
  string_finder.readFile(filepath2);
  ASSERT_NE(string_finder._fileDataMap.find(filepath), string_finder._fileDataMap.end());
  ASSERT_NE(string_finder._fileDataMap.find(filepath2), string_finder._fileDataMap.end());
  ASSERT_EQ(string_finder._fileDataMap.size(), 2);
  ASSERT_EQ(string_finder._fileDataMap[filepath].size(), 5);
  ASSERT_EQ(string_finder._fileDataMap[filepath2].size(), 5);
  // overwrite files
  string_finder.readFile(filepath);
  ASSERT_NE(string_finder._fileDataMap.find(filepath), string_finder._fileDataMap.end());
  ASSERT_NE(string_finder._fileDataMap.find(filepath2), string_finder._fileDataMap.end());
  ASSERT_EQ(string_finder._fileDataMap.size(), 2);
  ASSERT_EQ(string_finder._fileDataMap[filepath].size(), 5);
}

// ____________________________________________________________________________
TEST(StringDataTest, testFind) {
  InternStringFinder string_finder(string("test/_stringFinder.test.txt"));
  {
    vector<const string *> result = string_finder.find("NotInFile", true);
    ASSERT_EQ(result.size(), 0);
    result = string_finder.find("NotInFile", false);
    ASSERT_EQ(result.size(), 0);
  }
  {  // searching uppercase "E"
    vector<const string *> result = string_finder.find("E", true);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(result[0]->c_str(), "LinE with uppEr casE E");
    ASSERT_STREQ(result[1]->c_str(), "LinE with word EurEka with uppEr casE E");
    result = string_finder.find("E", false);
    ASSERT_EQ(result.size(), 4);
    ASSERT_STREQ(result[0]->c_str(), "LinE with uppEr casE E");
    ASSERT_STREQ(result[1]->c_str(), "Line without upper case e");
    ASSERT_STREQ(result[2]->c_str(), "Line with word eureka in lower case");
    ASSERT_STREQ(result[3]->c_str(), "LinE with word EurEka with uppEr casE E");
  }
  {  // searching lowercase "e"
    vector<const string *> result = string_finder.find("e", true);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(result[0]->c_str(), "Line without upper case e");
    ASSERT_STREQ(result[1]->c_str(), "Line with word eureka in lower case");
    result = string_finder.find("e", false);
    ASSERT_EQ(result.size(), 4);
    ASSERT_STREQ(result[0]->c_str(), "LinE with uppEr casE E");
    ASSERT_STREQ(result[1]->c_str(), "Line without upper case e");
    ASSERT_STREQ(result[2]->c_str(), "Line with word eureka in lower case");
    ASSERT_STREQ(result[3]->c_str(), "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = string_finder.find("EurEka", true);
    ASSERT_EQ(result.size(), 1);
    ASSERT_STREQ(result[0]->c_str(), "LinE with word EurEka with uppEr casE E");
    result = string_finder.find("EurEka", false);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(result[0]->c_str(), "Line with word eureka in lower case");
    ASSERT_STREQ(result[1]->c_str(), "LinE with word EurEka with uppEr casE E");
  }
  {
    vector<const string *> result = string_finder.find("eureka", true);
    ASSERT_EQ(result.size(), 1);
    ASSERT_STREQ(result[0]->c_str(), "Line with word eureka in lower case");
    result = string_finder.find("eureka", false);
    ASSERT_EQ(result.size(), 2);
    ASSERT_STREQ(result[0]->c_str(), "Line with word eureka in lower case");
    ASSERT_STREQ(result[1]->c_str(), "LinE with word EurEka with uppEr casE E");
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
