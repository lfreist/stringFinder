// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <boost/program_options.hpp>
#include <iostream>

#include "src/StringFinder.h"
#include "src/InteractiveFinder.h"

namespace po = boost::program_options;


int main(int argc, char **argv) {
  bool performance;
  bool verbose;
  bool matchCase;
  string searchPattern;
  string inputFile;
  unsigned nThreads;
  bool count;
  bool interactive;

  po::options_description options("Options for StringFinderMain");
  po::positional_options_description positional_options;

  auto add_positional = [&positional_options]<typename... Args>(Args && ... args) {
    positional_options.add(std::forward<Args>(args)...);
  };
  auto add = [&options]<typename... Args>(Args && ... args) {
    options.add_options()(std::forward<Args>(args)...);
  };
  add_positional("search-pattern", 1);
  add_positional("input-file", 1);
  add("search-pattern", po::value<string>(&searchPattern), "search-pattern.");
  add("input-file", po::value<string>(&inputFile), "input-file");
  add("help,h", "Produces this help message.");
  add("performance,p", po::bool_switch(&performance), "measure performance.");
  add("verbose,v", po::bool_switch(&verbose), "toggle verbosity.");
  add("matchCase,C", po::bool_switch(&matchCase), "match case.");
  add("threads,j", po::value<unsigned>(&nThreads)->default_value(1), "number of threads used for search.");
  add("count,c", po::bool_switch(&count), "only count the number of matching lines.");
  add("interactive", po::bool_switch(&count), "run in interactive mode (ignores other optional parameters).");

  po::variables_map optionsMap;

  try {
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional_options).run(), optionsMap);
    if (optionsMap.count("help")) {
      std::cout << options << std::endl;
      return 0;
    }
    if (!optionsMap.count("search-pattern") || !optionsMap.count("input-file")) {
      std::cerr << "Error: You must provide a search-pattern and an input-file." << std::endl;
      return 1;
    }
    po::notify(optionsMap);
  } catch (const std::exception& e) {
    std::cerr << "Error in command line argument: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }
  if (interactive) {

  } else {
    StringFinder string_finder(inputFile, verbose, nThreads);
    if (performance) {
      string_finder.measurePerformance(searchPattern, matchCase);
    } else {
      vector<const string *> res = string_finder.find(searchPattern, matchCase);
      if (count) {
        std::cout << res.size() << std::endl;
      } else {
        for (auto &str: res) {
          std::cout << "(" << str << ")\t" << *str << std::endl;
        }
      }
    }
  }
}
