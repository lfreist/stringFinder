// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <unistd.h>

#include <stdexcept>

#include "ESFPOSIXReader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ESFPOSIXReader::ESFPOSIXReader(const std::string& filePath, uint32_t maxChunkSize) {
  _filePath = filePath;
  _fd = ::open(_filePath.c_str(), O_RDONLY);
  if (_fd == -1) { throw std::runtime_error("Could not open file '" + filePath + "'."); }
  _prevOverhead.reserve(_maxChunkSize >> 2);
  _maxChunkSize = maxChunkSize;
}

// _____________________________________________________________________________________________________________________
ESFPOSIXReader::ESFPOSIXReader(const ESFPOSIXReader &reader)  : ESFReader(reader) {
  _filePath = reader._filePath;
  _prevOverhead = reader._prevOverhead;
  _fd = ::open(reader._filePath.c_str(), O_RDONLY);
}

// _____________________________________________________________________________________________________________________
ESFPOSIXReader::ESFPOSIXReader(ESFPOSIXReader &&reader) noexcept {
  _filePath = std::move(reader._filePath);
  _fd = ::open(_filePath.c_str(), O_RDONLY);
  _prevOverhead = std::move(reader._prevOverhead);
}

// _____________________________________________________________________________________________________________________
ESFPOSIXReader::~ESFPOSIXReader() {
  ::close(_fd);
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ESFPOSIXReader::chunkProvider() {
  std::optional<FileChunk> optionalChunk{FileChunk(_prevOverhead, _offset)};
  auto& chunk = optionalChunk.value();
  chunk.strPtr()->resize(_maxChunkSize);
  auto bytes_read = ::read(_fd, &chunk.strPtr()->at(_prevOverhead.length()), _maxChunkSize - (_prevOverhead.length()));
  if (bytes_read < 0) { throw std::runtime_error("Error while reading: " + std::to_string(bytes_read)); }
  if (bytes_read == 0) { return {}; }
  _prevOverhead.resize(0);
  for (unsigned index = _maxChunkSize - 1; index > 0; --index) {
    char lastChar = chunk.strPtr()->at(index);
    chunk.strPtr()->pop_back();
    if (lastChar == '\n') { break; }
    _prevOverhead.push_back(lastChar);
  }
  if (chunk.strPtr()->empty()) { throw std::runtime_error("Could not find new line."); }
  _offset += chunk.strPtr()->length();
  return optionalChunk;
}

}  // namespace sf::utils::readers