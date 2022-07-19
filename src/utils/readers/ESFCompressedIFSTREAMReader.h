// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fstream>
#include <memory>

#include "./ESFReader.h"
#include "../SFMetaFile.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_ESFCOMPRESSEDIFSTREAMREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_ESFCOMPRESSEDIFSTREAMREADER_H_

namespace sf::utils::readers {

class ESFCompressedIFSTREAMReader : public ESFReader {
 public:
  ESFCompressedIFSTREAMReader(const std::string &filePath, const std::string &metaFilePath);
  // copy constructor
  ESFCompressedIFSTREAMReader(const ESFCompressedIFSTREAMReader &reader);
  // move constructor
  ESFCompressedIFSTREAMReader(ESFCompressedIFSTREAMReader&& reader) noexcept ;
  ~ESFCompressedIFSTREAMReader() override;

  std::optional<FileChunk> chunkProvider() override;

 protected:
  std::ifstream _fileStream;
  std::string _metaFilePath;
  SFMetaFile _metaFile;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_ESFCOMPRESSEDIFSTREAMREADER_H_
