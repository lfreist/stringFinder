// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "FileChunk.h"

#include <utility>
#include <algorithm>

namespace sf::utils {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
FileChunk::FileChunk() = default;

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(std::string data, ulong offset) {
  _content = std::move(data);
  _offset = offset;
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(std::string data, ulong originalSize, ulong offset) {
  _content = std::move(data);
  _originalSize = originalSize;
  _offset = offset;
}

// _____________________________________________________________________________________________________________________
void FileChunk::setContent(std::string data) {
  _content = std::move(data);
}

// _____________________________________________________________________________________________________________________
bool FileChunk::isCompressed() const {
  return _offset != _content.size();
}

// _____________________________________________________________________________________________________________________
const std::string &FileChunk::str() const {
  return _content;
}

// _____________________________________________________________________________________________________________________
uint64_t FileChunk::getOffset() const {
  return _offset;
}

// _____________________________________________________________________________________________________________________
uint64_t FileChunk::getOriginalSize() const {
  return _originalSize;
}

// ----- private -------------------------------------------------------------------------------------------------------

}  // namespace sf::utils