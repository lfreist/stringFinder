// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <string>
#include <cmath>
#include <numeric>

#include "src/StringData.hpp"
#include "src/WallTimer.hpp"
#include "src/CpuTimer.hpp"

using std::string;
using std::vector;
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


double mean(vector<double> data) {
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}


double stdev(vector<double> data, double mean) {
    std::vector<double> diff(data.size());
    std::transform(data.begin(),
                   data.end(),
                   diff.begin(),
                   [mean](double x) { return x - mean; });
    double sq_sum = std::inner_product(diff.begin(),
                                       diff.end(),
                                       diff.begin(),
                                       0.0);
    return std::sqrt(sq_sum / data.size());
}


void benchmarkReadingFile(int n, string file, bool cpu = false) {
    if (cpu) {
        cout << "Measuring CPU time on readFile operation..." << endl;
    } else {
        cout << "Measuring Wall time on readFile operation..." << endl;
    }
    cout << "File:\t\t" << file << endl;
    cout << "Iterations:\t" << n << endl;

    WallTimer timer;
    StringData sd;

    vector<double> microseconds;

    for (int i = 0; i < n; i++) {
        timer.start();
        sd.readFile(file, true);
        timer.stop();
        microseconds.push_back(timer.elapsedMicroseconds());
    }
    double microsecondsMean = mean(microseconds);
    double microsecondsStdev = stdev(microseconds, microsecondsMean);
    cout << "Benchmark:\t("
    << microsecondsMean << " +/- " << microsecondsStdev
    << ") µs" << endl;
}

int main(int argc, char** argv) {
    string file = getInputFile(argc, argv);
    benchmarkReadingFile(500000, file, false);

    return 0;
}
