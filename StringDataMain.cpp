// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <string>
#include <cmath>

#include "StringData.hpp"
#include "Timer.hpp"

using std::string;
using std::cout;
using std::endl;

string getInputFile(int argc, char** argv) {
    optind = 1;
    if (optind >= argc) {
        cout << "No input file provided" << endl;
        exit(1);
    }
    return argv[optind];
}

int main(int argc, char** argv) {
    Timer readfileTimer;
    Timer searchTimer;
    StringData sd;
    // input file is parsed here to avoid time measurement errors from
    //  parseCommandLineArguments() runtime
    // sd.parseCommandLineArguments(argc, argv);
    string file = getInputFile(argc, argv);
    readfileTimer.start();
    sd.readFile(file);
    readfileTimer.stop();

    searchTimer.start();
    sd.find("EUREKA", false);
    searchTimer.stop();

    cout << "Elapsed time reading file: "
    << readfileTimer.elapsedMicroseconds() << "µs" << endl;
    cout << "Elapsed time searching: "
    << searchTimer.elapsedMicroseconds() << "µs" << endl;

    return 0;
}
