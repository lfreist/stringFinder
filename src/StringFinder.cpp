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
  Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};
  vector<string::size_type>* matchPositionsPtr = &matchPositions;

  buildThreads(matchPositionsPtr);

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
  for (auto &mergeThread: _mergingThreads) {
    mergeThread.join();
  }

  delete matchPositionsPtr;

  if (_performanceMeasuring) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }

  return matchPositions;
}

vector<string::size_type> StringFinder::find(string &pattern, const std::function<int(int)>& transformer) {
  Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};
  vector<string::size_type>* matchPositionsPtr = &matchPositions;

  buildThreads(matchPositionsPtr, transformer);

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
  for (auto &mergeThread: _mergingThreads) {
    mergeThread.join();
  }

  delete matchPositionsPtr;

  if (_performanceMeasuring) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }

  return matchPositions;
}

vector<string::size_type> StringFinder::find(string &pattern, const std::function<string(string)>& transformer) {
  Timer timer;
  if (_performanceMeasuring) { timer.start(); }

  vector<string::size_type> matchPositions = {};
  vector<string::size_type>* matchPositionsPtr = &matchPositions;

  buildThreads(matchPositionsPtr, transformer);

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
  for (auto &mergeThread: _mergingThreads) {
    mergeThread.join();
  }

  delete matchPositionsPtr;

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

// --- protected stuff -------------------------------------------------------------------------------------------------
void StringFinder::buildThreads(vector<string::size_type> *matchPositionsPtr) {
  // TODO: implement
}

void StringFinder::buildThreads(vector<string::size_type> *matchPositionsPtr, const std::function<int(int)>& transformer) {
  // TODO: implement
}

void StringFinder::buildThreads(vector<string::size_type> *matchPositionsPtr,
                                const std::function<string(string)>& transformer) {
  // TODO: implement
}

void StringFinder::decompressChunks(sf_utils::TSQueue<FileChunk *> &popFromQueue,
                                    sf_utils::TSQueue<FileChunk *> &pushToQueue) {
  // TODO: implement
}

void StringFinder::transformChunks(const std::function<int(int)>& transformer,
                                   sf_utils::TSQueue<FileChunk *> &popFromQueue,
                                   sf_utils::TSQueue<FileChunk *> &pushToQueue) {
  // TODO: implement
}

void StringFinder::transformChunks(const std::function<string(string)>& transformer,
                                   sf_utils::TSQueue<FileChunk *> &popFromQueue,
                                   sf_utils::TSQueue<FileChunk *> &pushToQueue) {
  // TODO: implement
}

void StringFinder::searchChunks(string &pattern,
                                sf_utils::TSQueue<FileChunk *> &popFromQueue,
                                sf_utils::TSQueue<FileChunk *> &pushToQueue) {
  // TODO: implement
}

void StringFinder::mergeResults(vector<string::size_type> *machPositionsPtr,
                                sf_utils::TSQueue<FileChunk *> &popFromQueue,
                                sf_utils::TSQueue<FileChunk *> &pushToQueue) {
  // TODO: implement
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
  _availableChunkQueue.setNumberOfWriteThreads(_nMergingThreads);
}

}