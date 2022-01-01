// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_EXTERNSTRINGFINDER_H_
#define SRC_EXTERNSTRINGFINDER_H_

#include <cstdio>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

#include "Timer.h"
#include "String.h"
#include "RotatingReader.h"


#define INIT_BUFFER_SIZE ((2 << 15)+1)
#define BUFFER_OVERFLOW (2 << 11)
#define MAX_BUFFER_SIZE (INIT_BUFFER_SIZE + BUFFER_OVERFLOW)

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
  ExternStringFinder(unsigned int nBuffers, String file, String pattern, bool performance, bool silent, bool count);
  // Destructor
  ~ExternStringFinder();

  /**
   * @brief parsing command line arguments.
   * 
   * @param argc
   * @param argv
   */
  void parseCommandLineArguments(int argc, char** argv);

  int find();

  void setFile(String filepath);

  std::vector<unsigned long>* getResult();

 private:
  void initializeQueues(unsigned int nBuffers);
  int writeBuffers();
  static void printHelpAndExit();

  String _pattern;
  FILE* _fp;

  std::queue<String*> _readBufferQueue;
  std::queue<String*> _writeBufferQueue;
  std::mutex _readMutex;
  std::mutex _writeMutex;

  unsigned long _bufferPosition;
  std::vector<unsigned long> _bytePositions;

  unsigned long _totalNumberBytesRead;

  bool _performance;
  bool _silent;
  bool _count;
  Timer _timer;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
