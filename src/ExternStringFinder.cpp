// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <istream>
#include <thread>
#include <algorithm>

#include "ExternStringFinder.h"
#include "./utils/exceptions.h"


// _____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(const string& file, const string& pattern, const string& metaFile,
                                       bool transform, bool verbose, bool performance, unsigned nBuffers,
                                       unsigned minBufferSize, unsigned bufferOverflowSize,
                                       unsigned nDecompressionThreads, unsigned nTransformThreads,
                                       unsigned nSearchThreads) {
  _searchFile = file;
  _pattern = pattern;
  _metaFile = metaFile.empty() ? nullptr : new ESFMetaFile(metaFile, std::ios::in);
  _toBeTransformed = transform;
  _verbose = verbose;
  _performance = performance;
  _nBuffers = nBuffers;
  _minBufferSize = minBufferSize;
  _nDecompressionThreads = nDecompressionThreads;
  _nTransformThreads = nTransformThreads;
  _nSearchThreads = nSearchThreads;
  _bufferPosition = 0;
  _totalNumberBytesRead = 0;
  _totalReadTime = 0;
  _totalReadWaitTime = 0;
  _totalDecompressionTime = 0;
  _totalDecompressionWaitTime = 0;
  _totalTransformTime = 0;
  _totalTransformWaitTime = 0;
  _totalSearchTime = 0;
  _totalSearchWaitTime = 0;
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
    _availableChunkQueue.push(chunk);
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readChunk(std::istream &input, TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue) {
  Timer waitTimer;
  Timer readingTimer;
  _totalNumberBytesRead = 0;
  if (_metaFile == nullptr) {
    while (true) {
      if (_performance) { waitTimer.start(); }
      FileChunk *currentChunk = popFromQueue.pop();
      if (_verbose && _performance) { waitTimer.stop(); }
      if (_performance) { readingTimer.start(false); }
      size_t bytesRead = currentChunk->setContentFromFile(input, _minBufferSize, true, _bufferPosition);
      if (_performance) { readingTimer.stop(); }
      if (bytesRead < 1) {
        pushToQueue.close();
        break;
      }
      _bufferPosition += bytesRead + 1;
      pushToQueue.push(currentChunk);
      _totalNumberBytesRead += bytesRead;
    }
  } else {
    while (true) {
      if (_performance) { waitTimer.start(false); }
      FileChunk *currentChunk = popFromQueue.pop();
      if (_performance) { waitTimer.stop(); }
      if (_performance) { readingTimer.start(); }
      auto currentChunkSize = _metaFile->nextChunkSize();
      size_t bytesRead = currentChunk->setContentFromZstdFile(
          input,
          currentChunkSize.originalSize,
          currentChunkSize.compressedSize,
          _bufferPosition
      );
      _bufferPosition += currentChunkSize.originalSize + 1;
      if (_performance) { readingTimer.stop(); }
      if (bytesRead < 1 || currentChunkSize.originalSize == 0) {
        pushToQueue.close();
        break;
      }
      pushToQueue.push(currentChunk);
      _totalNumberBytesRead += bytesRead;
    }
  }
  if (_performance) {
    std::unique_lock<std::mutex> printLock(_timerMutex);
    _totalReadTime += readingTimer.elapsedSeconds();
    _totalReadWaitTime += waitTimer.elapsedSeconds();
    if (_verbose) {
      std::cout << "Runtime (readChunk): " << readingTimer.elapsedSeconds() << "s." << std::endl;
      std::cout << "Waiting (readChunk): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

void ExternStringFinder::decompressChunk(TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue) {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_performance) { waitTimer.start(false); }
    FileChunk *currentBuffer = popFromQueue.pop(nullptr);
    if (_performance) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      pushToQueue.close();
      break;
    }
    if (_performance) { computeTimer.start(false); }
    currentBuffer->decompress();
    if (_performance) { computeTimer.stop(); }
    pushToQueue.push(currentBuffer);
  }
  if (_performance) {
    std::unique_lock timerLock(_timerMutex);
    _totalDecompressionTime += computeTimer.elapsedSeconds();
    _totalDecompressionWaitTime += waitTimer.elapsedSeconds();
    if (_verbose) {
      std::cout << "Runtime (decompressChunk): " << computeTimer.elapsedSeconds() << "s." << std::endl;
      std::cout << "Waiting (decompressChunk): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

void ExternStringFinder::transformChunk(std::function<int(int)> transformer, TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue) {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_performance) { waitTimer.start(false); }
    FileChunk *currentBuffer = popFromQueue.pop(nullptr);
    if (_performance) { waitTimer.stop(); }
    if (currentBuffer == nullptr) {
      pushToQueue.close();
      break;
    }
    if (_performance) { computeTimer.start(false); }
    currentBuffer->transform(transformer);
    if (_performance) { computeTimer.stop(); }
    pushToQueue.push(currentBuffer);
  }
  if (_performance) {
    std::unique_lock printLock(_timerMutex);
    _totalTransformTime += computeTimer.elapsedSeconds();
    _totalTransformWaitTime += waitTimer.elapsedSeconds();
    if (_verbose) {
      std::cout << "Runtime (transformChunk): " << computeTimer.elapsedSeconds() << "s." << std::endl;
      std::cout << "Waiting (transformChunk): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

void ExternStringFinder::searchBuffers(TSQueue<FileChunk*> &popFromQueue, TSQueue<FileChunk*> &pushToQueue) {
  Timer waitTimer;
  Timer computeTimer;
  while (true) {
    if (_performance) { waitTimer.start(false); }
    FileChunk *currentChunk = popFromQueue.pop(nullptr);
    if (_performance) { waitTimer.stop(); }
    if (currentChunk == nullptr) {
      _partialResultsQueue.close();
      break;
    }
    if (_performance) { computeTimer.start(false); }
    auto matches = currentChunk->searchAllPerLine(_pattern);
    if (_performance) { computeTimer.stop(); }
    pushToQueue.push(currentChunk);
    _partialResultsQueue.push(matches);
  }
  if (_performance) {
    std::unique_lock printLock(_timerMutex);
    _totalSearchTime += computeTimer.elapsedSeconds();
    _totalSearchWaitTime += waitTimer.elapsedSeconds();
    if (_verbose) {
      std::cout << "Runtime (searchBuffers): " << computeTimer.elapsedSeconds() << "s." << std::endl;
      std::cout << "Waiting (searchBuffers): " << waitTimer.elapsedSeconds() << "s." << std::endl;
    }
  }
}

// _____________________________________________________________________________________________________________________
// TODO<lfreist> building threads is weired distributed to find() and buildThreads()... do this at one place
vector<string::size_type> ExternStringFinder::find() {
  Timer timer;
  if (_performance) {
    timer.start(true);
  }

  if (_toBeTransformed) {
    transform(_pattern.begin(), _pattern.end(), _pattern.begin(), ::tolower);
  }

  buildThreads();

  if (_metaFile == nullptr) {  // no metafile provided -> no decompression
    if (_searchFile == "-") {
      if (_toBeTransformed) {
        std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(std::cin), std::ref(_availableChunkQueue), std::ref(_toBeTransformedChunkQueue));
        readBuffers.join();
      } else {
        std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(std::cin), std::ref(_availableChunkQueue), std::ref(_toBeSearchedChunkQueue));
        readBuffers.join();
      }
    } else {
      std::ifstream file(_searchFile);
      if (!file) {
        throw sf_utils::FileReadError();
      }
      if (_toBeTransformed) {
        std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(file), std::ref(_availableChunkQueue), std::ref(_toBeTransformedChunkQueue));
        readBuffers.join();
      } else {
        std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(file), std::ref(_availableChunkQueue), std::ref(_toBeSearchedChunkQueue));
        readBuffers.join();
      }
    }
    if (_toBeTransformed) {
      for (auto &transformThread : _transformThreads) {
        transformThread.join();
      }
    }
    for (auto &searchThread: _searchThreads) {
      searchThread.join();
    }
  } else {  // metafile provided -> decompression
    if (_searchFile == "-") {
      std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(std::cin), std::ref(_availableChunkQueue), std::ref(_toBeDecompressedChunkQueue));
      readBuffers.join();
    } else {
      std::ifstream file(_searchFile);
      if (!file) {
        throw sf_utils::FileReadError();
      }
      std::thread readBuffers(&ExternStringFinder::readChunk, this, std::ref(file), std::ref(_availableChunkQueue), std::ref(_toBeDecompressedChunkQueue));
      readBuffers.join();
    }
    for (auto &decompressionThread : _decompressionThreads) {
      decompressionThread.join();
    }
    for (auto &transformThread : _transformThreads) {
      transformThread.join();
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
    std::cout << "Total runtime for ESF::find(): " << timer.elapsedSeconds() << " s" << std::endl;
    std::cout << " ├─ <method>\t\t<computing time> s\t(+ <waiting for buffers> s)" << std::endl;
    std::cout << " ├─ read time:\t\t" << _totalReadTime << " s\t(+ " << _totalReadWaitTime << " s)" << std::endl;
    if (_toBeTransformed) {
      std::cout << " ├─ transform time:\t" << _totalTransformTime << " s\t(+ " << _totalTransformWaitTime << " s)" << std::endl;
    }
    if (_metaFile != nullptr) {
      std::cout << " ├─ decompression time:\t" << _totalDecompressionTime << " s\t(+ " << _totalDecompressionWaitTime << " s)" << std::endl;
    }
    std::cout << " └─ search time:\t" << _totalSearchTime << " s\t(+ " << _totalSearchWaitTime << " s)" << std::endl;
    std::cout << std::endl;
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
  if (_metaFile == nullptr) {  // no metafile provided -> do not decompress read chunks
    if (_toBeTransformed) {  // transform chunks
      _toBeTransformedChunkQueue.setNumberOfWriteThreads(1);
      _partialResultsQueue.setNumberOfWriteThreads(static_cast<int>(_nSearchThreads));
      _transformThreads.resize(_nTransformThreads);
      for (unsigned i = 0; i < _nTransformThreads; ++i) {
        _transformThreads[i] = std::thread(&ExternStringFinder::transformChunk,
                                           this,
                                           ::tolower,
                                           std::ref(_toBeTransformedChunkQueue),
                                           std::ref(_toBeSearchedChunkQueue));
      }
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nTransformThreads);
    } else {
      // only readChunks writes to _toBeSearchedQueue
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(1);
    }
  } else {  // metafile provided -> decompress read chunks
    _toBeDecompressedChunkQueue.setNumberOfWriteThreads(1);
    _decompressionThreads.resize(_nDecompressionThreads);
    if (_toBeTransformed) {
      _toBeTransformedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
      for (unsigned i = 0; i < _nDecompressionThreads; ++i) {
        _decompressionThreads[i] = std::thread(&ExternStringFinder::decompressChunk, this, std::ref(_toBeDecompressedChunkQueue), std::ref(_toBeTransformedChunkQueue));
      }
      _transformThreads.resize(_nTransformThreads);
      for (unsigned i = 0; i < _nTransformThreads; ++i) {
        _transformThreads[i] = std::thread(&ExternStringFinder::transformChunk,
                                           this,
                                           ::tolower,
                                           std::ref(_toBeTransformedChunkQueue),
                                           std::ref(_toBeSearchedChunkQueue));
      }
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nTransformThreads);
    } else {  // no transformation
      for (unsigned i = 0; i < _nDecompressionThreads; ++i) {
        _decompressionThreads[i] = std::thread(&ExternStringFinder::decompressChunk, this, std::ref(_toBeDecompressedChunkQueue), std::ref(_toBeSearchedChunkQueue));
      }
      _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
    }
  }
  _searchThreads.resize(_nSearchThreads);
  for (unsigned i = 0; i < _nSearchThreads; ++i) {
    _searchThreads[i] = std::thread(&ExternStringFinder::searchBuffers, this, std::ref(_toBeSearchedChunkQueue), std::ref(_availableChunkQueue));
  }
}

