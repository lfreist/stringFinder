// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "src/utils/Timer.h"
#include "src/string_search_algorithms/using_strstr.h"
#include "src/string_search_algorithms/using_stdstring.h"

namespace po = boost::program_options;

using std::string;
using std::vector;

void printTableLine(string algorithm, string pattern, int matches, double time) {
  std::cout << std::left << std::setw(25) << algorithm
            << std::left << std::setw(15) << pattern
            << std::right << std::setw(15) << matches
            << std::right << std::setw(15) << time << std::endl;
}

int main(int argc, char **argv) {
  string inputFile;
  vector<string> searchPatterns;
  vector<string> algorithms;
  bool searchPerLine;
  bool searchAll;

  po::options_description options("Options for strstrSearchMain");
  po::positional_options_description positional_options;

  auto add_positional = [&positional_options]<typename... Args>(Args &&... args) {
    positional_options.add(std::forward<Args>(args)...);
  };
  auto add = [&options]<typename... Args>(Args &&... args) {
    options.add_options()(std::forward<Args>(args)...);
  };

  add_positional("input-file", 1);
  add("input-file", po::value(&inputFile), "input-file");
  add("patterns,p", po::value(&searchPatterns)->multitoken(), "search-patterns");
  add("algorithms", po::value(&algorithms)->multitoken(), "algorithms (strstr, stdstring, all)");
  add("per-line,l", po::bool_switch(&searchPerLine)->default_value(false), "search per line");
  add("all,a", po::bool_switch(&searchAll)->default_value(true), "search all");
  add("help,h", "Produces this help message.");

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
    if (!optionsMap.count("search-patterns")) {
      searchPatterns = {"keyword"};
    }
    if (!optionsMap.count("algorithms")) {
      algorithms = {};
    }
    po::notify(optionsMap);
  } catch (const std::exception &e) {
    std::cerr << "Error in command line argument: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }

  if (!(searchAll || searchPerLine)) {
    std::cout << "Either unset --all or set --per-line" << std::endl;
    return 0;
  }

  std::ifstream file(inputFile);
  string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

  std::cout << std::left << std::setw(25) << "algorithm"
            << std::left << std::setw(15) << "pattern"
            << std::right << std::setw(15) << "matches"
            << std::right << std::setw(15) << "time [s]" << std::endl;
  std::cout << "----------------------------------------------------------------------" << std::endl;

  Timer timer;
  int matches;

  if (std::find(algorithms.begin(), algorithms.end(), "all") != algorithms.end()) {
    algorithms.clear();
  }

  if (algorithms.empty() || std::find(algorithms.begin(), algorithms.end(), "strstr") != algorithms.end()) {
    char *c_content = const_cast<char *>(content.c_str());
    if (searchPerLine) {
      for (const auto& pattern : searchPatterns) {
        timer.start(true);
        matches = strstr_alg::findAllPerLine(pattern.c_str(), c_content);
        timer.stop();
        printTableLine("strstr (per line)", pattern, matches, timer.elapsedSeconds());
      }
    }

    if (searchAll) {
      for (const auto& pattern : searchPatterns) {
        timer.start(true);
        matches = strstr_alg::findAll(pattern.c_str(), c_content);
        timer.stop();
        printTableLine("strstr (all)", pattern, matches, timer.elapsedSeconds());
      }
    }
  }

  if (algorithms.empty() || std::find(algorithms.begin(), algorithms.end(), "stdstring") != algorithms.end()) {
    if (searchPerLine) {
      for (const auto& pattern : searchPatterns) {
        timer.start(true);
        matches = stdstring_alg::findAllPerLine(pattern, c_content);
        timer.stop();
        printTableLine("stdstring (per line)", pattern, matches, timer.elapsedSeconds());
      }
    }

    if (searchAll) {
      for (const auto& pattern : searchPatterns) {
        timer.start(true);
        matches = stdstring_alg::findAll(pattern, c_content);
        timer.stop();
        printTableLine("stdstring (all)", pattern, matches, timer.elapsedSeconds());
      }
    }
  }

  return 0;
}
