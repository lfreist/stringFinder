// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <iostream>
#include <stdexcept>

#include "ESFMetaFile.h"


ESFMetaFile::ESFMetaFile(const std::string& filePath) {
  _metaFile.open(filePath, std::ios::in);
  if (!_metaFile.is_open()) {
    throw std::invalid_argument("Cannot open file");
  }
  std::string maxChunkSizeStr;
  getline(_metaFile, maxChunkSizeStr);
  _maxChunkSize = atoi(maxChunkSizeStr.c_str());
}

ESFMetaFile::~ESFMetaFile() {
  _metaFile.close();
}

std::pair<unsigned int, unsigned int> ESFMetaFile::nextChunkPair() {
  std::string orig_size;
  std::string comp_size;
  if (getline(_metaFile, orig_size, ' ')) {
    if (getline(_metaFile, comp_size)) {
      return std::make_pair(atoi(orig_size.c_str()), atoi(comp_size.c_str()));
    }
  }
  return std::make_pair(0, 0);
}
unsigned int ESFMetaFile::getMaxChunkSize() const {
  return _maxChunkSize;
}
