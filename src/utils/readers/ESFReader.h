// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>

#include "./BaseReader.h"

#ifndef STRINGFINDER_SRC_UTILS_READERS_EXTERNALREADER_H_
#define STRINGFINDER_SRC_UTILS_READERS_EXTERNALREADER_H_

namespace sf::utils::readers {

class ESFReader : public BaseReader {
 protected:
  std::string _filePath;
};

}  // namespace sf::utils::readers

#endif //STRINGFINDER_SRC_UTILS_READERS_EXTERNALREADER_H_
