// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <istream>
#include <thread>
#include <algorithm>

#include "ExternStringFinder.h"
#include "./utils/exceptions.h"
#include "./utils/output/tree.h"
#include "./utils/math.h"

using namespace sf_utils;

// _____________________________________________________________________________________________________________________
ExternStringFinder::ExternStringFinder(const string &file, const string &pattern, const string &metaFile,
                                       bool transform, bool performance, unsigned nBuffers,
                                       unsigned minBufferSize, unsigned bufferOverflowSize,
                                       unsigned nDecompressionThreads, unsigned nTransformThreads,
                                       unsigned nSearchThreads) {
  _searchFile = file;
  _pattern = pattern;
  _metaFile = metaFile.empty() ? nullptr : new ESFMetaFile(metaFile, std::ios::in);
  _toBeTransformed = transform;
  _performance = performance;
  _nBuffers = nBuffers;
  _minBufferSize = minBufferSize;
  setNumberOfDecompressionThreads(nDecompressionThreads);
  setNumberOfTransformationThreads(nTransformThreads);
  setNumberOfSearchThreads(nSearchThreads);
  initializeQueues();
}

// _____________________________________________________________________________________________________________________
ExternStringFinder::~ExternStringFinder() {
  delete _metaFile;
  for (auto chunk: _fileChunks) {
    delete chunk;
  }
  if (_file) {
    _file.close();
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
void ExternStringFinder::setNumberOfDecompressionThreads(unsigned int nDecompressionThreads) {
  if (_metaFile == nullptr) {
    _nDecompressionThreads = 0;
    return;
  }
  _nDecompressionThreads = nDecompressionThreads > 0 ? nDecompressionThreads : 1;
  if (_toBeTransformed) {
    _toBeTransformedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
  } else {
    _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nDecompressionThreads);
  }
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::setNumberOfTransformationThreads(unsigned int nTransformationThreads) {
  if (!_toBeTransformed) {
    _nTransformThreads = 0;
    return;
  }
  _nTransformThreads = nTransformationThreads > 0 ? nTransformationThreads : 1;
  _toBeSearchedChunkQueue.setNumberOfWriteThreads(_nTransformThreads);
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::setNumberOfSearchThreads(unsigned int nSearchThreads) {
  _nSearchThreads = nSearchThreads > 0 ? nSearchThreads : 1;
  _availableChunkQueue.setNumberOfWriteThreads(_nSearchThreads);
}

// _____________________________________________________________________________________________________________________
string ExternStringFinder::toString() const {
  string out;
  if (_performance) {
    out += "ExternStringFinder::find() on keyword '" + _pattern + "' and inputfile '" + _searchFile + "'.\n\n";
    double compTime = _totalReadTime + _totalTransformTime + _totalDecompressionTime + _totalSearchTime;
    double waitTime = _totalReadWaitTime + _totalDecompressionWaitTime + _totalTransformWaitTime + _totalSearchWaitTime;
    output::Node root(std::to_string(_totalTime) + " s in total (processor time: " + std::to_string(compTime) + " s)");
    output::Tree tree(root, "Performance report of ExternStringFinder::find()");
    tree.getRoot().addChild(output::Node("reading input (1 thread): " + std::to_string(_totalReadTime + _totalReadWaitTime) + " s"));
    tree.getRoot().getChild(-1).addChild(output::Node("computation: " + std::to_string(_totalReadTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalReadTime, compTime))) + "%)"));
    tree.getRoot().getChild(-1).addChild(output::Node("waiting    : " + std::to_string(_totalReadWaitTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalReadWaitTime, waitTime))) + "%)"));
    if (_metaFile != nullptr) {
      tree.getRoot().addChild(output::Node("decompression (" + std::to_string(_nDecompressionThreads) + " threads): " + std::to_string(_totalDecompressionTime + _totalDecompressionWaitTime) + " s"));
      tree.getRoot().getChild(-1).addChild(output::Node("computation: " + std::to_string(_totalDecompressionTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalDecompressionTime, compTime))) + "%)"));
      tree.getRoot().getChild(-1).addChild(output::Node("waiting    : " + std::to_string(_totalDecompressionWaitTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalDecompressionWaitTime, waitTime))) + "%)"));
    }
    if (_toBeTransformed) {
      tree.getRoot().addChild(output::Node("transformation: (" + std::to_string(_nTransformThreads) + " threads): " + std::to_string(_totalTransformTime + _totalTransformWaitTime) + " s"));
      tree.getRoot().getChild(-1).addChild(output::Node("computation: " + std::to_string(_totalTransformTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalTransformTime, compTime))) + "%)"));
      tree.getRoot().getChild(-1).addChild(output::Node("waiting    : " + std::to_string(_totalTransformWaitTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalTransformWaitTime, waitTime))) + "%)"));
    }
    tree.getRoot().addChild(output::Node("searching: (" + std::to_string(_nSearchThreads) + " threads): " + std::to_string(_totalSearchTime + _totalSearchWaitTime) + " s"));
    tree.getRoot().getChild(-1).addChild(output::Node("computation: " + std::to_string(_totalSearchTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalSearchTime, compTime))) + "%)"));
    tree.getRoot().getChild(-1).addChild(output::Node("waiting    : " + std::to_string(_totalSearchWaitTime) + " (" + std::to_string(static_cast<int>(math::percentage(_totalSearchWaitTime, waitTime))) + "%)"));
    out += tree.parse();
  } else {
    out += "No performance available (use --performance (-p) flag for performance reports.";
  }
  return out;
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::readChunk(std::istream &input,
                                   sf::utils::TSQueue<FileChunk *> &popFromQueue,
                                   sf::utils::TSQueue<FileChunk *> &pushToQueue) {
  Timer waitTimer;
  Timer readingTimer;
  _totalNumberBytesRead = 0;
  if (_metaFile == nullptr) {
    while (true) {
      if (_performance) { waitTimer.start(); }
      FileChunk *currentChunk = popFromQueue.pop();
      if (_performance) { waitTimer.stop(); }
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
  }
}

void ExternStringFinder::decompressChunk(sf::utils::TSQueue<FileChunk *> &popFromQueue, sf::utils::TSQueue<FileChunk *> &pushToQueue) {
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
  }
}

