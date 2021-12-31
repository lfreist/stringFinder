// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <thread>

#include "ExternStringFinder.h"
#include "Timer.h"


ExternFinder::ExternFinder(unsigned int nBuffers) {
  _buffer = new char[MAX_BUFFER_SIZE];
  _performance = false;
  _silent = false;
  _count = false;
  _fp = nullptr;
  _pattern = nullptr;
  _bufferPosition = 0;
  _rotReader = new RotatingReader(nBuffers);
}

ExternFinder::ExternFinder(unsigned int nBuffers, String file, String pattern, bool performance, bool silent, bool count) {
  _buffer = new char[MAX_BUFFER_SIZE];
  _performance = performance;
  _silent = silent;
  _count = count;
  _fp = fopen(file.cstring(), "r");
  _pattern = pattern;
  _bufferPosition = 0;
  _rotReader = new RotatingReader(nBuffers);
}

ExternFinder::~ExternFinder() {
  fclose(_fp);
  delete[] _buffer;
}

void ExternFinder::parseCommandLineArguments(int argc, char **argv) {
  struct option options[] = {
    {"help", 0, nullptr, 'h'},
    {"performance", 0, nullptr, 'p'},
    {"silent", 0, nullptr, 's'},
    {"count", 0, nullptr, 'c'},
    {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "h:p:s:c", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'h': printHelpAndExit(); break;
      case 'p':
        _performance = true;
        _silent = true;
        break;
      case 's': _silent = true; break;
      case 'c': _count = true; break;
      default: break;
    }
  }
  if (optind >= argc) {
    std::cout << "Missing input file or pattern" << std::endl;
    printHelpAndExit();
  }
  _pattern = argv[optind++];
  if (optind >= argc) {
    _fp = stdin;
  } else {
    _fp = fopen(argv[optind], "r");
  }
  if (_fp == nullptr) {
    printf("Could not open file '%s'\n", argv[optind]);
    exit(1);
  }
}

void ExternFinder::printHelpAndExit() {
  std::cout
    << "StringFinder - ExternStringFinder - Leon Freist <freist@informatik.uni-freibur.de>" << std::endl
    << "Usage: ./ExternStringFinderMain [PATTERN] [FILE] [OPTION]..." << std::endl
    << " Search for a PATTERN in a FILE." << std::endl
    << " Example: ./ExternStringFinderMain 'hello world' main.c"
    << std::endl
    << "  OPTIONS:" << std::endl
    << "  --help         -h  print this guide and exit." << std::endl
    << "  --performance  -p  measure wall time on find and print result." << std::endl
    << "  --silent       -s  dont print matching lines." << std::endl
    << "  --count        -c  print number of matching lines." << std::endl
    << std::endl
    << "When FILE is not provided read standard input" << std::endl
    << " Example: cat main.c | ./ExternStringFinderMain 'hello world'" << std::endl;
  exit(0);
}

int ExternFinder::nextBuffer() {
  Buffer* writeBuffer = _rotReader->getNextWriteBuffer();
  String* content;
  char additional_char;
  size_t bytes_read = fread(_buffer, sizeof(char), INIT_BUFFER_SIZE, _fp);
  if (bytes_read == 0) { return 0; }
  for (int i = (int) bytes_read; i < MAX_BUFFER_SIZE; i++) {
    if ((additional_char = (char) fgetc(_fp)) == EOF) {
      _buffer[i] = '\0';
      content = new String(_buffer);
      writeBuffer->write(content);
      return i;
    }
    if (additional_char == '\n') {
      _buffer[i] = additional_char;
      _buffer[i+1] = '\0';
      content = new String(_buffer);
      writeBuffer->write(content);
      return i;
    }
    _buffer[i] = additional_char;
  }
  return -1;
}

int ExternFinder::find() {
  _bufferPosition = 0;
  _bytePositions.clear();
  int bytes_read;
  int match_counter = 0;
  Buffer* readBuffer;

  if (_performance) {
    _timer.start(true);
  }
  while (true) {
    bytes_read = nextBuffer();
    _totalNumberBytesRead += bytes_read;
    if (bytes_read < 0) {
      puts("Error loading new buffer\n");
      exit(1);
    } else if (bytes_read == 0) {
      break;
    }
    readBuffer = _rotReader->getNextReadBuffer();
    match_counter += readBuffer->read()->findPerLineCaseSensitive(_pattern).size();
    _bufferPosition += bytes_read;
  }
  if (_count) {
    std::cout << "Found " << match_counter << " Matches" << std::endl;
  }
  if (_performance) {
    _timer.stop();
    std::cout << "Time: " << _timer.elapsedSeconds() << " s" << std:: endl;
  }
  printf("Bytes: %ld\n", _totalNumberBytesRead);
  return match_counter;
}

/*
 * in while-loop: check for item in _bufferQueue (mutex), pop it and perform search
*/
/*
int ExternFinder::find(String pattern) {
  _bufferPosition = 0;
  _bytePositions.clear();
  int bytes_read;
  int match_counter = 0;
  if (_performance) {
    _timer.start(true);
  }
  while (true) {
    bytes_read = nextBuffer();
    _totalNumberBytesRead += bytes_read;
    if (bytes_read < 0) {
      puts("Error loading new buffer\n");
      exit(1);
    } else if (bytes_read == 0) {
      break;
    }
    match_counter += findPattern(pattern, _buffer);
    _bufferPosition += bytes_read;
  }
  if (_count) {
    std::cout << "Found " << match_counter << " Matches" << std::endl;
  }
  if (_performance) {
    _timer.stop();
    std::cout << "Time: " << _timer.elapsedSeconds() << " s" << std:: endl;
  }
  printf("Bytes: %ld\n", _totalNumberBytesRead);
  return match_counter;
}
*/

/*
int ExternFinder::find() {
  return find(_pattern);
}
*/

void ExternFinder::setFile(String filepath) {

  FILE *fp = fopen(filepath.cstring(), "r");
  if (fp == nullptr) {
    printf("Could not open file '%s'\n", filepath.cstring());
    puts("Keeping current file pointer");
  } else {
    _fp = fp;
  }
}

std::vector<unsigned long> *ExternFinder::getResult() {
  return &_bytePositions;
}

/*
 * create a new char* [MAX_BUFFER_SIZE], fill it and push to _bufferQueue (mutex)
*/
/*
int ExternFinder::nextBuffer() {
  char additional_char;
  size_t bytes_read = fread(_buffer, sizeof(char), INIT_BUFFER_SIZE, _fp);
  if (bytes_read == 0) { return 0; }
  for (int i = (int) bytes_read; i < MAX_BUFFER_SIZE; i++) {
    if ((additional_char = (char) fgetc(_fp)) == EOF) {
      _buffer[i] = '\0';
      return i;
    }
    if (additional_char == '\n') {
      _buffer[i] = additional_char;
      _buffer[i+1] = '\0';
      return i;
    }
    _buffer[i] = additional_char;
  }
  return -1;
}
*/

/*
int ExternFinder::findPattern(String pattern, String content) {
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
*/