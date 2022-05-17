// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

#include <cstdio>
#include <vector>
#include <thread>
#include <string>
#include <mutex>

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
  ExternStringFinder(const string& file, const string& pattern, const string& metaFile = "",
                     bool verbose = false, bool performance = false, unsigned nBuffers = 10, unsigned minBufferSize = (1 << 22),
                     unsigned bufferOverflowSize = (1 << 15), unsigned nDecompressionThreads = 1,
                     unsigned nSearchThreads = 1);

  ~ExternStringFinder();

  /**
   * @brief find _pattern in _searchFile
   */
  vector<unsigned> find();

 private:

  void initializeQueues();

  void buildThreads();

  void readBuffers(std::istream &input);
  void decompressBuffers();
  void searchBuffers();

  TSQueue<FileChunk*> _readQueue;
  TSQueue<FileChunk*> _searchQueue;
  TSQueue<FileChunk*> _decompressQueue;
  TSQueue<vector<string::size_type>> _partialResultsQueue;

  std::vector<std::thread> _decompressionThreads;
  std::vector<std::thread> _searchThreads;

  std::vector<FileChunk*> _fileChunks;

  mutable std::mutex _printMutex;

  unsigned _bufferPosition;
  unsigned long _totalNumberBytesRead;

  string _pattern;
  string _searchFile;
  ESFMetaFile* _metaFile;

  unsigned _nBuffers;
  unsigned _maxBufferSize;
  unsigned _minBufferSize;

  unsigned _nDecompressionThreads;
  unsigned _nSearchThreads;

  bool _verbose;
  bool _performance;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
