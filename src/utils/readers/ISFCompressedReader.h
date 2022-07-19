// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include "./ISFBaseReader.h"
#include "../SFMetaFile.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_ISFCOMPRESSEDREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_ISFCOMPRESSEDREADER_H_

namespace sf::utils::readers {

class ISFCompressedReader : public ISFBaseReader {
 public:
  ISFCompressedReader(const std::string &filePath, const std::string &metaFilePath);

  uint64_t readFile(const std::string& filePath, const std::string& metaFilePath);

  std::optional<FileChunk> chunkProvider() override;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_ISFCOMPRESSEDREADER_H_
