// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <vector>

#include "./BaseReader.h"
#include "../FileChunk.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_ISFBASEREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_ISFBASEREADER_H_

namespace sf::utils::readers {

class ISFBaseReader : public BaseReader{
 public:
  void reset() { _currentIndex = 0; }
  void setIndex(uint64_t index) { _currentIndex = index; }

 protected:
  uint64_t _currentIndex = 0;
  std::vector<FileChunk> _fileChunks;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_ISFBASEREADER_H_
