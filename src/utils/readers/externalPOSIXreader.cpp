// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <unistd.h>

#include <stdexcept>

#include "externalPOSIXreader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
externalPOSIXreader::externalPOSIXreader(const std::string& filePath, uint32_t maxChunkSize) {
  _fdPtr = std::make_unique<int>(::open(filePath.c_str(), O_RDONLY));
  if (*_fdPtr == -1) { throw std::runtime_error("Could not open file '" + filePath + "'."); }
  _prevOverhead.reserve(_maxChunkSize >> 2);
  _maxChunkSize = maxChunkSize;
}

// _____________________________________________________________________________________________________________________
externalPOSIXreader::externalPOSIXreader(externalPOSIXreader &&reader) noexcept {
  _fdPtr = std::move(reader._fdPtr);
}

// _____________________________________________________________________________________________________________________
externalPOSIXreader::~externalPOSIXreader() {
  if (_fdPtr != nullptr) {  // false if move constructor was called previously
    ::close(*_fdPtr);
  }
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> externalPOSIXreader::reader() {
  std::optional<FileChunk> optionalChunk{FileChunk(_prevOverhead, _offset)};
  auto& chunk = optionalChunk.value();
  chunk.strPtr()->resize(_maxChunkSize);
  auto bytes_read = ::read(*_fdPtr, &chunk.strPtr()->at(_prevOverhead.length()), _maxChunkSize - (_prevOverhead.length()));
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