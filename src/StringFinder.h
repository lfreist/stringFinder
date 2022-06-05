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

 protected:
  void buildThreads(vector<string::size_type>* matchPositionsPtr);
  void buildThreads(vector<string::size_type>* matchPositionsPtr, const std::function<int(int)>& transformer);
  void buildThreads(vector<string::size_type>* matchPositionsPtr, const std::function<string(string)>& transformer);

  virtual void readChunks(std::istream &input, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  virtual void readChunks(std::string_view &input, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  void decompressChunks(sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  void transformChunks(const std::function<int(int)>& transformer, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  // TODO: for string_view?
  void transformChunks(const std::function<string(string)>& transformer, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  void searchChunks(string &pattern, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);
  void mergeResults(vector<string::size_type>* machPositionsPtr, sf_utils::TSQueue<FileChunk*> &popFromQueue, sf_utils::TSQueue<FileChunk*> &pushToQueue);

  // Queues holding FileChunks for specific purposes
  sf_utils::TSQueue<FileChunk*> _availableChunkQueue = {};
  sf_utils::TSQueue<FileChunk*> _toBeDecompressedChunkQueue = {};
  sf_utils::TSQueue<FileChunk*> _toBeTransformedChunkQueue = {};
  sf_utils::TSQueue<FileChunk*> _toBeSearchedChunkQueue = {};
  sf_utils::TSQueue<vector<string::size_type>> _partialResultsQueue = {};

  // threads:
  std::thread _readingThread;
  vector<std::thread> _decompressionThreads = {};
  vector<std::thread> _transformationThreads = {};
  vector<std::thread> _searchingThreads = {};
  vector<std::thread> _mergingThreads = {};

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

 private:
  void setNumberOfQueueWriteThreads();
};

}

#endif //STRINGFINDER_SRC_STRINGFINDERBASE_H_
