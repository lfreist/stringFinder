// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_ESFMETAFILE_H_
#define SRC_ESFMETAFILE_H_

#include <string>
#include <fstream>

#include <gtest/gtest.h>

class ESFMetaFile {
 public:
  explicit ESFMetaFile(const std::string& filePath);
  ~ESFMetaFile();

  std::pair<unsigned int, unsigned int> nextChunkPair();
  unsigned int getMaxChunkSize() const;

 private:
  unsigned int _maxChunkSize;
  std::fstream _metaFile;

  FRIEND_TEST(ESFMetaFileTest, constructor);
};

#endif  // SRC_ESFMETAFILE_H_