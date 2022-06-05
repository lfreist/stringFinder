// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

#include <cstdio>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <functional>

#include "./utils/Timer.h"
#include "./utils/FileChunk.h"
#include "./utils/ThreadSafeQueue.h"
#include "./utils/ESFMetaFile.h"

using std::string;
using std::vector;


class ExternStringFinder {
 public:

  /**
   * Constructor taking all mandatory properties.
   *  No need to run parseCommandLineArguments afterwards.
   * @param nBuffers
   * @param file
   * @param pattern
   * @param performance
   * @param silent
   * @param count
   * @param metaFile
   * @param minBufferSize
   * @param bufferOverflowSize
   */
  ExternStringFinder(const string& file, const string& pattern, const string& metaFile = "", bool transform = false,
                     bool verbose = false, bool performance = false, unsigned nBuffers = 10, unsigned minBufferSize = (1 << 22),
                     unsigned bufferOverflowSize = (1 << 15), unsigned nDecompressionThreads = 1,
                     unsigned nTransformThreads = 1, unsigned nSearchThreads = 1);

  ~ExternStringFinder();

  /**
   * @brief find _pattern in _searchFile
   */
  vector<string::size_type> find();

  void setNumberOfDecompressionThreads(unsigned nDecompressionThreads);
  void setNumberOfTransformationThreads(unsigned nTransformationThreads);
  void setNumberOfSearchThreads(unsigned nSearchThreads);

  string toString() const;

 private:

  void initializeQueues();

  void buildThreads();

  void readChunk(std::istream &input, TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue);
  void decompressChunk(TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue);
  void transformChunk(std::function<int(int)> transformer, TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue);
  void searchBuffers(TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue);

  TSQueue<FileChunk*> _availableChunkQueue;
  TSQueue<FileChunk*> _toBeDecompressedChunkQueue;
  TSQueue<FileChunk*> _toBeTransformedChunkQueue;
  TSQueue<FileChunk*> _toBeSearchedChunkQueue;
  TSQueue<vector<string::size_type>> _partialResultsQueue;

  std::thread _readThread;
  std::vector<std::thread> _decompressionThreads;
  std::vector<std::thread> _transformThreads;
  std::vector<std::thread> _searchThreads;

  std::vector<FileChunk*> _fileChunks;

  unsigned _bufferPosition;

  unsigned long _totalNumberBytesRead;
  string _pattern;

  string _searchFile;
  std::ifstream _file;
  ESFMetaFile* _metaFile;
  bool _toBeTransformed;
  unsigned _nBuffers;

  unsigned _minBufferSize;
  unsigned _nDecompressionThreads;

  unsigned _nTransformThreads;
  unsigned _nSearchThreads;
  bool _verbose;

  bool _performance;

  // for debugging (performance/verbose):
  mutable std::mutex _timerMutex;

  double _totalTime = 0;

  double _totalReadTime = 0;
  double _totalReadWaitTime = 0;
  double _totalDecompressionTime = 0;
  double _totalDecompressionWaitTime = 0;
  double _totalTransformTime = 0;
  double _totalTransformWaitTime = 0;
  double _totalSearchTime = 0;
  double _totalSearchWaitTime = 0;
};

std::ostream &operator<<(std::ostream &os, const ExternStringFinder &esf);

#endif  // SRC_EXTERNSTRINGFINDER_H_