void ExternStringFinder::transformChunk(std::function<int(int)> transformer,
                                        sf::utils::TSQueue<FileChunk *> &popFromQueue,
                                        sf::utils::TSQueue<FileChunk *> &pushToQueue) {
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
  }
}

void ExternStringFinder::searchBuffers(sf::utils::TSQueue<FileChunk *> &popFromQueue, sf::utils::TSQueue<FileChunk *> &pushToQueue) {
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
  }
}

// _____________________________________________________________________________________________________________________
vector<string::size_type> ExternStringFinder::find() {
  Timer timer;
  if (_performance) {
    timer.start(true);
  }

  if (_toBeTransformed) {
    transform(_pattern.begin(), _pattern.end(), _pattern.begin(), ::tolower);
  }

  buildThreads();

  _readThread.join();
  for (auto &decompressionThread: _decompressionThreads) {
    decompressionThread.join();
  }
  for (auto &transformThread: _transformThreads) {
    transformThread.join();
  }
  for (auto &searchThread: _searchThreads) {
    searchThread.join();
  }

  vector<string::size_type> mergedResult;

  // TODO<lfreist>: this can run in a thread parallel to search as well...
  while (true) {
    vector<string::size_type> elem = _partialResultsQueue.pop(vector<string::size_type>(1, 0));
    if (elem.size() == 1) {
      if (elem[0] == 0) break;
    }
    std::copy(elem.begin(), elem.end(), std::back_inserter(mergedResult));
  }

  if (_performance) {
    timer.stop();
    _totalTime = timer.elapsedSeconds();
  }
  // TODO: should be sorted in place while merging vectors
  std::sort(mergedResult.begin(), mergedResult.end());
  return mergedResult;
}

// _____________________________________________________________________________________________________________________
void ExternStringFinder::buildThreads() {
  // read from stdin
  if (_searchFile == "-") {
    _readThread = std::thread(&ExternStringFinder::readChunk,
                              this,
                              std::ref(std::cin),
                              std::ref(_availableChunkQueue),
                              (_metaFile == nullptr ? (_toBeTransformed ? std::ref(_toBeTransformedChunkQueue)
                                                                        : std::ref(_toBeSearchedChunkQueue))
                                                    : std::ref(_toBeDecompressedChunkQueue))
    );
  } else {  // read from file
    _file = std::ifstream(_searchFile);
    if (!_file) {
      throw sf_utils::FileReadError();
    }
    _readThread = std::thread(&ExternStringFinder::readChunk,
                              this,
                              std::ref(_file),
                              std::ref(_availableChunkQueue),
                              (_metaFile == nullptr ? (_toBeTransformed ? std::ref(_toBeTransformedChunkQueue)
                                                                        : std::ref(_toBeSearchedChunkQueue))
                                                    : std::ref(_toBeDecompressedChunkQueue))
    );
  }
  _decompressionThreads.resize(_nDecompressionThreads);
  _transformThreads.resize(_nTransformThreads);
  _searchThreads.resize(_nSearchThreads);
  for (auto &thread: _decompressionThreads) {
    thread = std::thread(&ExternStringFinder::decompressChunk,
                         this,
                         std::ref(_toBeDecompressedChunkQueue),
                         (_toBeTransformed ? std::ref(_toBeTransformedChunkQueue) : std::ref(_toBeSearchedChunkQueue)));
  }
  for (auto &thread: _transformThreads) {
    thread = std::thread(&ExternStringFinder::transformChunk,
                         this,
                         ::tolower,
                         std::ref(_toBeTransformedChunkQueue),
                         std::ref(_toBeSearchedChunkQueue));
  }
  for (auto &thread: _searchThreads) {
    thread = std::thread(&ExternStringFinder::searchBuffers,
                         this,
                         std::ref(_toBeSearchedChunkQueue),
                         std::ref(_availableChunkQueue));
  }
}

std::ostream &operator<<(std::ostream &os, const ExternStringFinder &esf) {
  os << esf.toString();
  return os;
}