// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <gtest/gtest.h>

// NOLINT [build/c++11]
#include <chrono>
#include <string>

using std::string;

class Timer {
 public:
    Timer();
    ~Timer();
    void start(bool restart = false);
    void stop();
    long double elapsedNanoseconds();
    long double elapsedMicroseconds();
    long double elapsedMilliseconds();
    long double elapsedSeconds();
 private:
    std::chrono::time_point<std::chrono::system_clock> _start;
    std::chrono::time_point<std::chrono::system_clock> _stop;
    // holds if timer was started and not stopped yet
    bool _running;
    // holds if timer was started overall
    bool _started;

    FRIEND_TEST(TimerTest, timerStartStop);
};

#endif  // TIMER_HPP_
