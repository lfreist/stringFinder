// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <optional>

#include "../FileChunk.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_BASEREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_BASEREADER_H_

namespace sf::utils::readers {

class BaseReader {
 public:
  virtual ~BaseReader() = default;
  virtual std::optional<FileChunk> chunkProvider() {};

 protected:
  // max 16 MiB per chunk by default
  uint32_t _maxChunkSize = 16777216;
  // relative offset of current chunk in respect of start of file or whatever is read
  uint64_t _offset = 0;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_BASEREADER_H_
