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
InteractiveStringFinder::InteractiveStringFinder(const string &filepath, bool verbose, unsigned int nThreads) : InternStringFinder(
    filepath,
    verbose,
    nThreads) {}

// _____________________________________________________________________________________________________________________
InteractiveStringFinder::InteractiveStringFinder(bool verbose, unsigned int nThreads) : InternStringFinder(verbose, nThreads) {}

// _____________________________________________________________________________________________________________________
void InteractiveStringFinder::run() {
  string input;
  while (true) {
    std::cout << "> ";
    getline(std::cin, input);
    if ((input == "exit") | (input == "quit")) {
      break;
    }
    std::optional<Command*> cmd = parse(input.begin(), input.end());
    if (cmd.has_value()) {
      cmd.value()->execute(*this);
    }
  }
}

// _____________________________________________________________________________________________________________________
void InteractiveStringFinder::usage() {
  std::cout << "Usage" << std::endl;
}
