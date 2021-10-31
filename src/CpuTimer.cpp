// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <time.h>

#include "CpuTimer.hpp"


// ____________________________________________________________________________
CpuTimer::CpuTimer() {
    _running = false;
    _started = false;
}

// ____________________________________________________________________________
CpuTimer::~CpuTimer() = default;

// ____________________________________________________________________________
void CpuTimer::start(bool restart) {
    if (_running && !restart) {
        return;
    }
    _running = true;
    _started = true;
    _start = clock();
}

// ____________________________________________________________________________
void CpuTimer::stop() {
    if (!_running) {
        return;
    }
    _stop = clock();
    _running = false;
}

// ____________________________________________________________________________
// probably not necessary and removable
double CpuTimer::elapsedNanoseconds() {
    return elapsedMicroseconds() * 1000;
}

// ____________________________________________________________________________
double CpuTimer::elapsedMicroseconds() {
    return elapsedMilliseconds() * 1000;
}

// ____________________________________________________________________________
double CpuTimer::elapsedMilliseconds() {
    return elapsedSeconds() * 1000;
}

// ____________________________________________________________________________
double CpuTimer::elapsedSeconds() {
    return cpuTicks()/CLOCKS_PER_SEC;
}

// ____________________________________________________________________________
double CpuTimer::cpuTicks() {
    if (!_started) {
        return -1;
    }
    if (_running) {
        _stop = clock();
    }
    return static_cast<double>(_stop, _start);
}
