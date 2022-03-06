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
  _debug = false;
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
  _debug = false;
  _count = count;
  _searchFile = fopen(file, "r");
  _metaFile = metaFile == nullptr ? nullptr : new ESFMetaFile(std::string(metaFile), std::ios::in);
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
  for (unsigned int i = 0; i <= _nBuffers; i++) {
    auto *str = new FileChunk(_maxBufferSize);
    _readQueue.push(str);
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::parseCommandLineArguments(int argc, char **argv) {
  struct option options[] = {
      {"help", 0, nullptr, 'h'},
      {"performance", 0, nullptr, 'p'},
      {"debug", 0, nullptr, 'd'},
      {"silent", 0, nullptr, 's'},
      {"count", 0, nullptr, 'c'},
      {"meta", 1, nullptr, 'm'},
      {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "h:p:s:c:m:d:", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'h': printHelpAndExit();
        break;
      case 'p': _performance = true;
        _silent = true;
        break;
      case 'd': _performance = true;
        _silent = true;
        _debug = true;
        break;
      case 's': _silent = true;
        break;
      case 'c': _count = true;
        break;
      case 'm': _metaFile = new ESFMetaFile(std::string(optarg), std::ios::in);
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
    _maxBufferSize = _metaFile->getMaxOriginalSize();
    _minBufferSize = _maxBufferSize;  // TODO: this is never used when a metafile is provided...
  }
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::printHelpAndExit() {
  std::cout
      << "StringFinder - ExternStringFinder - Leon Freist <freist@informatik.uni-freibur.de>" << std::endl
      << std::endl
      << "Usage: ./ExternStringFinderMain [PATTERN] [FILE] [OPTION]..." << std::endl
      << " Search for a PATTERN in a FILE." << std::endl
      << " Example: ./ExternStringFinderMain 'hello world' main.c" << std::endl
      << "If you provide a (zstd-)compressed file as input, also set a meta file using the --meta flag." << std::endl
      << std::endl
      << "  OPTIONS:" << std::endl
      << "  --help         -h  print this guide and exit." << std::endl
      << "  --performance  -p  measure wall time and print result." << std::endl
      << "  --debug        -d  performance plus waiting time of threads." << std::endl
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
  Timer waitTimer;
  _totalNumberBytesRead = 0;
  if (_metaFile == nullptr) {
    while (true) {
      if (_debug) { waitTimer.start(false); }
      FileChunk *currentBuffer = _readQueue.pop();
      if (_debug) { waitTimer.stop(); }
      int bytesRead = currentBuffer->setContentFromFile(_searchFile, _minBufferSize, true);
      if (bytesRead < 1) {
        _decompressQueue.close();
        if (_debug) {
          std::unique_lock<std::mutex> printLock(_printMutex);
          std::cout << "ReadBuffer waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl;
        }
        return;
      }
      _searchQueue.push(currentBuffer);
      _totalNumberBytesRead += bytesRead;
    }
  } else {
    while (true) {
      if (_debug) { waitTimer.start(false); }
      FileChunk *currentBuffer = _readQueue.pop();
      if (_debug) { waitTimer.stop(); }
      auto currentChunkSize = _metaFile->nextChunkSize();
      int bytesRead = currentBuffer->setContentFromFile(
          _searchFile,
          currentChunkSize.compressedSize,
          false,
          true,
          currentChunkSize.originalSize
      );
      if (bytesRead < 1 || currentChunkSize.originalSize == 0) {
        _decompressQueue.close();
        if (_debug) {
          std::unique_lock<std::mutex> printLock(_printMutex);
          std::cout << "Reading was waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl;
        }
        return;
      }
      _decompressQueue.push(currentBuffer);
      _totalNumberBytesRead += bytesRead;
    }
  }
}

void ExternStringFinder::decompressBuffers() {
  Timer waitTimer;
  while (true) {
    if (_debug) { waitTimer.start(false); }
    FileChunk *currentBuffer = _decompressQueue.pop(nullptr);
    if (_debug) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      _searchQueue.close();
      if (_debug) {
        std::unique_lock<std::mutex> printLock(_printMutex);
        std::cout << "Decompression was waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl;
      }
      return;
    }
    currentBuffer->decompress();
    _searchQueue.push(currentBuffer);
  }
}

void ExternStringFinder::searchBuffers() {
  Timer waitTimer;
  while (true) {
    if (_debug) { waitTimer.start(false); }
    FileChunk *currentBuffer = _searchQueue.pop(nullptr);
    if (_debug) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      _partialResultsQueue.close();
      break;
    }
    auto matches = currentBuffer->findPerLine(_pattern);
    _readQueue.push(currentBuffer);
    _partialResultsQueue.push(matches.size());
    // _bufferPosition += strlen(currentBuffer->cstring());
  }
  if (_debug) {
    std::unique_lock<std::mutex> printLock(_printMutex);
    std::cout << "Searching was waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::find() {
  Timer timer;
  if (_performance) {
    timer.start(true);
  }

  if (_metaFile == nullptr) {
    std::thread readBuffers(&ExternStringFinder::readBuffers, this);
    std::thread processBuffers(&ExternStringFinder::searchBuffers, this);

    readBuffers.join();
    processBuffers.join();
  } else {
    buildThreads();

    std::thread readBuffers(&ExternStringFinder::readBuffers, this);

    readBuffers.join();

    for (auto &decompressionThread: _decompressionThreads) {
      decompressionThread.join();
    }
    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  }

  unsigned res = 0;

  while (true) {
    auto elem = _partialResultsQueue.pop(-1);
    if (elem == -1) {
      break;
    }
    res += elem;
  }

  if (_performance || _debug) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std::endl;
  }
  if (_debug) {
    std::cout << "Total number of bytes read: " << _totalNumberBytesRead << std::endl;
  }
  if (_count) {
    std::cout << "Found " << res << " matches" << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::buildThreads() {
  _decompressQueue.setNumberOfWriteThreads(1);
  if (_readQueue.size() > 1) {
    int numDecompressionThreads = 4;
    int numSearchThreads = 3;
    for (int i = 0; i < numDecompressionThreads; i++) {
      _decompressionThreads.emplace_back(&ExternStringFinder::decompressBuffers, this);
    }
    _searchQueue.setNumberOfWriteThreads(numDecompressionThreads);
    for (int i = 0; i < numSearchThreads; i++) {
      _searchThreads.emplace_back(&ExternStringFinder::searchBuffers, this);
    }
    _partialResultsQueue.setNumberOfWriteThreads(numSearchThreads);
  } else {
    _decompressionThreads.emplace_back(&ExternStringFinder::decompressBuffers, this);
    _searchThreads.emplace_back(&ExternStringFinder::searchBuffers, this);
    _searchQueue.setNumberOfWriteThreads(1);
    _partialResultsQueue.setNumberOfWriteThreads(1);
  }
}
