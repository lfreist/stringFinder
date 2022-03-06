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


/**
 * @brief ExternStringFinder class should be run using ExternStringFinderMain.
 * See printHelpAndExit() for more information
 * 
 */
class ExternStringFinder {
 public:

  /**
   * Constructor taking number of buffers to be initialized.
   *  Run parseCommandLineArguments afterwards to set fp, pattern etc.
   * @param nBuffers number of rotating buffers
   */
  explicit ExternStringFinder(unsigned int nBuffers = 1);

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
  ExternStringFinder(unsigned int nBuffers, char* file, char* pattern, bool performance, bool silent, bool count,
                     char* metaFile = nullptr, unsigned int minBufferSize = (2 << 20),
                     unsigned int bufferOverflowSize = (2 << 11));

  // Destructor
  ~ExternStringFinder();

  /**
   * @brief parsing command line arguments.
   * 
   * @param argc
   * @param argv
   */
  void parseCommandLineArguments(int argc, char** argv);

  /**
   * @brief find _pattern in _searchFile
   */
  void find();

 private:
  static void printHelpAndExit();

  void initializeQueues();

  void buildThreads();

  void readBuffers();
  void decompressBuffers();
  void searchBuffers();

  TSQueue<FileChunk*> _readQueue;
  TSQueue<FileChunk*> _searchQueue;
  TSQueue<FileChunk*> _decompressQueue;
  TSQueue<int> _partialResultsQueue;

  std::vector<std::thread> _decompressionThreads;
  std::vector<std::thread> _searchThreads;

  mutable std::mutex _printMutex;

  unsigned _bufferPosition;
  unsigned long _totalNumberBytesRead;

  char* _pattern{};
  FILE* _searchFile;
  ESFMetaFile* _metaFile;

  unsigned _nBuffers;
  unsigned _maxBufferSize;
  unsigned _minBufferSize;

  bool _performance;
  bool _silent;
  bool _count;
  bool _debug;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
