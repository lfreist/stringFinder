// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fstream>

#include "./ESFReader.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_ESFIFSTREAMREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_ESFIFSTREAMREADER_H_

namespace sf::utils::readers {

class ESFIFSTREAMReader : public ESFReader {
 public:
  explicit ESFIFSTREAMReader(const std::string &filePath,
                             uint32_t minChunkSize = 16776192,
                             uint32_t maxChunkSize = 16777216);
  // copy constructor
  ESFIFSTREAMReader(const ESFIFSTREAMReader &reader);
  // move constructor
  ESFIFSTREAMReader(ESFIFSTREAMReader &&reader) noexcept;
  ~ESFIFSTREAMReader() override;

  std::optional<FileChunk> chunkProvider() override;

 private:
  std::ifstream _fileStream;
  uint32_t _minChunkSize;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_ESFIFSTREAMREADER_H_
