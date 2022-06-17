// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>

#include "./utils/ThreadSafeQueue.h"

#include "./StringFinder.h"

using std::string;
using std::vector;

namespace sf {

// --- public stuff ----------------------------------------------------------------------------------------------------
StringFinder::StringFinder() = default;

StringFinder::~StringFinder() = default;

void StringFinder::setNumberOfDecompressionThreads(unsigned int nDecompressionThreads) {
  _nDecompressionThreads = nDecompressionThreads > 0 ? nDecompressionThreads : 0;
  setNumberOfQueueWriteThreads();
}

void StringFinder::setNumberOfTransformationThreads(unsigned int nTransformationThreads) {
  _nTransformationThreads = nTransformationThreads > 0 ? nTransformationThreads : 0;
  setNumberOfQueueWriteThreads();
}

void StringFinder::setNumberOfSearchingThreads(unsigned int nSearchingThreads) {
  _nSearchingThreads = nSearchingThreads > 0 ? nSearchingThreads : 0;
}

void StringFinder::setNumberOfMergingThreads(unsigned int nMergingThreads) {
  _nMergingThreads = nMergingThreads;
}

void StringFinder::enablePerformanceMeasuring(bool enabled) {
  if (enabled | _performanceMeasuring) { _totalTime = 0; }
  _performanceMeasuring = enabled;
}

void StringFinder::setNumberOfFileChunks(unsigned int nFileChunks) {
  _nFileChunks = nFileChunks;
  _availableChunkQueue.setMaxSize(_nFileChunks);
  _toBeDecompressedChunkQueue.setMaxSize(_nFileChunks);
  _toBeTransformedChunkQueue.setMaxSize(_nFileChunks);
  _toBeSearchedChunkQueue.setMaxSize(_nFileChunks);
  _partialResultsQueue.setMaxSize(_nFileChunks);
}

vector<string::size_type> StringFinder::find(string &pattern) {
  utils::Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};

  buildThreads(matchPositions, pattern);

  _readingThread.join();
  for (auto &decompressionThread: _decompressionThreads) {
    decompressionThread.join();
  }
  for (auto &transformThread: _transformationThreads) {
    transformThread.join();
  }
  for (auto &searchThread: _searchingThreads) {
    searchThread.join();
  }
  _mergingThread.join();

  if (_performanceMeasuring) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }

  return matchPositions;
}

vector<string::size_type> StringFinder::find(string &pattern, const std::function<int(int)>& transformer) {
  utils::Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};

  buildThreads(matchPositions, pattern, transformer);

  _readingThread.join();
  for (auto &decompressionThread: _decompressionThreads) {
    decompressionThread.join();
  }
  for (auto &transformThread: _transformationThreads) {
    transformThread.join();
  }
  for (auto &searchThread: _searchingThreads) {
    searchThread.join();
  }
  _mergingThread.join();

  if (_performanceMeasuring) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }

  return matchPositions;
}

vector<string::size_type> StringFinder::find(string &pattern, const std::function<string(string)>& transformer) {
  utils::Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};

  buildThreads(matchPositions, pattern, transformer);

  _readingThread.join();
  for (auto &decompressionThread: _decompressionThreads) {
    decompressionThread.join();
  }
  for (auto &transformThread: _transformationThreads) {
    transformThread.join();
  }
  for (auto &searchThread: _searchingThreads) {
    searchThread.join();
  }
  _mergingThread.join();

  if (_performanceMeasuring) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }

  return matchPositions;
}

string StringFinder::toString() const {
  // TODO: implement
  return "";
}

void StringFinder::buildThreads(vector<string::size_type> &matchPositions, string &pattern) {
  // reader thread must be created in child classes
  // decompression thread must be created in child classes
  // transformation threads must be created in child classes
  // searching threads must be created in child classes
  _mergingThread = std::thread(&StringFinder::mergeResults, this, std::ref(matchPositions));
}

void StringFinder::buildThreads(vector<string::size_type> &matchPositions,
                                string &pattern,
                                const std::function<int(int)> &transformer) {}

void StringFinder::buildThreads(vector<string::size_type> &matchPositions,
                                string &pattern,
                                const std::function<string(string)> &transformer) {}

// --- protected stuff -------------------------------------------------------------------------------------------------
void StringFinder::readChunks(std::istream &input,
                              utils::TSQueue<utils::FileChunk *> &popFromQueue,
                              utils::TSQueue<utils::FileChunk *> &pushToQueue) {}

void StringFinder::readChunks(std::string_view &input,
                              utils::TSQueue<utils::FileChunk *> &popFromQueue,
                              utils::TSQueue<utils::FileChunk *> &pushToQueue) {}

