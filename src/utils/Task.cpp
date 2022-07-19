// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <optional>
#include <utility>

#include "Task.h"

namespace sf::utils {

// ===== Task ==========================================================================================================
template
class Task<FileChunk>;

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
template<class T>
Task<T>::Task(std::string name) {
  _name = std::move(name);
}

// _____________________________________________________________________________________________________________________
template<class T>
Task<T>::Task(std::string name, std::function<void(T*)> taskFunction, unsigned numThreads) {
  _name = std::move(name);
  _task = std::move(taskFunction);
  _numThreads = numThreads;
  _performanceMeasuring = true;
}

// _____________________________________________________________________________________________________________________
template<class T>
void Task<T>::start() {
  _threads.resize(_numThreads);
  if (_performanceMeasuring) { _totalTimer.start(); }
  for (auto& t: _threads) {
    t = std::thread(&Task<T>::executer, this);
  }
}

// _____________________________________________________________________________________________________________________
template<class T>
void Task<T>::setSubsequentTask(std::string name, std::function<void(T*)> task, unsigned nThreads) {
  _subsequentTask.reset(new Task(std::move(name), std::move(task), nThreads));
}

// _____________________________________________________________________________________________________________________
template<class T>
unsigned Task<T>::getNumThreads() const {
  return _numThreads;
}

// _____________________________________________________________________________________________________________________
template<class T>
void Task<T>::appendData(T data) {
  _dataQueue.push(std::move(data));
}

// _____________________________________________________________________________________________________________________
template<class T>
void Task<T>::close() {
  _dataQueue.close();
  for (auto& t: _threads) {
    if (t.joinable()) {
      t.join();
    }
  }
  if (_performanceMeasuring) {
    _totalTimer.stop();
    _totalTime = _totalTimer.elapsedSeconds();
  }
  if (_subsequentTask != nullptr) {
    _subsequentTask->close();
  }
}

// _____________________________________________________________________________________________________________________
template<class T>
double Task<T>::getTotalTime() const {
  return _totalTime;
}

// _____________________________________________________________________________________________________________________
template<class T>
double Task<T>::getComputeTime() const {
  return _computeTime;
}

// _____________________________________________________________________________________________________________________
template<class T>
double Task<T>::getPullWaitTime() const {
  return _pullWaitTime;
}

// _____________________________________________________________________________________________________________________
template<class T>
double Task<T>::getPushWaitTime() const {
  return _pushWaitTime;
}

// _____________________________________________________________________________________________________________________
template<class T>
Task<T> *Task<T>::getSubsequentTask() {
  return _subsequentTask.get();
}

// _____________________________________________________________________________________________________________________
template<class T>
std::string &Task<T>::getName() {
  return _name;
}

// ----- protected -----------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
template<class T>
void Task<T>::executer() {
  Timer computeTimer, pullWaitTimer, pushWaitTimer;
  while (true) {
    if (_performanceMeasuring) { pullWaitTimer.start(false); }
    std::optional<T> data = _dataQueue.pop();
    if (_performanceMeasuring) { pullWaitTimer.stop(); }
    if (!data) {
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    _task(&(data.value()));
    if (_performanceMeasuring) { computeTimer.stop(); }
    if (_subsequentTask != nullptr) {
      if (_performanceMeasuring) { pushWaitTimer.start(false); }
      _subsequentTask->appendData(std::move(data.value()));
      if (_performanceMeasuring) { pushWaitTimer.stop(); }
    }
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(*(_addTimeMutex));
    _computeTime += computeTimer.elapsedSeconds();
    _pullWaitTime += pullWaitTimer.elapsedSeconds();
    _pushWaitTime += pushWaitTimer.elapsedSeconds();
  }
}


// ===== TaskPipeline ==================================================================================================
template
class TaskPipeline<FileChunk>;

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
template<class T>
TaskPipeline<T>::TaskPipeline() = default;

// _____________________________________________________________________________________________________________________
template<class T>
Task<T> *TaskPipeline<T>::getFirstTask() {
  return _firstTask.get();
}

// _____________________________________________________________________________________________________________________
template<class T>
void TaskPipeline<T>::run() {
  auto* currentTask = _firstTask.get();
  for (unsigned i = 0; i < _length; ++i) {
    currentTask->start();
    currentTask = currentTask->getSubsequentTask();
  }

}

// _____________________________________________________________________________________________________________________
template<class T>
unsigned TaskPipeline<T>::length() const {
  return _length;
}

// _____________________________________________________________________________________________________________________
template<class T>
void TaskPipeline<T>::addTask(std::string name, std::function<void(T*)> task, unsigned nThreads) {
  if (_firstTask == nullptr) {
    _firstTask.reset(new Task(std::move(name), std::move(task), nThreads));
  } else {
    auto* currentLastTask = _firstTask.get();
    while (currentLastTask->getSubsequentTask() != nullptr) {
      currentLastTask = currentLastTask->getSubsequentTask();
    }
    currentLastTask->setSubsequentTask(std::move(name), std::move(task), nThreads);
  }
  _length++;
}

}  // namespace sf::utils