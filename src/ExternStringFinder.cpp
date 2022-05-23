// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <fstream>
#include <istream>
#include <thread>

#include "ExternStringFinder.h"
#include "./utils/exceptions.h"


// _____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(const string& file, const string& pattern,
                                       const string& metaFile, bool verbose, bool performance, unsigned nBuffers, unsigned minBufferSize,
                                       unsigned bufferOverflowSize, unsigned nDecompressionThreads,
                                       unsigned nSearchThreads) {
  _searchFile = file;
  _pattern = pattern;
  _metaFile = metaFile.empty() ? nullptr : new ESFMetaFile(metaFile, std::ios::in);
  _verbose = verbose;
  _performance = performance;
  _nBuffers = nBuffers;
  _minBufferSize = minBufferSize;
  _nDecompressionThreads = nDecompressionThreads;
  _nSearchThreads = nSearchThreads;
  _bufferPosition = 0;
  _totalNumberBytesRead = 0;
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  delete _metaFile;
  for (auto chunk: _fileChunks) {
    delete chunk;
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::initializeQueues() {
  for (unsigned i = 0; i <= _nBuffers; ++i) {
    auto *chunk = new FileChunk();
    _fileChunks.push_back(chunk);
    _readQueue.push(chunk);
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readBuffers(std::istream &input) {
  Timer waitTimer;
  Timer readingTimer;
  _totalNumberBytesRead = 0;
  if (_performance) {
    readingTimer.start(false);
  }
  if (_metaFile == nullptr) {
    while (true) {
      if (_verbose && _performance) { waitTimer.start(); }
      FileChunk *currentChunk = _readQueue.pop();
      if (_verbose && _performance) { waitTimer.stop(); }
      if (_performance) { readingTimer.start(false); }
      size_t bytesRead = currentChunk->setContentFromFile(input, _minBufferSize, true, _bufferPosition);
      if (_performance) { readingTimer.stop(); }
      if (bytesRead < 1) {
        _searchQueue.close();
        break;
      }
      _bufferPosition += bytesRead + 1;
      _searchQueue.push(currentChunk);
      _totalNumberBytesRead += bytesRead;
    }
  } else {
    while (true) {
      if (_verbose && _performance) { waitTimer.start(false); }
      FileChunk *currentChunk = _readQueue.pop();
      if (_verbose && _performance) { waitTimer.stop(); }
      if (_verbose && _performance) { readingTimer.start(); }
      auto currentChunkSize = _metaFile->nextChunkSize();
      size_t bytesRead = currentChunk->setContentFromZstdFile(
          input,
          currentChunkSize.originalSize,
          currentChunkSize.compressedSize,
          _bufferPosition
      );
      _bufferPosition += currentChunkSize.originalSize + 1;
      if (_verbose && _performance) { readingTimer.stop(); }
      if (bytesRead < 1 || currentChunkSize.originalSize == 0) {
        _decompressQueue.close();
        break;
      }
      _decompressQueue.push(currentChunk);
      _totalNumberBytesRead += bytesRead;
    }
  }
  if (_performance) {
    readingTimer.stop();
    std::unique_lock<std::mutex> printLock(_printMutex);
    std::cout << "Runtime (readBuffers): " << readingTimer.elapsedSeconds() << "s." << std::endl;
    if (_verbose) {
      std::cout << "Waiting (readBuffers): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

void ExternStringFinder::decompressBuffers() {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_verbose && _performance) { waitTimer.start(false); }
    FileChunk *currentBuffer = _decompressQueue.pop(nullptr);
    if (_verbose && _performance) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      _searchQueue.close();
      break;
    }
    if (_performance) { computeTimer.start(false); }
    currentBuffer->decompress();
    if (_performance) { computeTimer.stop(); }
    _searchQueue.push(currentBuffer);
  }
  if (_performance) {
    std::unique_lock printLock(_printMutex);
    std::cout << "Runtime (decompressBuffers): " << computeTimer.elapsedSeconds() << "s." << std::endl;
    if (_verbose) {
      std::cout << "Waiting (decompressBuffers): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

void ExternStringFinder::searchBuffers() {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_verbose && _performance) { waitTimer.start(false); }
    FileChunk *currentChunk = _searchQueue.pop(nullptr);
    if (_verbose && _performance) { waitTimer.stop(); }
    if (currentChunk == nullptr) {
      _partialResultsQueue.close();
      break;
    }
    if (_verbose && _performance) { computeTimer.start(false); }
    auto matches = currentChunk->searchAllPerLine(_pattern);
    if (_verbose && _performance) { computeTimer.stop(); }
    _readQueue.push(currentChunk);
    _partialResultsQueue.push(matches);
  }
  if (_performance) {
    std::unique_lock printLock(_printMutex);
    std::cout << "Runtime (searchBuffers): " << computeTimer.elapsedSeconds() << "s." << std::endl;
    if (_verbose) {
      std::cout << "Waiting (searchBuffers): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

// _____________________________________________________________________________________________________________________
vector<string::size_type> ExternStringFinder::find() {
  Timer timer;
  if (_performance) {
    timer.start(true);
  }

  buildThreads();

  if (_metaFile == nullptr) {
    if (_searchFile == "-") {
      std::thread readBuffers(&ExternStringFinder::readBuffers, this, std::ref(std::cin));
      readBuffers.join();
    } else {
      std::ifstream file(_searchFile);
      if (!file) {
        throw sf_utils::FileReadError();
      }
      std::thread readBuffers(&ExternStringFinder::readBuffers, this, std::ref(file));
      readBuffers.join();
    }

    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  } else {
    if (_searchFile == "-") {
      std::thread readBuffers(&ExternStringFinder::readBuffers, this, std::ref(std::cin));
      readBuffers.join();
    } else {
      std::ifstream file(_searchFile);
      std::thread readBuffers(&ExternStringFinder::readBuffers, this, std::ref(file));
      readBuffers.join();
    }

    for (auto &decompressionThread: _decompressionThreads) {
      decompressionThread.join();
    }
    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  }

  unsigned count = 0;
  vector<string::size_type> mergedResult;

  // TODO<lfreist>: this can run in a thread parallel to search as well...
  while (true) {
    vector<string::size_type> elem = _partialResultsQueue.pop(vector<string::size_type>(1, 0));
    if (elem.size() == 1) {
      if (elem[0] == 0) break;
    }
    count += elem.size();
    std::copy(elem.begin(), elem.end(), std::back_inserter(mergedResult));
  }

  if (_performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std::endl;
  }
  if (_verbose) {
    std::cout << "Total number of bytes read: " << _totalNumberBytesRead << std::endl;
  }
  if (_verbose) {
    std::cout << "Found " << count << " matches" << std::endl;
  }
  // TODO: should be sorted in place while merging vectors in line ~200
  Timer sortTimer;
  sortTimer.start();
  std::sort(mergedResult.begin(), mergedResult.end());
  sortTimer.stop();
  if (_performance) {
    std::cout << "Sorting result took " << sortTimer.elapsedSeconds() << " seconds." << std::endl;
  }
  return mergedResult;
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::buildThreads() {
  if (_metaFile == nullptr) {
    _searchQueue.setNumberOfWriteThreads(1);
    _partialResultsQueue.setNumberOfWriteThreads(static_cast<int>(_nSearchThreads));
    for (unsigned i = 0; i < _nSearchThreads; ++i) {
      _searchThreads.emplace_back(&ExternStringFinder::searchBuffers, this);
    }
  } else {
    _decompressQueue.setNumberOfWriteThreads(1);
    for (unsigned i = 0; i < _nDecompressionThreads; ++i) {
      _decompressionThreads.emplace_back(&ExternStringFinder::decompressBuffers, this);
    }
    _searchQueue.setNumberOfWriteThreads(static_cast<int>(_nDecompressionThreads));
    for (unsigned i = 0; i < _nSearchThreads; ++i) {
      _searchThreads.emplace_back(&ExternStringFinder::searchBuffers, this);
    }
    _partialResultsQueue.setNumberOfWriteThreads(static_cast<int>(_nSearchThreads));
  }
}
