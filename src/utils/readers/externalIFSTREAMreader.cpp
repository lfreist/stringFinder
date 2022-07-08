// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <stdexcept>

#include "externalIFSTREAMreader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
externalIFSTREAMreader::externalIFSTREAMreader(const std::string& filePath, uint32_t minChunkSize, uint32_t maxChunkSize) {
  _fileStreamPtr->open(filePath);
  if (!_fileStreamPtr) { throw std::runtime_error("Could not read file '" + filePath + "'."); }
  if (minChunkSize > maxChunkSize) { throw std::runtime_error("'minChunkSize' must be smaller than 'maxChunkSize'."); }
  _minChunkSize = minChunkSize;
  _maxChunkSize = maxChunkSize;
}

// _____________________________________________________________________________________________________________________
externalIFSTREAMreader::externalIFSTREAMreader(externalIFSTREAMreader &&reader) noexcept {
  _fileStreamPtr = std::move(reader._fileStreamPtr);
  _minChunkSize = reader._minChunkSize;
}

// _____________________________________________________________________________________________________________________
externalIFSTREAMreader::~externalIFSTREAMreader() {
  if (_fileStreamPtr && _fileStreamPtr->is_open())
    _fileStreamPtr->close();
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> externalIFSTREAMreader::reader() {
  std::optional<FileChunk> optionalChunk {FileChunk()};
  auto& chunk = optionalChunk.value();
  chunk.strPtr()->reserve(_maxChunkSize);
  chunk.strPtr()->resize(_minChunkSize);
  if (_fileStreamPtr->peek() == EOF) { return {}; }
  _fileStreamPtr->read(chunk.strPtr()->data(), _minChunkSize);
  if (!_fileStreamPtr) {
    chunk.strPtr()->resize(_fileStreamPtr->gcount());
    chunk.strPtr()->pop_back();  // remove EOF
  }
  else {
    bool NLError = true;
    char nextByte = 0;
    for (uint32_t counter = _minChunkSize; counter < _maxChunkSize; ++counter) {
      if (_fileStreamPtr->eof()) {
        NLError = false;
        break;
      }
      _fileStreamPtr->get(nextByte);
      if (nextByte == '\n') {
        NLError = false;
        break;
      }
      chunk.strPtr()->push_back(nextByte);
    }
    if (NLError) { throw std::runtime_error("Could not find new line."); }
  }
  chunk.setOffset(_offset);
  _offset += chunk.strPtr()->length();
  return optionalChunk;
}

}  // namespace sf::utils::readers