// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <iostream>
#include <stdexcept>

#include "ESFMetaFile.h"

namespace sf::utils {

// _____________________________________________________________________________________________________________________
ESFMetaFile::ESFMetaFile(const std::string &filePath, std::ios::openmode mode) {
  _mode = mode | std::ios::binary;
  _metaFile.open(filePath, _mode);
  _maxOriginalSize = 0;
  if (!_metaFile.is_open()) {
    throw std::invalid_argument("Cannot open file");
  }
  if (_mode == (std::ios::in | std::ios::binary)) {
    _metaFile.read(reinterpret_cast<char *>(&_maxOriginalSize), sizeof(size_t));
  }
}

// _____________________________________________________________________________________________________________________
ESFMetaFile::~ESFMetaFile() {
  _metaFile.close();
}

// _____________________________________________________________________________________________________________________
chunkSize ESFMetaFile::nextChunkSize() {
  _metaFile.read(reinterpret_cast<char *>(&_chunkSize), sizeof(_chunkSize));
  if (_metaFile.eof()) {
    return _endOfFileChunkSize;
  }
  return _chunkSize;
}

// _____________________________________________________________________________________________________________________
size_t ESFMetaFile::getMaxOriginalSize() const {
  return _maxOriginalSize;
}

// _____________________________________________________________________________________________________________________
void ESFMetaFile::writeMaxOriginalSize(size_t maxOriginalSize) {
  assert(_mode == (std::ios::out | std::ios::binary));
  _metaFile.write(reinterpret_cast<char *>(&maxOriginalSize), sizeof(size_t));
}

// _____________________________________________________________________________________________________________________
void ESFMetaFile::writeChunkSize(chunkSize chunk) {
  assert(_mode == (std::ios::out | std::ios::binary));
  _metaFile.write(reinterpret_cast<char *>(&chunk), sizeof(chunkSize));
}

}  // namespace sf::utils
