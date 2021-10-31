// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <chrono>  // NOLINT(build/c++11)

#include "WallTimer.hpp"


// ____________________________________________________________________________
WallTimer::WallTimer() {
    _running = false;
    _started = false;
}

// ____________________________________________________________________________
WallTimer::~WallTimer() = default;

// ____________________________________________________________________________
void WallTimer::start(bool restart) {
    if (_running && !restart) {
        return;
    }
    _running = true;
    _started = true;
    _start = std::chrono::system_clock::now();
}

// ____________________________________________________________________________
void WallTimer::stop() {
    if (!_running) {
        return;
    }
    _stop = std::chrono::system_clock::now();
    _running = false;
}

// ____________________________________________________________________________
long double WallTimer::elapsedNanoseconds() {
    if (!_started) {
        return -1;
    }
    if (_running) {
        _stop = std::chrono::system_clock::now();
    }
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
            _stop - _start).count();
}

// ____________________________________________________________________________
long double WallTimer::elapsedMicroseconds() {
    return elapsedNanoseconds() / 1000;
}

// ____________________________________________________________________________
long double WallTimer::elapsedMilliseconds() {
    return elapsedMicroseconds() / 1000;
}

// ____________________________________________________________________________
long double WallTimer::elapsedSeconds() {
    return elapsedMilliseconds() / 1000;
}
