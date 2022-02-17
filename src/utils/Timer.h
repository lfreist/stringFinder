// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_TIMER_H_
#define SRC_UTILS_TIMER_H_

#include <gtest/gtest.h>

#include <chrono>  // NOLINT(build/c++11)

class Timer {
 public:
  Timer();

  ~Timer();

  void start(bool restart = false);

  void stop();

  double elapsedMicroseconds();

  double elapsedSeconds();

 private:
  std::chrono::time_point<std::chrono::steady_clock> _start;
  std::chrono::time_point<std::chrono::steady_clock> _stop;
  // stores if timer was started and not stopped yet
  bool _running;
  // stores if timer was started overall
  bool _started;

  double _duration;

  FRIEND_TEST(WallTimerTest, wallTimerStartStop);
};

#endif  // SRC_UTILS_TIMER_H_
