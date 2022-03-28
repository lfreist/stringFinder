// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_INTERACTIVEFINDER_H_
#define SRC_INTERACTIVEFINDER_H_

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "StringFinder.h"

using std::string;
using std::vector;

class InteractiveFinder : public StringFinder {
 public:
  explicit InteractiveFinder(const string &filepath, bool verbose = false, unsigned nThreads = 1);
  explicit InteractiveFinder(bool verbose = false, unsigned nThreads = 1);
  void run();

 private:
  void usage() const;
};

#endif  // SRC_INTERACTIVEFINDER_H_
