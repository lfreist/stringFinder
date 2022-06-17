// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>

#include "./utils/ThreadSafeQueue.h"
#include "./utils/FileChunk.h"
#include "./utils/Timer.h"

using std::vector;
using std::string;

#ifndef STRINGFINDER_SRC_STRINGFINDERBASE_H_
#define STRINGFINDER_SRC_STRINGFINDERBASE_H_

namespace sf {

class StringFinder {
 public:
  StringFinder();
  ~StringFinder();

  void setNumberOfDecompressionThreads(unsigned nDecompressionThreads);
  void setNumberOfTransformationThreads(unsigned nTransformationThreads);
  void setNumberOfSearchingThreads(unsigned nSearchingThreads);
  void setNumberOfMergingThreads(unsigned nMergingThreads);

  void enablePerformanceMeasuring(bool enabled);
  void setNumberOfFileChunks(unsigned nFileChunks);

  [[nodiscard]] vector<string::size_type> find(string &pattern);
  [[nodiscard]] vector<string::size_type> find(string &pattern, const std::function<int(int)>& transformer);
  [[nodiscard]] vector<string::size_type> find(string &pattern, const std::function<string(string)>& transformer);

  string toString() const;

  virtual void buildThreads(vector<string::size_type> &matchPositions, string &pattern);
  virtual void buildThreads(vector<string::size_type> &matchPositions, string &pattern, const std::function<int(int)>& transformer);
  virtual void buildThreads(vector<string::size_type> &matchPositions, string &pattern, const std::function<string(string)>& transformer);

 protected:
  virtual void readChunks(std::istream &input, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  virtual void readChunks(std::string_view &input, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  void decompressChunks(utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  void transformChunks(const std::function<int(int)>& transformer, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  // TODO: for string_view?
  void transformChunks(const std::function<string(string)>& transformer, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  void searchChunks(string &pattern, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  void mergeResults(vector<string::size_type> &matchPositions);

  string _fileName;

  string _pattern;
  // Queues holding FileChunks for specific purposes
  utils::TSQueue<utils::FileChunk*> _availableChunkQueue = {};

  utils::TSQueue<utils::FileChunk*> _toBeDecompressedChunkQueue = {};
  utils::TSQueue<utils::FileChunk*> _toBeTransformedChunkQueue = {};
  utils::TSQueue<utils::FileChunk*> _toBeSearchedChunkQueue = {};
  utils::TSQueue<vector<string::size_type>> _partialResultsQueue = {};

  // threads:
  std::thread _readingThread;
  vector<std::thread> _decompressionThreads = {};
  vector<std::thread> _transformationThreads = {};
  vector<std::thread> _searchingThreads = {};
  std::thread _mergingThread;

  // mutex
  // mutable std::mutex _timerMutex;
  // mutable std::mutex _readingTimerMutex;
  mutable std::mutex _decompressionTimerMutex;

  mutable std::mutex _transformationTimerMutex;
  mutable std::mutex _searchingTimerMutex;
  // number of threads:
  unsigned _nDecompressionThreads = 0;

  unsigned _nTransformationThreads = 0;
  unsigned _nSearchingThreads = 0;
  unsigned _nMergingThreads = 0;

  bool _performanceMeasuring = false;
  unsigned _nFileChunks = 10;

  ulong _bufferPosition = 0;

  // performance stuff:
  ulong _totalNumberBytesRead = 0;
  double _totalTime = 0;

  double _totalReadingTime = 0;
  double _totalDecompressionTime = 0;
  double _totalTransformationTime = 0;
  double _totalSearchingTime = 0;
  double _totalMergingTime = 0;
  double _totalReadingWaitTime = 0;
  double _totalDecompressionWaitTime = 0;
  double _totalTransformationWaitTime = 0;
  double _totalSearchingWaitTime = 0;
  double _totalMergingWaitTime = 0;

 private:
  void setNumberOfQueueWriteThreads();
};

}

#endif //STRINGFINDER_SRC_STRINGFINDERBASE_H_
