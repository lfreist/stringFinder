// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <stdexcept>

#include "ESFIFSTREAMReader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ESFIFSTREAMReader::ESFIFSTREAMReader(const std::string& filePath, uint32_t minChunkSize, uint32_t maxChunkSize) {
  _filePath = filePath;
  _fileStream.open(_filePath);
  if (!_fileStream) { throw std::runtime_error("Could not read file '" + filePath + "'."); }
  if (minChunkSize > maxChunkSize) { throw std::runtime_error("'minChunkSize' must be smaller than 'maxChunkSize'."); }
  _minChunkSize = minChunkSize;
  _maxChunkSize = maxChunkSize;
}

// _____________________________________________________________________________________________________________________
ESFIFSTREAMReader::ESFIFSTREAMReader(const ESFIFSTREAMReader &reader)  : ESFReader(reader) {
  _filePath = reader._filePath;
  _fileStream.open(reader._filePath);
  _minChunkSize = reader._minChunkSize;
}

// _____________________________________________________________________________________________________________________
ESFIFSTREAMReader::ESFIFSTREAMReader(ESFIFSTREAMReader &&reader) noexcept {
  _filePath = std::move(reader._filePath);
  _fileStream.open(_filePath);
  _minChunkSize = reader._minChunkSize;
}

// _____________________________________________________________________________________________________________________
ESFIFSTREAMReader::~ESFIFSTREAMReader() {
  _fileStream.close();
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ESFIFSTREAMReader::chunkProvider() {
  std::optional<FileChunk> optionalChunk {FileChunk()};
  auto& chunk = optionalChunk.value();
  chunk.strPtr()->reserve(_maxChunkSize);
  chunk.strPtr()->resize(_minChunkSize);
  if (_fileStream.peek() == EOF) { return {}; }
  _fileStream.read(chunk.strPtr()->data(), _minChunkSize);
  if (!_fileStream) {
    chunk.strPtr()->resize(_fileStream.gcount());
    chunk.strPtr()->pop_back();  // remove EOF
  }
  else {
    bool NLError = true;
    char nextByte = 0;
    for (uint32_t counter = _minChunkSize; counter < _maxChunkSize; ++counter) {
      if (_fileStream.eof()) {
        NLError = false;
        break;
      }
      _fileStream.get(nextByte);
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