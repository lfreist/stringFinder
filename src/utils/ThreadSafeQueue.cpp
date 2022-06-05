// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <vector>

#include "ThreadSafeQueue.h"
#include "FileChunk.h"

using std::vector;

namespace sf::sf_utils {

template
class TSQueue<int>;
template
class TSQueue<FileChunk *>;
template
class TSQueue<unsigned>;
template
class TSQueue<vector<unsigned>>;
template
class TSQueue<vector<std::string::size_type>>;

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::TSQueue() {
  _maxSize = 5;
  _closed = false;
}

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::TSQueue(unsigned maxSize) {
  _maxSize = maxSize;
  _closed = false;
}

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::TSQueue(unsigned maxSize, unsigned numberOfWriteThreads) {
  _maxSize = maxSize;
  _numberOfWriteThreads = numberOfWriteThreads;
  _closed = _numberOfWriteThreads == 0;
}

// _____________________________________________________________________________________________________________________
template<class T>
TSQueue<T>::~TSQueue() = default;

// _____________________________________________________________________________________________________________________
template<class T>
void TSQueue<T>::setMaxSize(ulong maxSize) {
  std::unique_lock pushLock(_pushMutex);
  std::unique_lock popLock(_popMutex);
  _maxSize = maxSize
}

// _____________________________________________________________________________________________________________________
template<class T>
void TSQueue<T>::push(T element) {
  std::unique_lock lockPush(_pushMutex);
  while (_queue.size() >= _maxSize) {
    _pushCondVar.wait(lockPush);
  }
  std::unique_lock lockPop(_popMutex);
  _queue.push(element);
  _popCondVar.notify_one();
  _pushCondVar.notify_one();
}

// _____________________________________________________________________________________________________________________
template<class T>
T TSQueue<T>::pop() {
  std::unique_lock lock(_popMutex);
  while (_queue.empty()) {
    _popCondVar.wait(lock);
  }
  std::unique_lock lockPush(_pushMutex);
  T element = _queue.front();
  _queue.pop();
  _pushCondVar.notify_one();
  return element;
}

// _____________________________________________________________________________________________________________________
template<class T>
bool TSQueue<T>::empty() const {
  std::lock_guard lockPop(_popMutex);
  std::lock_guard lockPush(_pushMutex);
  return _queue.empty();
}

// _____________________________________________________________________________________________________________________
template<class T>
int TSQueue<T>::size() const {
  std::lock_guard lockPop(_popMutex);
  std::lock_guard lockPush(_pushMutex);
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
    _popCondVar.notify_all();
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
void TSQueue<T>::setNumberOfWriteThreads(unsigned numberOfWriteThreads) {
  std::lock_guard<std::mutex> lock(_numWriteThreadsMutex);
  _numberOfWriteThreads = numberOfWriteThreads;
}

// _____________________________________________________________________________________________________________________
template<class T>
T TSQueue<T>::pop(T defaultReturn) {
  std::unique_lock popLock(_popMutex);
  while (_queue.empty()) {
    if (isClosed()) {
      return defaultReturn;
    }
    _popCondVar.wait(popLock);
  }
  std::unique_lock pushLock(_pushMutex);
  T element = _queue.front();
  _queue.pop();
  _pushCondVar.notify_one();
  return element;
}

}