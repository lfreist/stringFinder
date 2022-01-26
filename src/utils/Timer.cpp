// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <chrono>  // NOLINT(build/c++11)

#include "Timer.h"

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
  _start = std::chrono::steady_clock::now();
}

// ____________________________________________________________________________
void Timer::stop() {
  if (!_running) {
    return;
  }
  _stop = std::chrono::steady_clock::now();
  _running = false;
}

// ____________________________________________________________________________
double Timer::elapsedSeconds() {
  if (!_started) {
    return -1;
  }
  if (_running) {
    _stop = std::chrono::steady_clock::now();
  }
  return std::chrono::duration_cast<std::chrono::duration<double>>(
      _stop - _start).count();
}

// ____________________________________________________________________________
double Timer::elapsedMicroseconds() {
  return elapsedSeconds() * 1000 * 1000;
}
