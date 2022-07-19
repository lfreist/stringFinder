// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "./ISFBaseReader.h"
#include "../FileChunk.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_ISFREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_ISFREADER_H_

namespace sf::utils::readers {

class ISFReader : public ISFBaseReader {
 public:
  explicit ISFReader(uint32_t minChunkSize = 16776192, uint32_t maxChunkSize = 16777216);
  explicit ISFReader(const std::string &filePath, uint32_t minChunkSize = 16776192, uint32_t maxChunkSize = 16777216);
  explicit ISFReader(const std::vector<std::string> &files,
                     uint32_t minChunkSize = 16776192,
                     uint32_t maxChunkSize = 16777216);

  uint64_t readFile(const std::string &file);
  std::optional<FileChunk> chunkProvider() override;

 protected:
  uint32_t _minChunkSize;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_ISFREADER_H_
