// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGFINDER_H_
#define SRC_STRINGFINDER_H_

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
  explicit StringFinder(const string& filepath, bool verbosity = false);

  ~StringFinder();

  void readFile(const string &path, bool append = false);

  [[nodiscard]] vector<const string *> find(string expression, bool matchCase = false) const;

  void measurePerformance(const string &expression, bool matchCase = false) const;

  [[nodiscard]] unsigned long dataSize() const;

 private:
  vector<string> _data;
  string _filepath;
  bool _verbose;

  FRIEND_TEST(StringFinderTest, readFile);
};

#endif  // SRC_STRINGFINDER_H_
