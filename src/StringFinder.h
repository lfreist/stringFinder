// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGFINDER_H_
#define SRC_STRINGFINDER_H_

#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

// merge two sorted vectors. used for omp reduction in StringFinder.find()
void mergeArrays(vector<const string *> *out, vector<const string *> *in);

// Class StringData
class StringFinder {
 public:
  explicit StringFinder(const string& filepath, bool verbose = false, unsigned nThreads = 1);
  explicit StringFinder(bool verbose = false, unsigned nThreads = 1);

  ~StringFinder();

  void readFile(const string &filepath, bool append = false);

  [[nodiscard]] vector<const string *> find(string expression, bool matchCase = false) const;

  void measurePerformance(const string &expression, bool matchCase = false) const;

  [[nodiscard]] unsigned long numLines() const;

 protected:
  std::map<const string, vector<string>> _fileDataMap;
  bool _verbose;
  unsigned _nThreads;

  FRIEND_TEST(StringFinderTest, testConstructor);
  FRIEND_TEST(StringFinderTest, testReadFile);
};

#endif  // SRC_STRINGFINDER_H_
