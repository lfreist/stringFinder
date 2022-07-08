// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <functional>

#include "UninitializedAllocator.h"

#ifndef STRINGFINDER_SRC_UTILS_FILECHUNK_H_
#define STRINGFINDER_SRC_UTILS_FILECHUNK_H_

namespace sf::utils {

using strtype = std::basic_string<char, std::char_traits<char>, ad_utility::default_init_allocator<char>>;

class FileChunk {
 public:
  FileChunk();
  explicit FileChunk(strtype data, ulong offset = 0);
  FileChunk(strtype data, ulong originalSize, ulong offset);

  void setContent(strtype data);

  [[nodiscard]] bool isCompressed() const;
  [[nodiscard]] const strtype& str() const;

  [[nodiscard]] uint64_t getOffset() const;
  [[nodiscard]] uint64_t getOriginalSize() const;

  void setOffset(uint64_t offset);

  strtype* strPtr();

 private:
  strtype _content;
  // indicating position of _content's first byte relative to start of whole data
  uint64_t _offset = 0;
  // if _content is compressed, the size of its uncompressed data must be known
  uint64_t _originalSize = 0;

  // friend class StringFinder;
};

}  // namespace sf::utils

#endif //STRINGFINDER_SRC_UTILS_FILECHUNK_H_
