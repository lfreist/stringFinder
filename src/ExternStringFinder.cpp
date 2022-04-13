// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <thread>

#include "ExternStringFinder.h"


// _____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(const string& file, const string& pattern,
                                       const string& metaFile, bool verbose, bool performance, unsigned nBuffers, unsigned minBufferSize,
                                       unsigned bufferOverflowSize, unsigned nDecompressionThreads,
                                       unsigned nSearchThreads) {
  _searchFile = file.empty() ? stdin : fopen(file.c_str(), "r");
  _pattern = pattern;
  _metaFile = metaFile.empty() ? nullptr : new ESFMetaFile(metaFile, std::ios::in);
  _verbose = verbose;
  _performance = performance;
  _nBuffers = nBuffers;
  _minBufferSize = minBufferSize;
  _maxBufferSize = minBufferSize + bufferOverflowSize;
  _nDecompressionThreads = nDecompressionThreads;
  _nSearchThreads = nSearchThreads;
  _bufferPosition = 0;
  _totalNumberBytesRead = 0;
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  if (_searchFile != nullptr) {
    fclose(_searchFile);
  }
  delete _metaFile;
  for (auto chunk: _fileChunks) {
    delete chunk;
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::initializeQueues() {
  for (unsigned i = 0; i <= _nBuffers; ++i) {
    auto *chunk = new FileChunk(_maxBufferSize);
    _fileChunks.push_back(chunk);
    _readQueue.push(chunk);
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readBuffers() {
  Timer waitTimer;
  _totalNumberBytesRead = 0;
  if (_metaFile == nullptr) {
    while (true) {
      if (_verbose && _performance) { waitTimer.start(false); }
      FileChunk *currentBuffer = _readQueue.pop();
      if (_verbose && _performance) { waitTimer.stop(); }
      int bytesRead = currentBuffer->setContentFromFile(_searchFile, _minBufferSize, true);
      if (bytesRead < 1) {
        _searchQueue.close();
        if (_verbose && _performance) {
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
      if (_verbose && _performance) { waitTimer.start(false); }
      FileChunk *currentBuffer = _readQueue.pop();
      if (_verbose && _performance) { waitTimer.stop(); }
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
        if (_verbose && _performance) {
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
  Timer computeTimer;
  while (true) {
    if (_verbose && _performance) { waitTimer.start(false); }
    FileChunk *currentBuffer = _decompressQueue.pop(nullptr);
    if (_verbose && _performance) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      _searchQueue.close();
      if (_verbose && _performance) {
        std::unique_lock printLock(_printMutex);
        std::cout << "Decompression was waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl
        << "Decompression was computing for " << computeTimer.elapsedSeconds() << "s" << std::endl
        << "Total: " << computeTimer.elapsedSeconds() + waitTimer.elapsedSeconds() << "s" << std::endl;
      }
      return;
    }
    if (_verbose && _performance) { computeTimer.start(false); }
    currentBuffer->decompress();
    if (_verbose && _performance) { computeTimer.stop(); }
    _searchQueue.push(currentBuffer);
  }
}

void ExternStringFinder::searchBuffers() {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_verbose && _performance) { waitTimer.start(false); }
    FileChunk *currentBuffer = _searchQueue.pop(nullptr);
    if (_verbose && _performance) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      _partialResultsQueue.close();
      break;
    }
    if (_verbose && _performance) { computeTimer.start(false); }
    auto matches = currentBuffer->findPerLine(_pattern.c_str());
    if (_verbose && _performance) { computeTimer.stop(); }
    _readQueue.push(currentBuffer);
    _partialResultsQueue.push(matches);
    // _bufferPosition += strlen(currentBuffer->cstring());
  }
  if (_verbose && _performance) {
    std::unique_lock printLock(_printMutex);
    std::cout << "Searching was waiting for " << waitTimer.elapsedSeconds() << "s" << std::endl
    << "Searching was computing for "<< computeTimer.elapsedSeconds() << "s" << std::endl
    << "Total: " << waitTimer.elapsedSeconds() + computeTimer.elapsedSeconds() << "s" << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
vector<unsigned> ExternStringFinder::find() {
  Timer timer;
  if (_performance) {
    timer.start(true);
  }

  buildThreads();

  if (_metaFile == nullptr) {
    std::thread readBuffers(&ExternStringFinder::readBuffers, this);
    readBuffers.join();

    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  } else {
    std::thread readBuffers(&ExternStringFinder::readBuffers, this);
    readBuffers.join();

    for (auto &decompressionThread: _decompressionThreads) {
      decompressionThread.join();
    }
    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  }

  unsigned count = 0;
  vector<unsigned> mergedResult;

  // TODO<lfreist>: this can run in a thread parallel to search as well...
  while (true) {
    auto elem = _partialResultsQueue.pop(vector<unsigned>(1, 0));
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
