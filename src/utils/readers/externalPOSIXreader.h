// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fcntl.h>

#include <optional>
#include <string>
#include <memory>

#include "./BaseReader.h"
#include "../FileChunk.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_

namespace sf::utils::readers {

class externalPOSIXreader : public BaseReader {
 public:
  explicit externalPOSIXreader(const std::string& filePath, uint32_t maxChunkSize = 16777216);
  // move constructor
  externalPOSIXreader(externalPOSIXreader&& reader) noexcept ;
  ~externalPOSIXreader() override;

  std::optional<FileChunk> reader() override;

 private:
  std::unique_ptr<int> _fdPtr = std::make_unique<int>(-1);
  strtype _prevOverhead;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_EXTERNALPOSIXREADER_H_
