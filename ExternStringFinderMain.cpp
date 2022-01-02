// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/ExternStringFinder.h"

int main(int argc, char **argv) {
  ExternStringFinder ef(10);
  ef.parseCommandLineArguments(argc, argv);
  ef.find();
  return 0;
}
