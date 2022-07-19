// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_COMPRESSOR_H_
#define SRC_UTILS_COMPRESSOR_H_

#include <string>
#include <iostream>
#include <fstream>
#include <exception>

#include "./ZstdWrapper.h"
#include "./FileChunk.h"
#include "./SFMetaFile.h"
#include "./readers/ESFIFSTREAMReader.h"

namespace sf::utils {

class ESFCompress {
 public:
  static void compress(const std::string &srcFile,
                       const std::string &outFile,
                       const std::string &metaFile = "",
                       int compressionLevel = 3,
                       std::streamsize minChunkSize = (2 << 23),
                       int chunkOverflowSize = (2 << 13)) {
    std::ifstream _srcFile(srcFile);
    if (!_srcFile.is_open()) {
      throw std::runtime_error("Could not read source file '" + srcFile + "'...");
    }
    std::ofstream _outFile(outFile);
    if (!_outFile.is_open()) {
      throw std::runtime_error("Could not write out file '" + outFile + "'...");
    }

    SFMetaFile meta((metaFile.empty() ? (outFile + std::string(".meta")) : metaFile), std::ios::out);
    readers::ESFIFSTREAMReader reader(srcFile, minChunkSize, minChunkSize+chunkOverflowSize);

    while (true) {
      std::optional<FileChunk> optionalChunk = reader.chunkProvider();
      if (!optionalChunk) { break; }
      auto& chunk = optionalChunk.value();
      uint32_t originalSize = chunk.strPtr()->length();
      std::vector<char> compressed = ZstdWrapper::compress(chunk.strPtr()->data(), originalSize, compressionLevel);
      _outFile.write(compressed.data(), static_cast<std::streamsize>(compressed.size()));
      chunkSize cs{originalSize, static_cast<uint32_t>(compressed.size())};
      meta.writeChunkSize(cs);
    }

    _srcFile.close();
    _outFile.close();
  }
};

}  // namespace sf::utils

#endif  // SRC_UTILS_COMPRESSOR_H_