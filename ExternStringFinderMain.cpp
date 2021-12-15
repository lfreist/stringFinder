// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/ExternStringFinder.hpp"

int main(int argc, char **argv) {
  ExternFinder ef;
  ef.parseCommandLineArguments(argc, argv);
  ef.find();
  return 0;
}
