// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <iostream>
#include <exception>

#include "SFMetaFile.h"

namespace sf::utils {

// _____________________________________________________________________________________________________________________
SFMetaFile::SFMetaFile(const std::string &filePath, std::ios::openmode mode) {
  _openMode = mode | std::ios::binary;
  _metaFileStream.open(filePath, _openMode);
  if (!_metaFileStream.is_open()) {
    throw std::runtime_error("Cannot open file '" + filePath + "'");
  }
}

// _____________________________________________________________________________________________________________________
SFMetaFile::~SFMetaFile() {
  _metaFileStream.close();
}

// _____________________________________________________________________________________________________________________
std::optional<chunkSize> SFMetaFile::nextChunkSize() {
  _metaFileStream.read(reinterpret_cast<char *>(&_chunkSize), sizeof(_chunkSize));
  if (_metaFileStream.eof()) {
    return {};
  }
  return _chunkSize;
}

// _____________________________________________________________________________________________________________________
void SFMetaFile::writeChunkSize(chunkSize chunk) {
  assert(_openMode == (std::ios::out | std::ios::binary));
  _metaFileStream.write(reinterpret_cast<char *>(&chunk), sizeof(chunkSize));
}

}  // namespace sf::utils
