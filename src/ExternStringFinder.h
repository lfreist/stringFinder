// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <functional>

#include "StringFinder.h"

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

namespace sf {

class ExternStringFinder {
 public:
  ExternStringFinder();

  std::vector<ulong> find(const std::string &pattern,
                          const std::string &filePath,
                          const std::string &metaFilePath = "",
                          bool ignoreCase = false,
                          bool performanceMeasuring = false,
                          unsigned nDecompressionThreads = 1,
                          unsigned nTransformThreads = 1,
                          unsigned nSearchThreads = 1);

  std::string toString();

 private:
  StringFinder _sf;
};

std::ostream &operator<<(std::ostream &os, const ExternStringFinder &esf);

}  // namespace sf

#endif  // SRC_EXTERNSTRINGFINDER_H_
