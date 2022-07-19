// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <unistd.h>

#include <stdexcept>

#include "ESFCompressedPOSIXReader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ESFCompressedPOSIXReader::ESFCompressedPOSIXReader(const std::string &filePath,
                                                   const std::string &metaFilePath) : _metaFile(metaFilePath,
                                                                                                std::ios::in) {
  _filePath = filePath;
  _metaFilePath = metaFilePath;
  _fd = ::open(_filePath.c_str(), O_RDONLY);
  if (_fd == -1) { throw std::runtime_error("Could not open file '" + filePath + "'."); }
}

// _____________________________________________________________________________________________________________________
ESFCompressedPOSIXReader::ESFCompressedPOSIXReader(const ESFCompressedPOSIXReader &reader)
  : ESFReader(reader), _metaFile(reader._metaFilePath, std::ios::in) {
  _filePath = reader._filePath;
  _metaFilePath = reader._metaFilePath;
  _fd = ::open(reader._filePath.c_str(), O_RDONLY);
}

// _____________________________________________________________________________________________________________________
ESFCompressedPOSIXReader::ESFCompressedPOSIXReader(ESFCompressedPOSIXReader &&reader) noexcept
  : _metaFile(reader._metaFilePath, std::ios::in) {
  _filePath = std::move(reader._filePath);
  _metaFilePath = std::move(reader._metaFilePath);
  _fd = ::open(_filePath.c_str(), O_RDONLY);
}

// _____________________________________________________________________________________________________________________
ESFCompressedPOSIXReader::~ESFCompressedPOSIXReader() {
  ::close(_fd);
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ESFCompressedPOSIXReader::chunkProvider() {
  std::optional<chunkSize> cs = _metaFile.nextChunkSize();
  if (!cs) { return {}; }
  std::optional<FileChunk> optionalChunk{FileChunk("", cs.value().originalSize, _offset)};
  auto &chunk = optionalChunk.value();
  chunk.strPtr()->resize(cs.value().compressedSize);
  auto bytes_read = ::read(_fd, &chunk.strPtr()->at(0), cs.value().compressedSize);
  if (bytes_read < 0) { throw std::runtime_error("Error while reading: " + std::to_string(bytes_read)); }
  _offset += cs.value().originalSize;
  return optionalChunk;
}

}  // namespace sf::utils::readers
