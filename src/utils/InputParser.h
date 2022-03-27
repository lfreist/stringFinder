// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_INPUTPARSER_H_
#define SRC_UTILS_INPUTPARSER_H_

#include <boost/spirit.hpp>
#include <vector>
#include <string>
#include <map>

namespace po = boost::program_options;

using std::string;
using std::vector;

class InputParser {
 public:
  InputParser(std::map<string, po::variables_map> commandsMap);
  ~InputParser();
  void parse(string input);

 private:
  po::variables_map _commandsMap;
};


#endif  // SRC__UTILS_INPUTPARSER_H_