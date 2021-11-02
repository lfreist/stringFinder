// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGFINDER_HPP_
#define SRC_STRINGFINDER_HPP_

#include <gtest/gtest.h>

#include <vector>
#include <string>

using std::vector;
using std::string;

// Class StringData
class StringFinder {
 public:
    StringFinder();
    ~StringFinder();
    void parseCommandLineArguments(int argc, char** argv);
    void readFile(const string& path, bool deleteOld = true);
    vector<string> find(string expression, bool matchCase = true) const;
    void measurePerformance(const string& expression) const;
 private:
    vector<string> _data;

    FRIEND_TEST(StringFinderTest, readFile);
};

#endif  // SRC_STRINGFINDER_HPP_
