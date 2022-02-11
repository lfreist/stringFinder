// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/utils/ESFCompressor.h"

#include <string>
#include <ostream>

int main(int argc, char **argv) {
  std::string src;
  std::string dest;
  std::string meta;
  if (argc == 3) {
    src = std::string(argv[1]);
    dest = std::string(argv[2]);
  } else if (argc == 4) {
    src = std::string(argv[1]);
    dest = std::string(argv[2]);
    meta = std::string(argv[3]);
  } else {
    std::cout << "Usage: ESFCompressor src_file out_file [meta_file]" << std::endl;
    return 2;
  }
  bool result = ESFCompress::compress(src, dest, meta, 3, (2 << 29));
  if (result) {
    return 0;
  }
  std::cerr << "Error compressing '" << src << "' to '" << dest << "'" << std::endl;
  return 1;
}
