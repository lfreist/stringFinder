// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_THREADSAFEQUEUE_H_
#define SRC_UTILS_THREADSAFEQUEUE_H_

#include <gtest/gtest.h>

#include <queue>
#include <mutex>
#include <condition_variable>

namespace sf::sf_utils {

/**
 * @brief thread safe queue using std::queue
 * 
 */
template<class T>
class TSQueue {
 public:
  TSQueue();
  explicit TSQueue(unsigned maxSize);
  explicit TSQueue(unsigned maxSize, unsigned numberOfWriteThreads);
  ~TSQueue();

  void setMaxSize(ulong maxSize);

  /**
   * @brief push an element of type T onto queue
   * 
   * @param element 
   */
  void push(T element);

  /**
   * @brief pop an element from queue. Note that in constrast to std::queue, this pop returns the first element.
   * If the queue is empty, pop() waits for a push() call.
   * 
   * @return T 
   */
  T pop();

  T pop(T defaultReturn);

  /**
   * @brief returns, whether the Queue is empty or not
   * 
   * @return true  -> if _queue is empty
   * @return false -> if _queue is not empty
   */
  bool empty() const;

  /**
   * @brief returns number of elements hold by the queue
   *
   * @return int
   */
  int size() const;

  void setNumberOfWriteThreads(unsigned numberOfWriteThreads);
  void close();

  bool isClosed();

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
