// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <memory>

#include "./src/InternStringFinder.h"
#include "./src/utils/readers/ISFCompressedReader.h"
#include "./src/utils/readers/ISFReader.h"

namespace po = boost::program_options;

void option_dependency(const po::variables_map &vm, const std::string &for_what, const std::string &required_option) {
  if ((vm.count(for_what) && !vm[for_what].defaulted())
    && (vm.count(required_option) == 0 || vm[required_option].defaulted())) {
    throw std::logic_error(std::string("Option '") + for_what + "' requires option '" + required_option + "'.");
  }
}

void option_anti_dependency(const po::variables_map &vm,
                            const std::string &for_what,
                            const std::string &anti_dependency) {
  if ((vm.count(for_what) && !vm[for_what].defaulted())
    && (vm.count(anti_dependency) && !vm[anti_dependency].defaulted())) {
    throw std::logic_error(std::string("Option '") + for_what + "' can not be set with '" + anti_dependency + "'.");
  }
}

int main(int argc, char **argv) {
  bool performance;
  bool ignoreCase;
  std::string inputFile;
  std::string metaFile;
  std::vector<unsigned> nThreads;
  bool count;
  unsigned nBuffers;
  unsigned minBufferSize;
  unsigned bufferOverflowSize;

  po::options_description options("Options for InterStringFinderMain");
  po::positional_options_description positional_options;

  auto add_positional = [&positional_options]<typename... Args>(Args &&... args) {
    positional_options.add(std::forward<Args>(args)...);
  };
  auto add = [&options]<typename... Args>(Args &&... args) {
    options.add_options()(std::forward<Args>(args)...);
  };

  add_positional("input-file", 1);
  add("input-file", po::value<std::string>(&inputFile), "input-file.");
  add("meta-file,m", po::value<std::string>(&metaFile)->default_value(""), "meta-file (if input is ESF-compressed).");
  add("help,h", "Produces this help message.");
  add("performance,p", po::bool_switch(&performance), "measure performance.");
  add("threads,j", po::value<std::vector<unsigned>>(&nThreads)->multitoken(),
      "number of threads used for decompression, transformation and search (e.g. '-j 4 2 2' for 4 decompression threads, 2 transformation threads and 2 search threads).");
  add("ignore-case,C", po::bool_switch(&ignoreCase), "ignore case.");
  add("count,c", po::bool_switch(&count), "only count the number of matching lines.");
  add("buffer-size", po::value(&minBufferSize)->default_value(2 << 23), "minimal size of one buffer.");

  po::variables_map optionsMap;

  try {
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional_options).run(), optionsMap);
    if (optionsMap.count("help")) {
      std::cout << options << std::endl;
      return 0;
    }
    if (!optionsMap.count("input-file")) {
      throw std::runtime_error("You must provide an input file");
    }
    po::notify(optionsMap);
    if (optionsMap.count("threads")) {
      // if only one integer is provided, it is considered to be the total number of threads
      if (nThreads.size() == 1) {
        if (nThreads[0] <= 2) {
          if (optionsMap["meta-file"].defaulted()) {
            if (ignoreCase) {
              nThreads = {0, 1, 1};
            } else {
              nThreads = {0, 0, nThreads[0]};
            }
          } else {
            if (ignoreCase) {
              nThreads = {1, 1, 1};
            } else {
              nThreads = {1, 0, 1};
            }
          }
        } else {
          if (ignoreCase) {
            unsigned numThreads = ceil(static_cast<double>(nThreads[0]) / 3);
            nThreads = {numThreads, numThreads, nThreads[0] - numThreads - numThreads};
          } else {

            unsigned numThreads = ceil(static_cast<double>(nThreads[0]) / 2);
            nThreads = {numThreads, 0, nThreads[0] - numThreads};
          }
        }
      }
    } else {
      nThreads = {static_cast<unsigned int>(metaFile.empty() ? 0 : 1), static_cast<unsigned int>(ignoreCase ? 1 : 0), 1};
    }
  } catch (const std::exception &e) {
    std::cerr << "Error in command line argument: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }

  sf::InternStringFinder isf;
  std::unique_ptr<sf::utils::readers::BaseReader> reader;
  if (!metaFile.empty()) {
    reader = std::make_unique<sf::utils::readers::BaseReader>(sf::utils::readers::ISFCompressedReader(inputFile, metaFile));
  } else {
    reader = std::make_unique<sf::utils::readers::BaseReader>(sf::utils::readers::ISFReader());
  }

  while (true) {
    std::cout << "keyword: ";
    std::string input;
    std::cin >> input;
    if (input == "exit" || input == "quit") {
      std::cout << "bye" << std::endl;
      break;
    }
    auto results = isf.find(input, std::move(reader), ignoreCase, performance, nThreads[0], nThreads[1], nThreads[2]);
    if (results.empty()) {
      std::cout << "Nothing found for '" << input << "'..." << std::endl;
      continue;
    }
    std::cout << "Found " << results.size() << " matches:" << std::endl;
    for (int i = 1; i < 11; ++i) {
      std::cout << i << ". " << results[i] << "\n";
    }
    std::cout << std::endl;
  }
  return 0;
}
