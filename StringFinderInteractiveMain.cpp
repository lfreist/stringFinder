// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/InteractiveFinder.hpp"

int main(int argc, char **argv) {
  InteractiveFinder finder;
  finder.parseCommandLineArguments(argc, argv);
  finder.run();
}
