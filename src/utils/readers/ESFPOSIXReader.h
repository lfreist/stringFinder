// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fcntl.h>

#include <optional>
#include <string>

#include "./ESFReader.h"
#include "../FileChunk.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_

namespace sf::utils::readers {

class ESFPOSIXReader : public ESFReader {
 public:
  explicit ESFPOSIXReader(const std::string& filePath, uint32_t maxChunkSize = 16777216);
  // copy constructor
  ESFPOSIXReader(const ESFPOSIXReader &reader);
  // move constructor
  ESFPOSIXReader(ESFPOSIXReader&& reader) noexcept ;
  ~ESFPOSIXReader() override;

  std::optional<FileChunk> chunkProvider() override;

 protected:
  int _fd = -1;
  strtype _prevOverhead;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_
