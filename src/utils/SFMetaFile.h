// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_ESFMETAFILE_H_
#define SRC_ESFMETAFILE_H_

#include <string>
#include <fstream>
#include <optional>

#include <gtest/gtest.h>

namespace sf::utils {

struct chunkSize {
  uint32_t originalSize;
  uint32_t compressedSize;

  bool operator==(const chunkSize &other) const {
    return (originalSize == other.originalSize) && (compressedSize == other.compressedSize);
  }
};

class SFMetaFile {
 public:
  explicit SFMetaFile(const std::string &filePath, std::ios::openmode mode);
  ~SFMetaFile();

  std::optional<chunkSize> nextChunkSize();

  void writeChunkSize(chunkSize chunk);

 private:
  chunkSize _chunkSize {};
  std::fstream _metaFileStream;
  std::ios::openmode _openMode;

  FRIEND_TEST(ESFMetaFileTest, constructor);
  FRIEND_TEST(ESFMetaFileTest, writeAndRead);
};

}  // namespace sf::utils

#endif  // SRC_ESFMETAFILE_H_