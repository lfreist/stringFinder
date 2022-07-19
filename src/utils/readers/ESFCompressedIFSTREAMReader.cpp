// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "ESFCompressedIFSTREAMReader.h"

namespace sf::utils::readers {

// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ESFCompressedIFSTREAMReader::ESFCompressedIFSTREAMReader(const std::string &filePath, const std::string &metaFilePath)
  : _metaFile(metaFilePath, std::ios::in) {
  _filePath = filePath;
  _metaFilePath = metaFilePath;
  _fileStream.open(_filePath);
  if (!_fileStream) { throw std::runtime_error("Could not read file '" + _filePath + "'."); }
}

// _____________________________________________________________________________________________________________________
ESFCompressedIFSTREAMReader::ESFCompressedIFSTREAMReader(const ESFCompressedIFSTREAMReader &reader)
  : ESFReader(reader), _metaFile(reader._metaFilePath, std::ios::in) {
  _filePath = reader._filePath;
  _metaFilePath = reader._metaFilePath;
  _fileStream.open(_filePath);
}

// _____________________________________________________________________________________________________________________
ESFCompressedIFSTREAMReader::ESFCompressedIFSTREAMReader(ESFCompressedIFSTREAMReader &&reader) noexcept: _metaFile(
  reader._metaFilePath, std::ios::in) {
  _filePath = std::move(reader._filePath);
  _metaFilePath = std::move(reader._metaFilePath);
  _fileStream.open(_filePath);
}

// _____________________________________________________________________________________________________________________
ESFCompressedIFSTREAMReader::~ESFCompressedIFSTREAMReader() {
  _fileStream.close();
}

// _____________________________________________________________________________________________________________________
std::optional<FileChunk> ESFCompressedIFSTREAMReader::chunkProvider() {
  std::optional<chunkSize> cs = _metaFile.nextChunkSize();
  if (!cs) { return {}; }
  std::optional<FileChunk> optionalChunk{FileChunk("", cs.value().originalSize, _offset)};
  auto &chunk = optionalChunk.value();
  chunk.strPtr()->resize(cs.value().compressedSize);
  _fileStream.read(chunk.strPtr()->data(), cs.value().compressedSize);
  _offset += cs.value().originalSize;
  return optionalChunk;
}

}  // namespace sf::utils::readers