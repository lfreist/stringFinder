// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <chrono>  // NOLINT(build/c++11)

#include "Timer.h"

// ____________________________________________________________________________
Timer::Timer() {
  _running = false;
  _started = false;
  _duration = 0;
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
  if (restart) {
    _duration = 0;
  }
  _start = std::chrono::steady_clock::now();
}

// ____________________________________________________________________________
void Timer::stop() {
  if (!_running) {
    return;
  }
  _stop = std::chrono::steady_clock::now();
  _running = false;
  _duration += std::chrono::duration_cast<std::chrono::duration<double>>(_stop - _start).count();
}

// ____________________________________________________________________________
double Timer::elapsedSeconds() const {
  if (!_started || _running) {
    return -1;
  }
  return _duration;
}

// ____________________________________________________________________________
double Timer::elapsedMicroseconds() const {
  return elapsedSeconds() * 1000 * 1000;
}
