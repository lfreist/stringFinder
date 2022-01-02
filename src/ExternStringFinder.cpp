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
  _bufferPosition = 0;
  initializeQueues(nBuffers);
}

// _____________________________________________________________________________________________________________________
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

// _____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  fclose(_fp);
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::initializeQueues(unsigned int nBuffers) {
  String* str = nullptr;
  for (unsigned int i = 0; i < nBuffers; i++) {
    str = new String(MAX_BUFFER_SIZE);
    _readQueue.push(str);
  }
}

// _____________________________________________________________________________________________________________________
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
  _pattern.set(argv[optind++]);
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

// _____________________________________________________________________________________________________________________
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

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readBuffers() {
  String* currentBuffer;
  int bytesRead;
  while (true) {
    currentBuffer = _readQueue.pop();
    bytesRead = currentBuffer->readToNewLine(_fp, MIN_BUFFER_SIZE);
    if (bytesRead < 1) {
      _searchQueue.push(nullptr);
      break;
    }
    _searchQueue.push(currentBuffer);
    _totalNumberBytesRead += bytesRead;
  }
}

std::vector<unsigned long> ExternStringFinder::searchBuffers() {
  String* currentBuffer = nullptr;
  int numMatches = 0;
  std::vector<unsigned long> matchBytePositions;

  while (true) {
    currentBuffer = _searchQueue.pop();
    if (currentBuffer == nullptr) {
      break;
    }
    std::vector<unsigned int> matches = currentBuffer->findPerLineCaseSensitive(_pattern);
    _readQueue.push(currentBuffer);
    matchBytePositions.insert(matchBytePositions.end(), matches.begin(), matches.end());
  }
  if (_count) {
    std::cout << "Found " << matchBytePositions.size() << " matches" << std::endl;
  }
  return matchBytePositions;
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::find() {
  if (_performance) {
    _timer.start(true);
  }

  std::thread readBuffers(&ExternStringFinder::readBuffers, this);
  std::thread processBuffers(&ExternStringFinder::searchBuffers, this);

  readBuffers.join();
  processBuffers.join();

  /*
  while(!_readQueue.empty()) {
    String* currentBuffer = _readQueue.pop();
    int bytesRead = currentBuffer->readToNewLine(_fp, MIN_BUFFER_SIZE);
    if (bytesRead < 1) {
      _searchQueue.push(nullptr);
      break;
    }
    _searchQueue.push(currentBuffer);
    _totalNumberBytesRead += bytesRead;
  }

  while(!_searchQueue.empty()) {
    String* b = _searchQueue.pop();
    if (b == nullptr) { break; }
    std::cout << b->findPerLineCaseSensitive(_pattern).size() << std::endl;
  }
  */

  if (_performance) {
    _timer.stop();
    std::cout << "Time: " << _timer.elapsedSeconds() << " s" << std:: endl;
  }
  printf("Bytes: %ld\n", _totalNumberBytesRead);
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::setFile(String filepath) {

  FILE *fp = fopen(filepath.cstring(), "r");
  if (fp == nullptr) {
    printf("Could not open file '%s'\n", filepath.cstring());
    puts("Keeping current file pointer");
  } else {
    _fp = fp;
  }
}
