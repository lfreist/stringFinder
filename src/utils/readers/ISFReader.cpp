// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "./ESFIFSTREAMReader.h"
#include "./ISFReader.h"


namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ISFReader::ISFReader(uint32_t minChunkSize, uint32_t maxChunkSize) {
  _minChunkSize = minChunkSize;
  _maxChunkSize = maxChunkSize;
}

// _____________________________________________________________________________________________________________________
ISFReader::ISFReader(const std::string &filePath, uint32_t minChunkSize, uint32_t maxChunkSize) {
  _minChunkSize = minChunkSize;
  _maxChunkSize = maxChunkSize;
  readFile(filePath);
}

// _____________________________________________________________________________________________________________________
ISFReader::ISFReader(const std::vector<std::string> &files, uint32_t minChunkSize, uint32_t maxChunkSize) {
  _minChunkSize = minChunkSize;
  _maxChunkSize = maxChunkSize;
  for (auto& file: files) {
    readFile(file);
  }
}

// _____________________________________________________________________________________________________________________
uint64_t ISFReader::readFile(const std::string &file) {
  ESFIFSTREAMReader reader(file, _minChunkSize, _maxChunkSize);
  for (uint64_t counter = 0;; ++counter) {
    auto chunk = reader.chunkProvider();
    if (!chunk) { return counter; }
    _fileChunks.push_back(chunk.value());
  }
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ISFReader::chunkProvider() {
  if (_currentIndex < _fileChunks.size()) {
    return _fileChunks[_currentIndex++];
  }
  return {};
}

}  // namespace sf::utils::readers