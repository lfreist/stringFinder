// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

#include <cstdio>
#include <vector>
#include <thread>
#include <string>

#include "./utils/Timer.h"
#include "./utils/Buffer.h"
#include "./utils/ThreadSafeQueue.h"
#include "./utils/ESFMetaFile.h"


/**
 * @brief ExternStringFinder class should be run using ExternStringFinderMain.
 * See printHelpAndExit() for more information
 * 
 */
class ExternStringFinder {
 public:
  // Constructor taking number of buffers to be initialized.
  //  Run parseCommandLineArguments afterwards to set fp, pattern etc.
  explicit ExternStringFinder(unsigned int nBuffers = 1);
  // Constructor taking all mandatory properties.
  //  No need to run parseCommandLineArguments afterwards.
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

  void find();

  void setFile(char* filepath);

 private:
  void initializeQueues();
  void readBuffers();
  void decompressBuffers();
  std::vector<unsigned long> searchBuffers();
  static void printHelpAndExit();

  char* _pattern{};
  FILE* _searchFile;
  ESFMetaFile* _metaFile;

  unsigned int _nBuffers;
  unsigned int _maxBufferSize;
  unsigned int _minBufferSize;
  TSQueue<Buffer*> _searchQueue;
  TSQueue<Buffer*> _readQueue;
  TSQueue<Buffer*> _decompressQueue;

  unsigned long _bufferPosition;

  unsigned long _totalNumberBytesRead{};

  bool _performance;
  bool _silent;
  bool _count;
  bool _debug;
  Timer _timer;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
