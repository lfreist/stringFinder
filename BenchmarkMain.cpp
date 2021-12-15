// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/Benchmark.hpp"

int main(int argc, char **argv) {
  Benchmark bm;
  bm.parseCommandLineArguments(argc, argv);
  bm.run();

  return 0;
}
