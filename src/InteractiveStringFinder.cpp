// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <vector>
#include <string>
#include <iostream>

#include "InteractiveStringFinder.h"
#include "utils/InputParser.h"

using std::vector;
using std::string;

// _____________________________________________________________________________________________________________________
InteractiveStringFinder::InteractiveStringFinder(const string &filepath, bool verbose, unsigned int nThreads) : StringFinder(
    filepath,
    verbose,
    nThreads) {}

// _____________________________________________________________________________________________________________________
InteractiveStringFinder::InteractiveStringFinder(bool verbose, unsigned int nThreads) : StringFinder(verbose, nThreads) {}

// _____________________________________________________________________________________________________________________
void InteractiveStringFinder::run() {
  string input;
  bool parsingSucceeded;
  while (true) {
    std::cout << "> ";
    getline(std::cin, input);
    auto cmd = parse(input.begin(), input.end(), parsingSucceeded);
    if ((input == "exit") | (input == "quit")) {
      break;
    }
    if (!parsingSucceeded) {
      std::cout << "Error parsing..." << std::endl;
      continue;
    }
    // TODO: how do I get if parser returned SearchCommand or LoadCommand
    if (cmd->type() == "search") {
      auto searchCmd = dynamic_cast<SearchCommand*>(cmd);
      auto result = find(searchCmd->getSearchPattern());
      std::cout << "found " << result.size() << " matches." << std::endl;
      auto limit = searchCmd->getLimit() >= 0 ? searchCmd->getLimit() : result.size();
      for (unsigned i = 0; i < limit; ++i) {
        std::cout << i+1 << ".\t" << &result[i] << std::endl;
      }
    } else if (cmd->type() == "load") {
      auto loadCmd = dynamic_cast<LoadCommand*>(cmd);
      readFile(loadCmd->getFilePath(), true);
    }
  }
}

// _____________________________________________________________________________________________________________________
void InteractiveStringFinder::usage() const {
  std::cout << "Usage" << std::endl;
}
