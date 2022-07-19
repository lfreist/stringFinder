// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <fcntl.h>

#include <string>

#include "./ESFReader.h"
#include "../SFMetaFile.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_EXTERNALCOMPRESSEDPOSIXREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_EXTERNALCOMPRESSEDPOSIXREADER_H_

namespace sf::utils::readers {

class ESFCompressedPOSIXReader : public ESFReader {
 public:
  ESFCompressedPOSIXReader(const std::string &filePath, const std::string &metaFilePath);
  // copy constructor
  ESFCompressedPOSIXReader(const ESFCompressedPOSIXReader &reader);
  // move constructor
  ESFCompressedPOSIXReader(ESFCompressedPOSIXReader&& reader) noexcept ;
  ~ESFCompressedPOSIXReader() override;

  std::optional<FileChunk> chunkProvider() override;

 protected:
  std::string _metaFilePath;
  SFMetaFile _metaFile;
  int _fd = -1;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_EXTERNALCOMPRESSEDPOSIXREADER_H_
