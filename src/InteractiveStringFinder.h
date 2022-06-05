// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_INTERACTIVEFINDER_H_
#define SRC_INTERACTIVEFINDER_H_

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "InternStringFinder.h"

using std::string;
using std::vector;

class InteractiveStringFinder : public InternStringFinder {
 public:
  explicit InteractiveStringFinder(const string& filepath, bool verbose = false, unsigned nThreads = 1);
  explicit InteractiveStringFinder(bool verbose = false, unsigned nThreads = 1);
  void run();

 private:
  static void usage() ;
};

#endif  // SRC_INTERACTIVEFINDER_H_
