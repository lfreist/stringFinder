// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_BENCHMARK_HPP_
#define SRC_BENCHMARK_HPP_
#include <vector>
#include <string>
#include <sstream>

#include "StringData.hpp"


using std::vector;
using std::string;
using std::stringstream;


class Benchmark {
 public:
    Benchmark();
    ~Benchmark();
    void parseCommandLineArguments(int argc, char** argv);
    void run();
    void reset();
 private:
    // default: 100
    int _iterations;
    string _file;
    StringData* _sd;
    stringstream _result;
    void benchmarkCpuTime();
    void benchmarkWallTime();
};

double mean(vector<double> data);
double stddev(vector<double> data);

#endif  // SRC_BENCHMARK_HPP_
