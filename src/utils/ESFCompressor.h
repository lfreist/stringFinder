// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_COMPRESSOR_H_
#define SRC_UTILS_COMPRESSOR_H_

#include <string>
#include <iostream>
#include <fstream>

#include "ZstdWrapper.h"
#include "FileChunk.h"
#include "ESFMetaFile.h"


class ESFCompress {
 public:
  static bool compress(const std::string& srcFile, const std::string& outFile, const std::string& metaFile = "",
                int compressionLevel = 3, unsigned long minChunkSize = (2<<23), int chunkOverflowSize = (2<<13)) {
    std::ifstream _srcFile(srcFile);
    if (!_srcFile.is_open()) {
      std::cerr << "Could not read source file '" << srcFile << "'..." << std::endl;
      return false;
    }
    std::ofstream _outFile(outFile);
    if (!_outFile.is_open()) {
      std::cerr << "Could not write out file '" << outFile << "'..." << std::endl;
      return false;
    }
    ESFMetaFile meta((metaFile.empty() ? (outFile + std::string(".meta")) : metaFile), std::ios::out);

    // write the max uncompressed buffer size to the top of the meta file
    meta.writeMaxOriginalSize(static_cast<unsigned long>(minChunkSize + chunkOverflowSize));
    // read whole file and compress chunks
    for (int counter = 1; ; counter++) {
      FileChunk chunk;
      size_t uncompressed_bytes_read = chunk.setContentFromFile(_srcFile, minChunkSize, true);
      if (uncompressed_bytes_read == 0) { break; } else if (uncompressed_bytes_read < 0) {
        std::cerr << "Error reading chunk: Did not found a new line. Increase chunkOverflowSize." << std::endl;
        return false;
      }
      // actual compression
      chunk.compress(compressionLevel);
      _outFile.write(chunk.getCompressedContent().data(), static_cast<std::streamsize>(chunk.compressedLength()));
      // write the actual uncompressed buffer size and the compressed size to the meta file: 'orig_size comp_size'
      chunkSize cs{static_cast<unsigned>(chunk.length()), static_cast<unsigned>(chunk.compressedLength())};
      meta.writeChunkSize(cs);
    }
    _srcFile.close();
    _outFile.close();
    return true;
  }
};

#endif  // SRC_UTILS_COMPRESSOR_H_