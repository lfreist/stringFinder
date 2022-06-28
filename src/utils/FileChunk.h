// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <functional>

#ifndef STRINGFINDER_SRC_UTILS_FILECHUNK_H_
#define STRINGFINDER_SRC_UTILS_FILECHUNK_H_

namespace sf::utils {

class FileChunk {
 public:
  FileChunk();
  explicit FileChunk(std::string data, ulong offset = 0);
  FileChunk(std::string data, ulong originalSize, ulong offset);

  void setContent(std::string data);

  [[nodiscard]] bool isCompressed() const;
  [[nodiscard]] const std::string& str() const;

  [[nodiscard]] uint64_t getOffset() const;
  [[nodiscard]] uint64_t getOriginalSize() const;


 private:
  std::string _content;
  // indicating position of _content's first byte relative to start of whole data
  uint64_t _offset = 0;
  // if _content is compressed, the size of its uncompressed data must be known
  uint64_t _originalSize = 0;

  // friend class StringFinder;
};

}  // namespace sf::utils

#endif //STRINGFINDER_SRC_UTILS_FILECHUNK_H_
