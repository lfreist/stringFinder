// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <string>
#include <vector>

#include "src/StringData.hpp"
#include "src/Timer.hpp"

using std::string;
using std::vector;
using std::cout;
using std::endl;


void printResults(const vector<string>& results) {
    for (const string& result : results) {
        cout << result << endl;
    }
}


int main(int argc, char** argv) {
    StringData sd;

    sd.parseCommandLineArguments(argc, argv);

    return 0;
}
