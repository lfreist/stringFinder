// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "ThreadSafeQueue.h"

// ____________________________________________________________________________________________________________________
template <class T>
TSQueue<T>::TSQueue(): _queue(), _queueMutex(), _condVar() {}

// ____________________________________________________________________________________________________________________
template <class T>
TSQueue<T>::~TSQueue() = default;

// ____________________________________________________________________________________________________________________
template <class T>
void TSQueue<T>::push(T element) {
  std::lock_guard<std::mutex> lock(_queueMutex);
  _queue.push(element);
  _condVar.notify_one();
}

// ____________________________________________________________________________________________________________________
template <class T>
T TSQueue<T>::pop() {
  std::unique_lock<std::mutex> lock(_queueMutex);
  while (_queue.empty()) {
    _condVar.wait(lock);
  }
  T element = _queue.front();
  _queue.pop();
  return element;
}

// ____________________________________________________________________________________________________________________
template <class T>
bool TSQueue<T>::empty() {
  std::lock_guard<std::mutex> lock(_queueMutex);
  return _queue.empty();
}
