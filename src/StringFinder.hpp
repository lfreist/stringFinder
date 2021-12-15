// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGFINDER_HPP_
#define SRC_STRINGFINDER_HPP_

#include <gtest/gtest.h>

#include <vector>
#include <string>

using std::vector;
using std::string;

// merge two sorted vectors. used for omp reduction in StringFinder.find()
void mergeArrays(vector<const string *> *out, vector<const string *> *in);

// Class StringData
class StringFinder {
 public:
  StringFinder();

  ~StringFinder();

  void parseCommandLineArguments(int argc, char **argv);

  void readFile(const string &path, bool append = false);

  vector<const string *> find(string expression, bool matchCase = false) const;

  void measurePerformance(const string &expression,
                          bool matchCase = false) const;

  int dataSize() const;

 private:
  vector<string> _data;

  FRIEND_TEST(StringFinderTest, readFile);
};

#endif  // SRC_STRINGFINDER_HPP_
