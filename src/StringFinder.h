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

  void readFile(const string &filepath, string alias = "");

  [[nodiscard]] vector<const string *> find(string expression, bool matchCase = false) const;

  void measurePerformance(const string &expression, bool matchCase = false) const;

  template <typename ...ArgsT>
  [[nodiscard]] ulong numLines(ArgsT ...files) const;

  void reset();

 protected:
  [[nodiscard]] ulong numLines(std::initializer_list<string> files) const;
  std::map<const string, vector<string>> _fileDataMap;
  bool _verbose;
  unsigned _nThreads;

  FRIEND_TEST(StringFinderTest, testConstructor);
  FRIEND_TEST(StringFinderTest, testReadFile);
};

#endif  // SRC_STRINGFINDER_H_
