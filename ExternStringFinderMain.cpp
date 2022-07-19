// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <boost/program_options.hpp>
#include <string>
#include <vector>

#include "src/ExternStringFinder.h"

namespace po = boost::program_options;

using std::string;
using std::vector;

void option_dependency(const po::variables_map &vm, const string &for_what, const string &required_option) {
  if ((vm.count(for_what) && !vm[for_what].defaulted())
      && (vm.count(required_option) == 0 || vm[required_option].defaulted())) {
    throw std::logic_error(std::string("Option '") + for_what + "' requires option '" + required_option + "'.");
  }
}

void option_anti_dependency(const po::variables_map &vm, const string &for_what, const string &anti_dependency) {
  if ((vm.count(for_what) && !vm[for_what].defaulted())
      && (vm.count(anti_dependency) && !vm[anti_dependency].defaulted())) {
    throw std::logic_error(std::string("Option '") + for_what + "' can not be set with '" + anti_dependency + "'.");
  }
}

int main(int argc, char **argv) {
  bool performance;
  bool verbose;
  bool matchCase;
  string searchPattern;
  string inputFile;
  string metaFile;
  vector<unsigned> nThreads;
  bool count;
  unsigned nBuffers;
  unsigned minBufferSize;
  unsigned bufferOverflowSize;

  po::options_description options("Options for ExternStringFinderMain");
  po::positional_options_description positional_options;

  auto add_positional = [&positional_options]<typename... Args>(Args &&... args) {
    positional_options.add(std::forward<Args>(args)...);
  };
  auto add = [&options]<typename... Args>(Args &&... args) {
    options.add_options()(std::forward<Args>(args)...);
  };

  add_positional("search-pattern", 1);
  add_positional("input-file", 1);
  add("search-pattern", po::value<string>(&searchPattern), "search-pattern.");
  add("input-file", po::value<string>(&inputFile), "input-file.");
  add("meta-file,m", po::value<string>(&metaFile)->default_value(""), "meta-file (if input is ESF-compressed).");
  add("help,h", "Produces this help message.");
  add("verbose,v", po::bool_switch(&verbose), "toggle verbosity.");
  add("performance,p", po::bool_switch(&performance), "measure performance.");
  add("threads,j",
      po::value<vector<unsigned>>(&nThreads)->multitoken(),
      "number of threads used for decompression and search (e.g. '-j 4 2' for 4 decompression threads and 2 search threads).");
  add("buffers,b",
      po::value<unsigned>(&nBuffers)->default_value(10),
      "number of buffers used for decompression and search.");
  add("match-case,C", po::bool_switch(&matchCase), "match case.");
  add("count,c", po::bool_switch(&count), "only count the number of matching lines.");
  add("buffer-size", po::value(&minBufferSize)->default_value(2 << 23), "minimal size of one buffer.");
  add("buffer-overflow-size", po::value(&bufferOverflowSize)->default_value(2 << 15), "maximal additional sized to buffer-size.");

  po::variables_map optionsMap;

  try {
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional_options).run(), optionsMap);
    if (optionsMap.count("help")) {
      std::cout << options << std::endl;
      return 0;
    }
    if (!optionsMap.count("search-pattern") || !optionsMap.count("input-file")) {
      std::cerr << "Error: You must provide a search-pattern and a file."
                << "If you want to read stdin, provide '-' as file." << std::endl;
      return 1;
    }
    po::notify(optionsMap);
    if (optionsMap.count("threads")) {
      // if only one integer is provided, it is considered to be the total number of threads
      if (nThreads.size() == 1) {
        if (nThreads[0] <= 1) {
          nThreads[0] = 1;
          nThreads.push_back(1);
        } else if (!optionsMap["meta-file"].defaulted()) {
          nThreads[0] = nThreads[0] / 2;
          nThreads.push_back(nThreads[0]);
        } else {
          nThreads.push_back(nThreads[0]);
          nThreads[0] = 0;
        }
      }
    } else {
      nThreads = {1, 1};
    }
  } catch (const std::exception &e) {
    std::cerr << "Error in command line argument: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }

  ExternStringFinder extern_string_finder(inputFile, searchPattern, metaFile, verbose, performance, nBuffers,
                                          minBufferSize, bufferOverflowSize, nThreads[0], nThreads[1]);
  auto result = extern_string_finder.find();
  if (count) {
    std::cout << "Found " << result.size() << " matches" << std::endl;
  }
  return 0;
}
