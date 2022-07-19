// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "./ISFCompressedReader.h"
#include "./ESFCompressedPOSIXReader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ISFCompressedReader::ISFCompressedReader(const std::string &filePath, const std::string &metaFilePath) {
  readFile(filePath, metaFilePath);
}

// _____________________________________________________________________________________________________________________
uint64_t ISFCompressedReader::readFile(const std::string &filePath, const std::string &metaFilePath) {
  ESFCompressedPOSIXReader reader(filePath, metaFilePath);
  for (uint64_t counter = 0;; ++counter) {
    auto chunk = reader.chunkProvider();
    if (!chunk) { return counter; }
    _fileChunks.push_back(chunk.value());
  }
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ISFCompressedReader::chunkProvider() {
  if (_currentIndex < _fileChunks.size()) {
    return _fileChunks[_currentIndex++];
  }
  return {};
}

// _____________________________________________________________________________________________________________________


// _____________________________________________________________________________________________________________________


// _____________________________________________________________________________________________________________________

}  // namespace sf::utils::readers