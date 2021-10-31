// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_BENCHMARK_HPP
#define SRC_BENCHMARK_HPP


class Benchmark {
 public:
    Benchmark();
    ~Benchmark();
    void parseCommandLineArguments(int argc, char** argv);
    void run();
 private:
    // default: 100
    int _iterations;
    string _file;
    StringData* _sd;
    string _result;
    void benchmarkCpuTime();
    void benchmarkWallTime();
    string info();
};


#endif // SRC_BENCHMARK_HPP
