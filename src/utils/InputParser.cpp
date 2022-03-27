// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "InputParser.h"

InputParser::InputParser(std::map<string, po::variables_map> commandsMap) {
  _commandsMap = commandsMap;
}

InputParser::~InputParser() = default;

void InputParser::parse(string input) {

}