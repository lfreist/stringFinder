// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/utils/ESFCompressor.h"

#include <string>
#include <ostream>

#include <getopt.h>

std::string sourceFile;
std::string outputFile;
std::string metaFile;
int compressionLevel = 3;
int chunkSize = (2 << 23);

void parseCommandLineArgs(int argc, char **argv) {
  struct option options[] = {
      {"help", 0, nullptr, 'h'},
      {"meta", 1, nullptr, 'm'},
      {"chunk-size", 1, nullptr, 's'},
      {"compression-level", 1, nullptr, 'c'},
      {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "h:m:s:c:", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'h': break;
      case 'm': metaFile.assign(optarg);
        break;
      case 's': chunkSize = atoi(optarg);
        break;
      case 'c': compressionLevel = atoi(optarg);
        break;
      default: break;
    }
  }
  sourceFile.assign(argv[optind++]);
  outputFile.assign(argv[optind]);
}

int main(int argc, char **argv) {
  parseCommandLineArgs(argc, argv);
  bool result = ESFCompress::compress(sourceFile, outputFile, metaFile, compressionLevel, chunkSize);
  if (result) {
    return 0;
  }
  std::cerr << "Error compressing '" << sourceFile << "' to '" << outputFile << "'" << std::endl;
  return 1;
}
