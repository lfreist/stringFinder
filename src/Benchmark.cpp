// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <unistd.h>

#include <numeric>
#include <iostream>
#include <cmath>
#include <utility>
#include <algorithm>

#include "Benchmark.hpp"
#include "StringFinder.hpp"
#include "Timer.hpp"


using std::cout;
using std::endl;

// ____________________________________________________________________________
Benchmark::Benchmark() {
    _iterations = 100;
    _result << "No benchmark performed yet.";
    _expression = "badminton";
    _matchCase = false;
}

// ____________________________________________________________________________
Benchmark::~Benchmark() = default;

// ____________________________________________________________________________
void Benchmark::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {"iterations", 1, nullptr, 'i'},
            {"expression", 1, nullptr, 'e'},
            {"matchCase", 0, nullptr, 'c'},
            {nullptr, 0, nullptr, 0}
    };
    optind = 1;
    while (true) {
        int c = getopt_long(argc, argv, "i:e:c", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'i':
                _iterations = atoi(optarg);
                break;
            case 'e':
                _expression = string(optarg);
                break;
            case 'c':
                _matchCase = true;
                break;
            default:
                break;
        }
    }
    if (optind >= argc) {
        exit(1);
    }
    _file = argv[optind];
}

// ____________________________________________________________________________
void Benchmark::run() {
    _result.str(string());
    _result << "Benchmark 'find(\"" << _expression << ", " << _matchCase
        << "\")':" << endl;
    _result << " Iterations: " << _iterations << endl;
    benchmarkWallTime();
    cout << _result.str();
}

// ____________________________________________________________________________
void Benchmark::reset() {
    _result << "No benchmark performed yet.";
}

// ____________________________________________________________________________
void Benchmark::benchmarkWallTime() {
    vector<double> measurements;
    Timer timer;
    StringFinder sf;
    sf.readFile(_file);
    for (int i = 0; i < _iterations; i++) {
        timer.start(true);
        sf.find(_expression, _matchCase);
        timer.stop();
        measurements.push_back(timer.elapsedSeconds());
    }
    _result << " Walltime\t[/s]:" << endl;
    _result << "  Mean:\t\t" << mean(measurements) << endl;
    _result << "  Variance:\t" << variance(measurements) << endl;
    _result << "  Stddev:\t" << stddev(measurements) << endl;
    _result << "  Max:\t\t" << max(measurements) << endl;
    _result << "  Min:\t\t" << min(measurements) << endl;
}

// ____________________________________________________________________________
// ____________________________________________________________________________
double mean(vector<double> data) {
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}


double variance(vector<double> data) {
    vector<double> diff(data.size());
    double calculatedMean = mean(data);
    std::transform(
            data.begin(),
            data.end(),
            diff.begin(),
            [calculatedMean](double x) { return x - calculatedMean; });
    double sq_sum = std::inner_product(
            diff.begin(),
            diff.end(),
            diff.begin(),
            0.0);
    return sq_sum / data.size();
}


double stddev(vector<double> data) {
    return sqrt(variance(std::move(data)));
}


double max(vector<double> data) {
    return *std::max_element(data.begin(), data.end());
}


double min(vector<double> data) {
    return *std::min_element(data.begin(), data.end());
}
