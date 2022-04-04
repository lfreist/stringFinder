// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "src/utils/ESFCompressor.h"

#include <string>
#include <ostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using std::string;


int main(int argc, char **argv) {
  string sourceFile;
  string outputFile;
  string metaFile;
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
  add("input-file", po::value<string>(&sourceFile), "input-file.");
  add("output-file,o", po::value<string>(&outputFile)->default_value("<input-file>.esf.zst"), "output-file.");
  add("meta-file,m", po::value<string>(&metaFile)->default_value("<output-vile>.meta"), "meta-file.");
  add("compression-level,c", po::value<int>(&compressionLevel)->default_value(3), "zstd compression level.");
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
  outputFile = outputFile == "<input-file>.esf.zst" ? sourceFile + string(".esf.zst") : outputFile;
  metaFile = metaFile == "<input-file>.esf.zst.meta" ? metaFile = outputFile + string(".meta") : metaFile;

  std::cout << "Compressing (level " << compressionLevel << ") '" << sourceFile << "' to '" << outputFile << "' in "
  << blockSize << " blocks." << std::endl;
  bool result = ESFCompress::compress(sourceFile, outputFile, metaFile, compressionLevel, blockSize);
  if (result) {
    return 0;
  }
  std::cerr << "Error compressing '" << sourceFile << "." << std::endl;
  return 1;
}
