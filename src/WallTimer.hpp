// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_WALLTIMER_HPP_
#define SRC_WALLTIMER_HPP_

#include <gtest/gtest.h>

#include <chrono>  // NOLINT(build/c++11)

class WallTimer {
 public:
    WallTimer();
    ~WallTimer();
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

    FRIEND_TEST(WallTimerTest, wallTimerStartStop);
};

#endif  // SRC_WALLTIMER_HPP_
