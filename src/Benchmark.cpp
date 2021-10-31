// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>

#include <numeric>
#include <iostream>
#include <cmath>

#include "Benchmark.hpp"
#include "StringData.hpp"
#include "CpuTimer.hpp"
#include "WallTimer.hpp"


using std::cout;
using std::endl;

// ____________________________________________________________________________
Benchmark::Benchmark() {
    _sd = nullptr;
    _iterations = 100;
    _result << "No benchmark performed yet.";
}

// ____________________________________________________________________________
Benchmark::~Benchmark() {
    delete _sd;
}

// ____________________________________________________________________________
void Benchmark::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {"iterations", 1, nullptr, 'i'},
            {nullptr, 0, nullptr, 0}
    };
    optind = 1;
    while (true) {
        char c = getopt_long(argc, argv, "i", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'i': _iterations = atoi(optarg); break;
            default: break;
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
    _result << "Benchmark using " << _iterations << ":" << endl;
    benchmarkCpuTime();
    benchmarkWallTime();
    cout << "RESULT:" << endl << _result.str() << endl;
}

// ____________________________________________________________________________
void Benchmark::reset() {
    _result << "No benchmark performed yet.";
    _sd = nullptr;
}


// TODO(lfreist): am I using clock() and CLOCKS_PER_SEC correctly? stddev is
//                 huge...
//                 Why is CPU Time > Wall Time?
// ____________________________________________________________________________
void Benchmark::benchmarkCpuTime() {
    _result << "CPU Time find(\"Eureka\"):" << endl;
    vector<double> measurements;
    CpuTimer cpuTimer;
    _sd = new StringData();
    for (int i = 0; i < _iterations; i++) {
        cpuTimer.start();
        _sd->find("Eureka");
        cpuTimer.stop();
        measurements.push_back(cpuTimer.elapsedMicroseconds());
    }
    double vMean = mean(measurements);
    double vStddev = stddev(measurements);
    _result << "per call:\t(" << vMean << " +/- " << vStddev << ") µs" << endl;
}

// ____________________________________________________________________________
void Benchmark::benchmarkWallTime() {
    _result << "Wall Time find(\"Eureka\"):" << endl;
    vector<double> measurements;
    WallTimer wallTimer;
    _sd = new StringData();
    for (int i = 0; i < _iterations; i++) {
        wallTimer.start(true);
        _sd->find("Eureka");
        wallTimer.stop();
        measurements.push_back(wallTimer.elapsedMicroseconds());
    }
    double vMean = mean(measurements);
    double vStddev = stddev(measurements);
    _result << "per call:\t(" << vMean << " +/- " << vStddev << ") µs" << endl;
}

// ____________________________________________________________________________
// ____________________________________________________________________________
double mean(vector<double> data) {
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}


double stddev(vector<double> data) {
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
    return std::sqrt(sq_sum / data.size());
}
