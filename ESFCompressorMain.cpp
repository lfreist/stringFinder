// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>
#include <ostream>
#include <boost/program_options.hpp>

#include "src/utils/ESFCompressor.h"

namespace po = boost::program_options;


int main(int argc, char **argv) {
  std::string sourceFile;
  std::string outputFile;
  std::string metaFile;
  int compressionLevel;
  int blockSize;

  po::options_description options("Options for ESFCompressorMain");
  po::positional_options_description positional_options;

  auto add_positional = [&positional_options]<typename... Args>(Args && ... args) {
    positional_options.add(std::forward<Args>(args)...);
  };
  auto add = [&options]<typename... Args>(Args && ... args) {
    options.add_options()(std::forward<Args>(args)...);
  };
  add_positional("input-file", 1);
  add_positional("output-file", 1);
  add("help,h", "Produces this help message.");
  add("input-file", po::value<std::string>(&sourceFile), "input-file.");
  add("output-file,o", po::value<std::string>(&outputFile)->default_value("<input-file>.esf.zst"), "output-file.");
  add("meta-file,m", po::value<std::string>(&metaFile)->default_value("<output-file>.meta"), "meta-file.");
  add("compression-level,l", po::value<int>(&compressionLevel)->default_value(3), "zstd compression level.");
  add("block-size", po::value<int>(&blockSize)->default_value(2 << 23), "size of one block.");

  po::variables_map optionsMap;

  try {
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional_options).run(), optionsMap);
    if (optionsMap.count("help")) {
      std::cout << options << std::endl;
      return 0;
    }
    if (!optionsMap.count("input-file")) {
      std::cerr << "Error: You must provide an input-file." << std::endl;
      return 1;
    }
    po::notify(optionsMap);
  } catch (const std::exception& e) {
    std::cerr << "Error in command line argument: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }
  outputFile = outputFile == "<input-file>.esf.zst" ? sourceFile + ".esf.zst" : outputFile;
  metaFile = metaFile == "<output-file>.meta" ? outputFile + ".meta" : metaFile;
  compressionLevel = compressionLevel > 19 ? 19 : compressionLevel;
  compressionLevel = compressionLevel < 1 ? 1 : compressionLevel;

  std::cout << "Compressing (level " << compressionLevel << ") '" << sourceFile << "' to '" << outputFile << std::endl;
  sf::utils::ESFCompress::compress(sourceFile, outputFile, metaFile, compressionLevel, blockSize);
  return 0;
}
