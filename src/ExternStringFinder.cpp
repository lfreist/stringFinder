// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <iostream>
#include <thread>

#include "ExternStringFinder.h"
#include "Timer.h"


// ____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(unsigned int nBuffers) {
  _performance = false;
  _silent = false;
  _count = false;
  _fp = nullptr;
  _pattern = nullptr;
  _bufferPosition = 0;
  initializeQueues(nBuffers);
}

// ____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(unsigned int nBuffers, String file, String pattern, bool performance,
                                       bool silent, bool count) {
  _performance = performance;
  _silent = silent;
  _count = count;
  _fp = fopen(file.cstring(), "r");
  _pattern = pattern;
  _bufferPosition = 0;
  initializeQueues(nBuffers);
}

// ____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  fclose(_fp);
}

// ____________________________________________________________________________________________________________________
void ExternStringFinder::parseCommandLineArguments(int argc, char **argv) {
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

// ____________________________________________________________________________________________________________________
void ExternStringFinder::printHelpAndExit() {
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

// ____________________________________________________________________________________________________________________
void ExternStringFinder::initializeQueues(unsigned int nBuffers) {
  String* str = nullptr;
  for (unsigned int i = 0; i < nBuffers; i++) {
    str = new String(MAX_BUFFER_SIZE);
    _writeBufferQueue.push(str);
  }
}

// ____________________________________________________________________________________________________________________
int ExternStringFinder::writeBuffers() {
  while (true) {
    if (_writeBufferQueue.size() > 0) {

    }
  }
  return str->read(_fp);
}

// ____________________________________________________________________________________________________________________
int ExternStringFinder::find() {
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

// ____________________________________________________________________________________________________________________
void ExternStringFinder::setFile(String filepath) {

  FILE *fp = fopen(filepath.cstring(), "r");
  if (fp == nullptr) {
    printf("Could not open file '%s'\n", filepath.cstring());
    puts("Keeping current file pointer");
  } else {
    _fp = fp;
  }
}

// ____________________________________________________________________________________________________________________
std::vector<unsigned long> *ExternStringFinder::getResult() {
  return &_bytePositions;
}
