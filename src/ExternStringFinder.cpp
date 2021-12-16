// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <getopt.h>

#include <iostream>

#include "ExternStringFinder.hpp"
#include "Timer.hpp"

// ____________________________________________________________________________________________________________________
int nextBuffer1(FILE *fp, char *buffer) {
  char c;
  size_t s = fread(buffer, INIT_BUFFER_SIZE, 1, fp);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if ((c = fgetc(fp)) == EOF) {
      buffer[i] = '\0';
      return i;
    }
    if (c == '\n') {
      buffer[i] = c;
      buffer[i + 1] = '\0';
      return i;
    }
    buffer[i] = c;
  }
  return -1;
}

int nextBuffer2(int fd, char *buffer) {
  char c;
  ssize_t s = read(fd, buffer, INIT_BUFFER_SIZE);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if (read(fd, &c, 1) == 0) {
      buffer[i] = '\0';
      return i;
    }
    if (c == '\n') {
      buffer[i] = c;
      buffer[i+1] = '\0';
      return i;
    }
    buffer[i] = c;
  }
  return -1;
}

int findPattern(char *pattern, char *content) {
  char *tmp = content;
  char *tmp2;
  int len;
  int position;
  int counter = 0;
  unsigned long shift;
  while (true) {
    tmp = strstr(tmp, pattern);
    if (tmp == nullptr) {
      return counter;
    }
    counter++;
    tmp2 = tmp;
    tmp = strchr(tmp, '\n');
    if (tmp == nullptr) {
      return counter;
    }
    position = strlen(content) - strlen(tmp2);
    shift = 0;
    while (position >= 0 && content[position] != '\n') {
      position--;
      shift++;
    }
    len = strlen(tmp2) - strlen(tmp) + shift;
    printf("%.*s", len, content + position + 1);
  }
}

int find1(char *pattern, char *filename, bool performance, bool count) {
  FILE *fp = fopen(filename, "r");
  int s;
  int counter = 0;
  char textBuffer[MAX_BUFFER_SIZE];
  Timer timer;
  if (performance) {
    timer.start(true);
  }
  while (true) {
    s = nextBuffer1(fp, textBuffer);
    if (s < 0) {
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pattern, textBuffer);
  }
  if (count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std:: endl;
  }
  fclose(fp);
  return counter;
}

int find2(char *pattern, char *filename, bool performance, bool count) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    exit(1);
  }
  int s;
  int counter = 0;
  char textBuffer[MAX_BUFFER_SIZE];
  Timer timer;
  if (performance) {
    timer.start(true);
  }
  while (true) {
    s = nextBuffer2(fd, textBuffer);
    if (s < 0) {
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pattern, textBuffer);
  }
  if (count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std:: endl;
  }
  close(fd);
  return counter;
}

// ____________________________________________________________________________________________________________________
// ExternFinder Class

ExternFinder::ExternFinder() {
  _buffer = new char[MAX_BUFFER_SIZE];
  _performance = false;
  _silent = false;
  _count = false;
  _filename = nullptr;
  _pattern = nullptr;
  _bufferPosition = 0;
}

ExternFinder::~ExternFinder() {
  delete[] _buffer;
}

void ExternFinder::parseCommandLineArguments(int argc, char **argv) {
  struct option options[] = {
    {"performance", 0, nullptr, 'p'},
    {"silent", 0, nullptr, 's'},
    {"count", 0, nullptr, 'c'},
    {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "p:s:c", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'p': _performance = true; break;
      case 's': _silent = true; break;
      case 'c': _count = true; break;
      default: break;
    }
  }
  if (optind >= argc) {
    std::cout << "Missing input file or pattern" << std::endl;
  }
  _pattern = argv[optind++];
  _filename = argv[optind];
}

int ExternFinder::find(char *pat) {
  _bufferPosition = 0;
  _bytePositions.clear();
  int fd = open(_filename, O_RDONLY);
  if (fd < 0) {
    printf("Could not open file '%s'\n", _filename);
    exit(1);
  }
  int s;
  int counter = 0;
  if (_performance) {
    _timer.start(true);
  }
  while (true) {
    s = nextBuffer(fd);
    if (s < 0) {
      puts("Error loading new buffer\n");
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pat, _buffer);
    _bufferPosition += s;
  }
  if (_count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (_performance) {
    _timer.stop();
    std::cout << "Time: " << _timer.elapsedSeconds() << " s" << std:: endl;
  }
  close(fd);
  return counter;
}

int ExternFinder::find() {
  return find(_pattern);
}

void ExternFinder::setFilePath(char *filepath) {
  delete[] _filename;
  _filename = filepath;
}

std::vector<unsigned long> *ExternFinder::getResult() {
  return &_bytePositions;
}

int ExternFinder::nextBuffer(int fd) {
  char c;
  ssize_t s = read(fd, _buffer, INIT_BUFFER_SIZE);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if (read(fd, &c, 1) == 0) {
      _buffer[i+1] = '\0';
      return i;
    }
    if (c == '\n') {
      _buffer[i] = c;
      _buffer[i+1] = '\0';
      return i;
    }
    _buffer[i] = c;
  }
  return -1;
}

int ExternFinder::findPattern(char *pat, char *content) {
  char *tmp = content;
  char *tmp2;
  int len;
  unsigned long contentLen = strlen(content);
  int position;
  int counter = 0;
  unsigned long shift;
  while (true) {
    tmp = strstr(tmp, pat);
    if (tmp == nullptr) {
      return counter;
    }
    counter++;
    tmp2 = tmp;
    tmp = strchr(tmp, '\n');
    if (tmp == nullptr) {
      return counter;
    }
    position = contentLen - strlen(tmp2);
    _bytePositions.push_back(_bufferPosition+position);
    if (_silent) {
      continue;
    }
    shift = 0;
    while (position >= 0 && content[position] != '\n') {
      position--;
      shift++;
    }
    len = strlen(tmp2) - strlen(tmp) + shift;
    printf("%.*s", len, content + position + 1);
  }
}
