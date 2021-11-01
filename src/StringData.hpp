// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGDATA_HPP_
#define SRC_STRINGDATA_HPP_

#include <gtest/gtest.h>

#include <vector>
#include <string>

using std::vector;
using std::string;

// Class StringData
class StringData {
 public:
    StringData();
    ~StringData();
    void parseCommandLineArguments(int argc, char** argv);
    void readFile(const string& path, bool deleteOld = true);
    vector<string> find(string expression, bool matchCase = true) const;
    void performance(const string& expression) const;
 private:
    vector<string> _data;

    FRIEND_TEST(StringDataTest, readFile);
};

#endif  // SRC_STRINGDATA_HPP_
