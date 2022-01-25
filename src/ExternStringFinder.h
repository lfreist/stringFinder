// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

#include <cstdio>
#include <vector>
#include <thread>

#include "Timer.h"
#include "Buffer.h"
#include "ThreadSafeQueue.h"


#define MIN_BUFFER_SIZE (2 << 16)
#define BUFFER_OVERFLOW (2 << 11)
#define MAX_BUFFER_SIZE (MIN_BUFFER_SIZE + BUFFER_OVERFLOW)

/**
 * @brief ExternStringFinder class should be run using ExternStringFinderMain.
 * See printHelpAndExit() fro more informations
 * 
 */
class ExternStringFinder {
 public:
  // Constructor taking number of buffers to be initialized.
  //  Run parseCommandLineAruments afterwards to set fp, pattern etc.
  ExternStringFinder(unsigned int nBuffers = 1);
  // Constructor taking all mandatory properties.
  //  No need to run parseCommandLineArguments afterwards.
  ExternStringFinder(unsigned int nBuffers, char* file, char* pattern, bool performance, bool silent, bool count);
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
  void initializeQueues(unsigned int nBuffers);
  void readBuffers();
  std::vector<unsigned long> searchBuffers();
  static void printHelpAndExit();

  char* _pattern;
  FILE* _fp;

  TSQueue<Buffer*> _searchQueue;
  TSQueue<Buffer*> _readQueue;

  unsigned long _bufferPosition;

  unsigned long _totalNumberBytesRead;

  bool _performance;
  bool _silent;
  bool _count;
  Timer _timer;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
