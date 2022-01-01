// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_THREADSAFEQUEUE_H_
#define SRC_THREADSAFEQUEUE_H_

#include <gtest/gtest.h>

#include <queue>
#include <mutex>


/**
 * @brief thread safe queue using std::queue
 * 
 */
template <class T>
class TSQueue {
 public:
  // default constructor
  TSQueue();
  // destructor
  ~TSQueue();

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

  /**
   * @brief returns, whether the Queue is empty or not
   * 
   * @return true  -> if _queue is empty
   * @return false -> if _queue is not empty
   */
  bool empty();

 private:
  std::queue<T> _queue;
  mutable std::mutex _queueMutex;
  std::condition_variable _condVar;

  FRIEND_TEST(TSQueueTest, Constructor);
  FRIEND_TEST(TSQueueTest, methodsTest);
};

#endif  // SRC_THREADSAFEQUEUE_H_
