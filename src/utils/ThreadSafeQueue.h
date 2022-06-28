// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_THREADSAFEQUEUE_H_
#define SRC_UTILS_THREADSAFEQUEUE_H_

#include <gtest/gtest.h>

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "FileChunk.h"

namespace sf::utils {

/**
 * @brief thread safe queue using std::queue
 * 
 */
template<class T>
class TSQueue {
 public:
  TSQueue() {
    _maxSize = 20;
    _closed = false;
  }
  explicit TSQueue(unsigned maxSize) {
    _maxSize = maxSize;
    _closed = false;
  }
  explicit TSQueue(unsigned maxSize, unsigned numberOfWriteThreads) {
    _maxSize = maxSize;
    _numberOfWriteThreads = numberOfWriteThreads;
    _closed = _numberOfWriteThreads == 0;
  }

  void setMaxSize(ulong maxSize) {
    std::unique_lock pushLock(_pushMutex);
    std::unique_lock popLock(_popMutex);
    _maxSize = maxSize;
  }

  /**
   * @brief push an element of type T onto queue
   * 
   * @param element 
   */
  void push(T element) {
    std::unique_lock lockPush(_pushMutex);
    while (_queue.size() >= _maxSize) {
      _pushCondVar.wait(lockPush);
    }
    std::unique_lock lockPop(_popMutex);
    _queue.push(std::move(element));
    _popCondVar.notify_one();
    _pushCondVar.notify_one();
  }

  /**
   * @brief pop an element from queue. Note that in contrast to std::queue, this pop returns the first element.
   * If the queue is empty, pop() waits for a push() call.
   * 
   * @return T 
   */
  std::optional<T> pop() {
    std::unique_lock popLock(_popMutex);
    while (_queue.empty()) {
      if (isClosed()) {
        return {};
      }
      _popCondVar.wait(popLock);
    }
    std::unique_lock pushLock(_pushMutex);
    T element = _queue.front();
    _queue.pop();
    _pushCondVar.notify_one();
    return element;
  }

  /**
   * @brief returns, whether the Queue is empty or not
   * 
   * @return true  -> if _queue is empty
   * @return false -> if _queue is not empty
   */
  bool empty() const {
    std::lock_guard lockPop(_popMutex);
    std::lock_guard lockPush(_pushMutex);
    return _queue.empty();
  }

  /**
   * @brief returns number of elements hold by the queue
   *
   * @return int
   */
  int size() const {
    std::lock_guard lockPop(_popMutex);
    std::lock_guard lockPush(_pushMutex);
    return _queue.size();
  }

  void setNumberOfWriteThreads(unsigned numberOfWriteThreads) {
    std::lock_guard lock(_numWriteThreadsMutex);
    _numberOfWriteThreads = numberOfWriteThreads;
  }

  void writerDone() {
    if (isClosed()) { return; }
    std::lock_guard lock(_numWriteThreadsMutex);
    _numberOfWriteThreads--;
    if (_numberOfWriteThreads <= 0) {
      _numberOfWriteThreads = 0;
      std::unique_lock closedLock(_closedMutex);
      _closed = true;
      _popCondVar.notify_all();
    }
  }

  void close() {
    if (isClosed()) { return; }
    std::unique_lock closedLock(_closedMutex);
    _closed = true;
    _popCondVar.notify_all();
  }

  bool isClosed() {
    std::unique_lock<std::mutex> lock(_closedMutex);
    return _closed;
  }

 private:
  unsigned _maxSize;

  std::queue<T> _queue;

  mutable std::mutex _pushMutex;
  mutable std::mutex _popMutex;
  mutable std::mutex _numWriteThreadsMutex;
  mutable std::mutex _closedMutex;

  std::condition_variable _popCondVar;
  std::condition_variable _pushCondVar;

  bool _closed;

  unsigned _numberOfWriteThreads = 1;

  FRIEND_TEST(TSQueueTest, Constructor);
  FRIEND_TEST(TSQueueTest, methodsTest);
};

}

#endif  // SRC_UTILS_THREADSAFEQUEUE_H_