void StringFinder::decompressChunks(utils::TSQueue<utils::FileChunk *> &popFromQueue,
                                    utils::TSQueue<utils::FileChunk *> &pushToQueue) {
  utils::Timer waitTimer;
  utils::Timer computeTimer;
  while (true) {
    if (_performanceMeasuring) { waitTimer.start(false); }
    auto currentChunk = popFromQueue.pop();
    if (_performanceMeasuring) { waitTimer.stop(); }
    if (!currentChunk.has_value()) {
      pushToQueue.close();
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    currentChunk.value()->decompress();
    if (_performanceMeasuring) { computeTimer.stop(); }
    pushToQueue.push(currentChunk.value());
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(_decompressionTimerMutex);
    _totalDecompressionWaitTime += waitTimer.elapsedSeconds();
    _totalDecompressionTime += computeTimer.elapsedSeconds();
  }
}

void StringFinder::transformChunks(const std::function<int(int)>& transformer,
                                   utils::TSQueue<utils::FileChunk *> &popFromQueue,
                                   utils::TSQueue<utils::FileChunk *> &pushToQueue) {
  utils::Timer waitTimer;
  utils::Timer computeTimer;
  while (true) {
    if (_performanceMeasuring) { waitTimer.start(false); }
    auto currentChunk = popFromQueue.pop();
    if (_performanceMeasuring) { waitTimer.stop(); }
    if (!currentChunk.has_value()) {
      pushToQueue.close();
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    currentChunk.value()->transform(transformer);
    if (_performanceMeasuring) { computeTimer.stop(); }
    pushToQueue.push(currentChunk.value());
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(_transformationTimerMutex);
    _totalTransformationWaitTime += waitTimer.elapsedSeconds();
    _totalTransformationTime += computeTimer.elapsedSeconds();
  }
}

void StringFinder::transformChunks(const std::function<string(string)>& transformer,
                                   utils::TSQueue<utils::FileChunk *> &popFromQueue,
                                   utils::TSQueue<utils::FileChunk *> &pushToQueue) {
  utils::Timer waitTimer;
  utils::Timer computeTimer;
  while (true) {
    if (_performanceMeasuring) { waitTimer.start(false); }
    auto currentChunk = popFromQueue.pop();
    if (_performanceMeasuring) { waitTimer.stop(); }
    if (!currentChunk.has_value()) {
      pushToQueue.close();
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    currentChunk.value()->transform(transformer);
    if (_performanceMeasuring) { computeTimer.stop(); }
    pushToQueue.push(currentChunk.value());
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(_transformationTimerMutex);
    _totalTransformationWaitTime += waitTimer.elapsedSeconds();
    _totalTransformationTime += computeTimer.elapsedSeconds();
  }
}

void StringFinder::searchChunks(string &pattern,
                                utils::TSQueue<utils::FileChunk *> &popFromQueue,
                                utils::TSQueue<utils::FileChunk *> &pushToQueue) {
  utils::Timer waitTimer;
  utils::Timer computeTimer;
  while (true) {
    if (_performanceMeasuring) { waitTimer.start(false); }
    auto currentChunk = popFromQueue.pop();
    if (_performanceMeasuring) { waitTimer.stop(); }
    if (!currentChunk.has_value()) {
      pushToQueue.close();
      _partialResultsQueue.close();
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    vector<ulong> matches = currentChunk.value()->searchAllPerLine(pattern);
    if (_performanceMeasuring) { computeTimer.stop(); }
    _partialResultsQueue.push(matches);
    pushToQueue.push(currentChunk.value());
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(_transformationTimerMutex);
    _totalSearchingWaitTime += waitTimer.elapsedSeconds();
    _totalSearchingTime += computeTimer.elapsedSeconds();
  }
}

void StringFinder::mergeResults(vector<string::size_type> &matchPositions) {
  utils::Timer waitTimer;
  utils::Timer computeTimer;
  while (true) {
    if (_performanceMeasuring) { waitTimer.start(false); }
    auto partialResult = _partialResultsQueue.pop();
    if (_performanceMeasuring) { waitTimer.stop(); }
    if (!partialResult.has_value()) {
      break;
    }
    if (_performanceMeasuring) { computeTimer.start(false); }
    std::merge(matchPositions.begin(), matchPositions.end(),
               partialResult.value().begin(), partialResult.value().end(),
               matchPositions.begin());
    if (_performanceMeasuring) { computeTimer.stop(); }
  }
  if (_performanceMeasuring) {
    std::unique_lock timerLock(_transformationTimerMutex);
    _totalMergingWaitTime += waitTimer.elapsedSeconds();
    _totalMergingTime += computeTimer.elapsedSeconds();
  }
}

// --- private stuff ---------------------------------------------------------------------------------------------------
void StringFinder::setNumberOfQueueWriteThreads() {
  if (_nDecompressionThreads > 0) {
    _toBeDecompressedChunkQueue.setNumberOfWriteThreads(1);
    if (_nTransformationThreads > 0) {
      _toBeTransformedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
    } else {
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
    }
  } else {
    if (_nTransformationThreads > 0) {
      _toBeTransformedChunkQueue.setNumberOfWriteThreads(1);
    } else {
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(1);
    }
  }
  _partialResultsQueue.setNumberOfWriteThreads(_nSearchingThreads);
  _availableChunkQueue.setNumberOfWriteThreads(_nSearchingThreads);
}

}