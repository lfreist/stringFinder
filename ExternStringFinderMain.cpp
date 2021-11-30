#include <iostream>

#include <stdio.h>

#include "src/ExternStringFinder.hpp"
#include "src/Timer.hpp"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    if (argc < 1) {
        return 1;
    }
    char* filename = argv[1];
    char* pattern = argv[2];


    FILE* fp = fopen(filename, "r");

    Timer timer;
    timer.start();

    vector<int> results = search(fp, pattern);

    timer.stop();

    fclose(fp);
    cout << endl << timer.elapsedSeconds() << endl;
    cout << "Found " << results.size() << " matches." << endl;

    return 0;
}
