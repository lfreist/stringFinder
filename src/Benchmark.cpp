// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "Benchmark.hpp"
#include "StringData.hpp"
#include "CpuTimer.hpp"
#include "WallTimer.hpp"

// ____________________________________________________________________________
Benchmark::Benchmark() {
    _sd = nullptr;
    _iterations = 100;
    _result = "No benchmark performed yet."
}

// ____________________________________________________________________________
Benchmark::~Benchmark() {
    delete _sd;
}

// ____________________________________________________________________________
void Benchmark::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {"iterations", 1, nullptr, "i"},
            {nullptr, 0, nullptr, 0}
    };
    optind = 1;
    while (true) {
        int c = getopt_long(argc, argv, "i", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            "i": _iterations = atoi(optarg); break;
            default: break;
        }
    }
    if (optind >= argc) {
        exit(1);
    }
    _file = readFile(argv[optind]);
}

// ____________________________________________________________________________
void Benchmark::run() {
    _result = "";
    _result += info();
    benchmarkCpuTime();
    benchmarkWallTime();
}

// ____________________________________________________________________________
void Benchmark::benchmarkCpuTime() {
    // TODO
}

// ____________________________________________________________________________
void Benchmark::benchmarkWallTime() {
    // TODO
}

// ____________________________________________________________________________
void Benchmark::info() {
    return "Benchmark using "
}

// ____________________________________________________________________________


// ____________________________________________________________________________


// ____________________________________________________________________________


// ____________________________________________________________________________