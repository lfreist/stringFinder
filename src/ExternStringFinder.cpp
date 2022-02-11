// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <iostream>
#include <thread>

#include "ExternStringFinder.h"
#include "./utils/ESFMetaFile.h"

// ____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(unsigned int nBuffers) {
  _performance = false;
  _silent = false;
  _count = false;
  _searchFile = nullptr;
  _metaFile = nullptr;
  _bufferPosition = 0;
  _nBuffers = nBuffers;
  _minBufferSize = (2 << 20);
  _maxBufferSize = _minBufferSize + (2 << 11);
}

// _____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(unsigned int nBuffers, char *file, char *pattern, bool performance, bool silent,
                                       bool count, char *metaFile, unsigned int minBufferSize,
                                       unsigned int bufferOverflowSize) {
  _performance = performance;
  _silent = silent;
  _count = count;
  _searchFile = fopen(file, "r");
  _metaFile = metaFile == nullptr ? nullptr : new ESFMetaFile(std::string(metaFile));
  _pattern = pattern;
  _bufferPosition = 0;
  _totalNumberBytesRead = 0;
  _nBuffers = nBuffers;
  _minBufferSize = minBufferSize;
  _maxBufferSize = minBufferSize + bufferOverflowSize;
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  if (_searchFile != nullptr) {
    fclose(_searchFile);
  }
  delete _metaFile;
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::initializeQueues() {
  for (unsigned int i = 0; i < _nBuffers; i++) {
    auto *str = new Buffer(_maxBufferSize);
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
      {"meta", 1, nullptr, 'm'},
      {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "h:p:s:c:m:", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'h': printHelpAndExit();
        break;
      case 'p':_performance = true;
        _silent = true;
        break;
      case 's': _silent = true;
        break;
      case 'c': _count = true;
        break;
      case 'm': _metaFile = new ESFMetaFile(std::string(optarg));
      default: break;
    }
  }
  if (optind >= argc) {
    std::cout << "Missing input file or pattern" << std::endl;
    printHelpAndExit();
  }
  _pattern = argv[optind++];
  if (optind >= argc) {
    _searchFile = stdin;
  } else if (strcmp(argv[optind], "-") == 0) {
    _searchFile = stdin;
  } else {
    _searchFile = fopen(argv[optind], "r");
  }
  if (_searchFile == nullptr) {
    printf("Could not open file '%s'\n", argv[optind]);
    exit(1);
  }
  if (_metaFile != nullptr) {
    _maxBufferSize = _metaFile->getMaxChunkSize();
    _minBufferSize = _maxBufferSize;  // TODO: this is never used when a metafile is provided...
  }
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::printHelpAndExit() {
  std::cout
      << "StringFinder - ExternStringFinder - Leon Freist <freist@informatik.uni-freibur.de>" << std::endl
      << "Usage: ./ExternStringFinderMain [PATTERN] [FILE] [OPTION]..." << std::endl
      << " Search for a PATTERN in a FILE." << std::endl
      << " Example: ./ExternStringFinderMain 'hello world' main.c" << std::endl
      << "If you provide a (zstd-)compressed file as input, also set a meta file using the --meta flag." << std::endl
      << std::endl
      << "  OPTIONS:" << std::endl
      << "  --help         -h  print this guide and exit." << std::endl
      << "  --performance  -p  measure wall time on find and print result." << std::endl
      << "  --silent       -s  dont print matching lines." << std::endl
      << "  --count        -c  print number of matching lines." << std::endl
      << "  --meta [FILE]  -m  set meta file." << std::endl
      << std::endl
      << "When FILE is not provided read standard input" << std::endl
      << " Example: cat main.c | ./ExternStringFinderMain 'hello world'" << std::endl;
  exit(0);
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readBuffers() {
  if (_metaFile == nullptr) {
    while (true) {
      Buffer *currentBuffer = _readQueue.pop();
      int bytesRead = currentBuffer->setContentFromFile(_searchFile, _minBufferSize, true);
      if (bytesRead < 1) {
        _searchQueue.push(nullptr);
        return;
      }
      _searchQueue.push(currentBuffer);
      _totalNumberBytesRead += bytesRead;
    }
  } else {
    while (true) {
      Buffer *currentBuffer = _readQueue.pop();
      auto currentPair = _metaFile->nextChunkPair();
      int bytesRead = currentBuffer->setContentFromFile(
          _searchFile,
          currentPair.second,
          false,
          true,
          currentPair.first
      );
      if (bytesRead < 1) {
        _decompressQueue.push(nullptr);
        return;
      }
      _decompressQueue.push(currentBuffer);
      _totalNumberBytesRead += bytesRead;
    }
  }
}

void ExternStringFinder::decompressBuffers() {
  while (true) {
    Buffer *currentBuffer = _decompressQueue.pop();
    if (currentBuffer == nullptr) {
      _searchQueue.push(nullptr);
      return;
    }
    currentBuffer->decompress();
    _searchQueue.push(currentBuffer);
  }
}

std::vector<unsigned long> ExternStringFinder::searchBuffers() {
  std::vector<unsigned long> matchBytePositions;
  while (true) {
    Buffer *currentBuffer = _searchQueue.pop();
    if (currentBuffer == nullptr) {
      break;
    }
    std::vector<unsigned int> matches = currentBuffer->findPerLine(_pattern);
    _readQueue.push(currentBuffer);
    // TODO: make multithreadable
    matchBytePositions.insert(matchBytePositions.end(), matches.begin(), matches.end());
    _bufferPosition += strlen(currentBuffer->cstring());
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

  if (_metaFile == nullptr) {
    std::thread readBuffers(&ExternStringFinder::readBuffers, this);
    std::thread processBuffers(&ExternStringFinder::searchBuffers, this);

    readBuffers.join();
    processBuffers.join();
  } else {
    std::thread readBuffers(&ExternStringFinder::readBuffers, this);
    std::thread decompressBuffers(&ExternStringFinder::decompressBuffers, this);
    std::thread processBuffers(&ExternStringFinder::searchBuffers, this);

    readBuffers.join();
    decompressBuffers.join();
    processBuffers.join();
  }

  if (_performance) {
    _timer.stop();
    std::cout << "Time: " << _timer.elapsedSeconds() << " s" << std::endl;
  }
  printf("Bytes: %ld\n", _totalNumberBytesRead);
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::setFile(char *filepath) {

  FILE *fp = fopen(filepath, "r");
  if (fp == nullptr) {
    printf("Could not open file '%s'\n", filepath);
    puts("Keeping current file pointer");
  } else {
    _searchFile = fp;
  }
}
