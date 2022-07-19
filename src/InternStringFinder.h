// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <vector>
#include <string>

#include "./StringFinder.h"
#include "./utils/readers/ISFBaseReader.h"

#ifndef SRC_STRINGFINDER_H_
#define SRC_STRINGFINDER_H_

namespace sf {

class InternStringFinder {
 public:
  InternStringFinder() = default;

  std::vector<uint64_t> find(
    const std::string &pattern,
    std::unique_ptr<utils::readers::BaseReader> reader,
    bool ignoreCase = false,
    bool performanceMeasuring = false,
    unsigned nDecompressionThreads = 0,
    unsigned nTransformThreads = 1,
    unsigned nSearchThreads = 1
  );

  std::string toString();

 private:
  StringFinder _sf;
};

}  // namespace sf

#endif  // SRC_STRINGFINDER_H_
