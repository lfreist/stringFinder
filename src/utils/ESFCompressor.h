// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_COMPRESSOR_H_
#define SRC_UTILS_COMPRESSOR_H_

#include <cstdio>
#include <string>
#include <iostream>

#include "ZstdWrapper.h"
#include "FileChunk.h"
#include "ESFMetaFile.h"


class ESFCompress {
 public:
  static bool compress(const std::string& srcFile, const std::string& outFile, const std::string& metaFile = "",
                int compressionLevel = 3, unsigned long minChunkSize = (2<<23), int chunkOverflowSize = (2<<10)) {
    FILE *src = fopen(srcFile.c_str(), "r");
    if (src == nullptr) {
      std::cerr << "Could not read source file '" << srcFile << "'..." << std::endl;
      return false;
    }
    FILE *out = fopen(outFile.c_str(), "w");
    if (out == nullptr) {
      std::cerr << "Could not write out file '" << outFile << "'..." << std::endl;
      return false;
    }
    ESFMetaFile meta((metaFile.empty() ? (outFile + std::string(".meta")) : metaFile), std::ios::out);

    // write the max uncompressed buffer size to the top of the meta file
    meta.writeMaxOriginalSize(static_cast<unsigned long>(minChunkSize + chunkOverflowSize));
    // read whole file and compress chunks
    for (int counter = 1; ; counter++) {
      FileChunk chunk(minChunkSize + chunkOverflowSize);
      long uncompressed_bytes_read = chunk.setContentFromFile(src, minChunkSize, true);
      if (uncompressed_bytes_read == 0) { break; } else if (uncompressed_bytes_read < 0) {
        std::cerr << "Error reading chunk: Did not found a new line. Increase chunkOverflowSize." << std::endl;
        return false;
      }
      // actual compression
      std::vector<char> comp = ZstdWrapper::compress((char*) chunk.cstring(), chunk.length(), compressionLevel);
      fwrite(comp.data(), comp.size(), 1, out);
      // write the actual uncompressed buffer size and the compressed size to the meta file: 'orig_size comp_size'
      chunkSize cs{static_cast<unsigned long>(uncompressed_bytes_read), comp.size()};
      cs.originalSize = static_cast<unsigned long>(uncompressed_bytes_read);
      cs.compressedSize = static_cast<unsigned long>(comp.size());
      meta.writeChunkSize(cs);
    }
    fclose(src);
    fclose(out);
    return true;
  }


  static bool decompress(const std::string& srcFile, const std::string& outFile, const std::string& metaFile = "") {
    FILE *src = fopen(srcFile.c_str(), "r");
    if (src == nullptr) {
      std::cerr << "Could not read source file '" << srcFile << "'..." << std::endl;
      return false;
    }
    FILE *out = fopen(outFile.c_str(), "w");
    if (out == nullptr) {
      std::cerr << "Could not write out file '" << outFile << "'..." << std::endl;
      return false;
    }
    FILE *meta =  metaFile.empty() ?
                  fopen((outFile + std::string(".meta")).c_str(), "w") : fopen(metaFile.c_str(), "w");
    if (meta == nullptr) {
      std::cerr << "Could not write meta file..." << std::endl;
      return false;
    }
    // TODO: implement if needed or delete
  }

  // TODO: implement if needed or delete
  static int decompress(const FileChunk& compressed, FileChunk* uncompressed, int startPosition, int endPosition) {
    return 0;
  }
};

#endif  // SRC_UTILS_COMPRESSOR_H_