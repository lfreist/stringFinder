// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

/**
 * Task provides a multi-threaded processing of a provided function (std::function<void(T*)> task) using numThreads
 *  number of threads. The task gets it's data from a TSQueue that must know how many
 */

#pragma once

#include <thread>
#include <memory>

#include "ThreadSafeQueue.h"
#include "FileChunk.h"
#include "Timer.h"

#ifndef STRINGFINDER_SRC_UTILS_TASK_H_
#define STRINGFINDER_SRC_UTILS_TASK_H_

namespace sf::utils {

template<class T>
class Task {
 public:
  Task() = default;
  explicit Task(std::string name);
  explicit Task(std::string name, std::function<void(T*)> task, unsigned numThreads = 1);

  void start();

  void setSubsequentTask(std::string name, std::function<void(T*)> task, unsigned nThreads = 1);

  unsigned getNumThreads() const;

  void appendData(T data);
  void close();

  double getTotalTime() const;
  double getComputeTime() const;
  double getPullWaitTime() const;
  double getPushWaitTime() const;

  Task* getSubsequentTask();

  std::string& getName();

 private:
  void executer();

  std::string _name;
  std::unique_ptr<Task> _subsequentTask = nullptr;

  std::function<void(T*)> _task;
  unsigned _numThreads = 1;
  TSQueue<T> _dataQueue = {};

  std::vector<std::thread> _threads;

  // performance measuring stuff
  mutable std::unique_ptr<std::mutex> _addTimeMutex = std::make_unique<std::mutex>();
  bool _performanceMeasuring = true;
  Timer _totalTimer;
  double _totalTime = 0;
  double _computeTime = 0;
  double _pullWaitTime = 0;
  double _pushWaitTime = 0;
};


template<class T>
class TaskPipeline {
 public:
  TaskPipeline();

  Task<T>* getFirstTask();

  void run();
  [[nodiscard]] unsigned length() const;

  void addTask(std::string name, std::function<void(T*)> task, unsigned nThreads = 1);

 private:
  std::unique_ptr<Task<T>> _firstTask = nullptr;
  unsigned _length = 0;
};


}  // namespace sf::utils

#endif //STRINGFINDER_SRC_UTILS_TASK_H_
