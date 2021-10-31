// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_CPUTIMER_HPP_
#define SRC_CPUTIMER_HPP_

#include <gtest/gtest.h>

#include <time.h>

class CpuTimer {
 public:
    CpuTimer();
    ~CpuTimer();
    void start(bool restart = false);
    void stop();
    double elapsedNanoseconds();
    double elapsedMicroseconds();
    double elapsedMilliseconds();
    double elapsedSeconds();
    double cpuTicks();
 private:
    clock_t _start;
    clock_t _stop;
    // holds if timer was started and not stopped yet
    bool _running;
    // holds if timer was started overall
    bool _started;

    FRIEND_TEST(CpuTimerTest, cpuTimerStartStop);
};

#endif  // SRC_CPUTIMER_HPP_
