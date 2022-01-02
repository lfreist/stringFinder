// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>


#include "ThreadSafeQueue.h"
#include "String.h"

template class TSQueue<int>;
template class TSQueue<String*>;

// _____________________________________________________________________________________________________________________
template <class T>
  TSQueue<T>::TSQueue() = default;

// _____________________________________________________________________________________________________________________
template <class T>
TSQueue<T>::~TSQueue() = default;

// _____________________________________________________________________________________________________________________
template <class T>
void TSQueue<T>::push(T element) {
  std::lock_guard<std::mutex> lock(_queueMutex);
  _queue.push(element);
  _condVar.notify_one();
}

// _____________________________________________________________________________________________________________________
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

// _____________________________________________________________________________________________________________________
template <class T>
bool TSQueue<T>::empty() const {
  std::lock_guard<std::mutex> lock(_queueMutex);
  return _queue.empty();
}

// _____________________________________________________________________________________________________________________
template <class T>
int TSQueue<T>::size() const {
  std::lock_guard<std::mutex> lock(_queueMutex);
  return _queue.size();
}
