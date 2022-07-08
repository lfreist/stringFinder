// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fstream>
#include <memory>

#include "./BaseReader.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_EXTERNALIFSTREAMREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_EXTERNALIFSTREAMREADER_H_

namespace sf::utils::readers {

class externalIFSTREAMreader : public BaseReader {
 public:
  explicit externalIFSTREAMreader(const std::string& filePath, uint32_t minChunkSize = 16776192, uint32_t maxChunkSize = 16777216);
  // move constructor
  externalIFSTREAMreader(externalIFSTREAMreader&& reader) noexcept ;
  ~externalIFSTREAMreader() override;

  std::optional<FileChunk> reader() override;

 private:
  std::unique_ptr<std::ifstream> _fileStreamPtr = std::make_unique<std::ifstream>();
  uint32_t _minChunkSize;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_EXTERNALIFSTREAMREADER_H_
