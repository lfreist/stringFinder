// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>

#include "InternStringFinder.h"

using std::vector;
using std::string;
using std::map;
using std::pair;
using std::ifstream;

namespace sf {

InternStringFinder::InternStringFinder() = default;

InternStringFinder::~InternStringFinder() = default;

void InternStringFinder::buildThreads(vector<string::size_type> &matchPositions, string &pattern) {
  StringFinder::buildThreads(matchPositions, pattern);
}

void InternStringFinder::buildThreads(vector<string::size_type> &matchPositions,
                                      string &pattern,
                                      const std::function<int(int)> &transformer) {
  StringFinder::buildThreads(matchPositions, pattern, transformer);
}

void InternStringFinder::buildThreads(vector<string::size_type> &matchPositions,
                                      string &pattern,
                                      const std::function<string(string)> &transformer) {
  StringFinder::buildThreads(matchPositions, pattern, transformer);
}

}  // namespace sf