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

    cout << "Elapsed time reading file: " << readfileTimer.elapsedNanoseconds() << "ns" << endl;
    cout << "Elapsed time searching: " << searchTimer.elapsedNanoseconds() << "ns" << endl;

    Timer timer;
    timer.start();
    int counter = 0;
    double test, test2;
    while(timer.elapsedMilliseconds() < 5000000.0)
    {
        counter++;
        test = std::cos(counter / M_PI);
        test2 = std::sin(counter / M_PI);
    }
    timer.stop();
    std::cout << "Milliseconds: " << timer.elapsedMilliseconds() << std::endl;

    return 0;
}
