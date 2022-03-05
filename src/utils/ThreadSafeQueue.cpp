// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>


#include "ThreadSafeQueue.h"
#include "FileChunk.h"

template
class TSQueue<int>;
template
class TSQueue<FileChunk *>;
template
class TSQueue<unsigned>;

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::TSQueue() {
  _numberOfWriteThreads = 1;
  _closed = false;
}

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::TSQueue(int numberOfWriteThreads) {
  _numberOfWriteThreads = numberOfWriteThreads;
  _closed = _numberOfWriteThreads == 0;
}

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::~TSQueue() = default;

// _____________________________________________________________________________________________________________________
template<class T>
void TSQueue<T>::push(T element) {
  std::lock_guard<std::mutex> lock(_queueMutex);
  _queue.push(element);
  _condVar.notify_one();
}

// _____________________________________________________________________________________________________________________
template<class T>
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
template<class T>
bool TSQueue<T>::empty() const {
  std::lock_guard<std::mutex> lock(_queueMutex);
  return _queue.empty();
}

// _____________________________________________________________________________________________________________________
template<class T>
int TSQueue<T>::size() const {
  std::lock_guard<std::mutex> lock(_queueMutex);
  return _queue.size();
}

// _____________________________________________________________________________________________________________________
template<class T>
void TSQueue<T>::close() {
  if (isClosed()) { return; }
  std::lock_guard<std::mutex> lock(_numWriteThreadsMutex);
  _numberOfWriteThreads--;
  if (_numberOfWriteThreads <= 0) {
    _numberOfWriteThreads = 0;
    std::unique_lock<std::mutex> closedLock(_closedMutex);
    _closed = true;
    _condVar.notify_one();
  }
}

// _____________________________________________________________________________________________________________________
template<class T>
bool TSQueue<T>::isClosed() {
  std::unique_lock<std::mutex> lock(_closedMutex);
  return _closed;
}

// _____________________________________________________________________________________________________________________
template<class T>
void TSQueue<T>::setNumberOfWriteThreads(int numberOfWriteThreads) {
  std::lock_guard<std::mutex> lock(_numWriteThreadsMutex);
  _numberOfWriteThreads = numberOfWriteThreads;
}

// _____________________________________________________________________________________________________________________
template<class T>
T TSQueue<T>::pop(T defaultReturn) {
  std::unique_lock<std::mutex> queueLock(_queueMutex);
  while (_queue.empty()) {
    if (isClosed()) {
      return defaultReturn;
    }
    _condVar.wait(queueLock);
  }
  T element = _queue.front();
  _queue.pop();
  return element;
}