//
// Created by lfreist on 23.11.21.
//

#ifndef LIB_FILEREADER_HPP_
#define LIB_FILEREADER_HPP_

#include <cstdio>
#include <vector>

#include "Timer.hpp"

// TODO(lfreist): optimize values:
#define INIT_BUFFER_SIZE ((2 << 15)+1)
#define BUFFER_OVERFLOW (2 << 11)
#define MAX_BUFFER_SIZE (INIT_BUFFER_SIZE + BUFFER_OVERFLOW)

class ExternFinder {
 public:
  ExternFinder();
  ExternFinder(char* file, char* pattern, bool performance, bool silent, bool count);
  ~ExternFinder();

  void parseCommandLineArguments(int argc, char** argv);

  int find();
  int find(char *pattern);

  void setFile(char *filepath);

  std::vector<unsigned long>* getResult();

 private:
  // int nextBuffer();
  int nextBuffer();
  int findPattern(char *pattern, char *text);
  static void printHelpAndExit();

  char *_buffer;
  // int _fd;
  FILE *_fp;
  char *_pattern;

  unsigned long _bufferPosition;
  std::vector<unsigned long> _bytePositions;

  unsigned long _nbytes;

  bool _performance;
  bool _silent;
  bool _count;
  Timer _timer;
};

#endif  // LIB_FILEREADER_HPP_
