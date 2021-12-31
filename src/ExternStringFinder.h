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

// TODO(lfreist): optimize values:
#define INIT_BUFFER_SIZE ((2 << 15)+1)
#define BUFFER_OVERFLOW (2 << 11)
#define MAX_BUFFER_SIZE (INIT_BUFFER_SIZE + BUFFER_OVERFLOW)

class ExternFinder {
 public:
  ExternFinder(unsigned int nBuffers = 1);
  ExternFinder(unsigned int nBuffers, String file, String pattern, bool performance, bool silent, bool count);
  ~ExternFinder();

  void parseCommandLineArguments(int argc, char** argv);

  // int find();
  // int find(String pattern);
  int find();

  void setFile(String filepath);

  std::vector<unsigned long>* getResult();

 private:
  // int nextBuffer();
  // int findPattern(String pattern, String text);
  int nextBuffer();
  static void printHelpAndExit();

  RotatingReader* _rotReader;

  char* _buffer;
  String _pattern;
  FILE* _fp;

  std::queue<String> _bufferQueue;
  std::mutex _queueMutex;

  unsigned long _bufferPosition;
  std::vector<unsigned long> _bytePositions;

  unsigned long _totalNumberBytesRead;

  bool _performance;
  bool _silent;
  bool _count;
  Timer _timer;
};

#endif  // SRC_EXTERNSTRINGFINDER_H_
