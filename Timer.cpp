// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <chrono>
#include <string>

#include "Timer.hpp"

using std::string;

// ____________________________________________________________________________
Timer::Timer() {
    _running = false;
    _started = false;
}

// ____________________________________________________________________________
Timer::~Timer() = default;

// ____________________________________________________________________________
void Timer::start(bool restart) {
    if (_running && !restart) {
        return;
    }
    _running = true;
    _started = true;
    _start = std::chrono::system_clock::now();
}

// ____________________________________________________________________________
void Timer::stop() {
    if (!_running) {
        return;
    }
    _stop = std::chrono::system_clock::now();
    _running = false;
}

// ____________________________________________________________________________
long double Timer::elapsedNanoseconds() {
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
long double Timer::elapsedMicroseconds() {
    return elapsedNanoseconds() / 1000;
}

// ____________________________________________________________________________
long double Timer::elapsedMilliseconds() {
    return elapsedMicroseconds() / 1000;
}

// ____________________________________________________________________________
long double Timer::elapsedSeconds() {
    return elapsedMilliseconds() / 1000;
}
