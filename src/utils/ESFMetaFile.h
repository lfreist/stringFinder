// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_ESFMETAFILE_H_
#define SRC_ESFMETAFILE_H_

#include <string>
#include <fstream>

#include <gtest/gtest.h>

namespace sf::utils {

struct chunkSize {
  size_t originalSize;
  size_t compressedSize;

  bool operator==(const chunkSize &other) const {
    return (originalSize == other.originalSize) && (compressedSize == other.compressedSize);
  }
};

class ESFMetaFile {
 public:
  explicit ESFMetaFile(const std::string &filePath, std::ios::openmode mode);
  ~ESFMetaFile();

  chunkSize nextChunkSize();
  size_t getMaxOriginalSize() const;

  void writeMaxOriginalSize(size_t maxOriginalSize);
  void writeChunkSize(chunkSize chunk);

 private:
  size_t _maxOriginalSize;
  chunkSize _chunkSize;
  const chunkSize _endOfFileChunkSize{0, 0};
  std::fstream _metaFile;
  std::ios::openmode _mode;

  FRIEND_TEST(ESFMetaFileTest, constructor);
  FRIEND_TEST(ESFMetaFileTest, writeAndRead);
};

}  // namespace sf::utils

#endif  // SRC_ESFMETAFILE_H_