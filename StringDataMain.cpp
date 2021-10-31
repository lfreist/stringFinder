// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <string>
#include <vector>

#include "src/StringData.hpp"
#include "src/WallTimer.hpp"

using std::string;
using std::vector;
using std::cout;
using std::endl;

int main(int argc, char** argv) {
    StringData sd;
    sd.parseCommandLineArguments(argc, argv);
    vector<string> results = sd.find("EUREKA", false);

    return 0;
}
