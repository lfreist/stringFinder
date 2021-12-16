//
// Created by lfreist on 23.11.21.
//

#ifndef LIB_FILEREADER_HPP_
#define LIB_FILEREADER_HPP_

#include <stdio.h>
#include <vector>

#include "Timer.hpp"

// TODO(lfreist): optimize values:
#define INIT_BUFFER_SIZE ((2 << 15)+1)
#define BUFFER_OVERFLOW (2 << 11)
#define MAX_BUFFER_SIZE (INIT_BUFFER_SIZE + BUFFER_OVERFLOW)

int nextBuffer1(FILE *fp, char *buffer);

int nextBuffer2(int fd, char *buffer);

int find1(char *pattern, char *filename, bool performance, bool count);

int find2(char *pattern, char *filename, bool performance, bool count);

int findPattern(char *pattern, char *content);

class ExternFinder {
 public:
  ExternFinder();
  ~ExternFinder();

  void parseCommandLineArguments(int argc, char** argv);

  int find();
  int find(char *pattern);

  void setFilePath(char *filepath);

  std::vector<unsigned long>* getResult();

 private:
  int nextBuffer(int fd);
  int findPattern(char *pattern, char *text);

  char *_buffer;
  char *_filename;
  char *_pattern;

  unsigned long _bufferPosition;
  std::vector<unsigned long> _bytePositions;

  bool _performance;
  bool _silent;
  bool _count;
  Timer _timer;
};

#endif  // LIB_FILEREADER_HPP_
