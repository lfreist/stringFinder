// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <vector>
#include <string>
#include <iostream>

#include "InteractiveFinder.h"
#include "utils/InputParser.h"

using std::vector;
using std::string;

// _____________________________________________________________________________________________________________________
InteractiveFinder::InteractiveFinder(const string &filepath, bool verbose, unsigned int nThreads) : StringFinder(
    filepath,
    verbose,
    nThreads) {}

// _____________________________________________________________________________________________________________________
InteractiveFinder::InteractiveFinder(bool verbose, unsigned int nThreads) : StringFinder(verbose, nThreads) {}

// _____________________________________________________________________________________________________________________
void InteractiveFinder::run() {
  string input;
  bool parsingSucceeded;
  while (true) {
    std::cout << "> ";
    getline(cin, input);
    auto cmd = parse(input.begin(), input.end(), parsingSucceeded);
    if (!parsingSucceeded) {
      std::cout << "Error parsing..."
      continue;
    }
    if (cmd->type() == "search") {
      auto searchCmd = dynamic_cast<SearchCommand*>(cmd);
      auto result = find(searchCmd->getSearchPattern());
      std::cout << "found " << result.size() << "matches." << std::endl;
      if (searchCmd->getLimit() >= 0) {
        for (int i = 0; i < searchCmd->getLimit(); ++i) {
          std::cout << i+1 << ".\t" << result[i] << std::endl;
        }
      }
    }
    if (cmd->type() == "load") {
      auto loadCmd = dynamic_cast<LoadCommand*>(cmd);
    }
  }
}

// _____________________________________________________________________________________________________________________
void InteractiveFinder::usage() const {

}
