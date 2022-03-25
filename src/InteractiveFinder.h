// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_INTERACTIVEFINDER_H_
#define SRC_INTERACTIVEFINDER_H_

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "StringFinder.h"

using std::string;
using std::vector;

class InteractiveFinder {
 public:
  InteractiveFinder();

  ~InteractiveFinder();

  void parseCommandLineArguments(int argc, char **argv);

  void run();

 private:
  StringFinder _sf;

  uint8_t parseInput();

  void usage() const;

  void interactiveUsage() const;

  void printUsageAndExit() const;
};

class Argument {
 public:
  explicit Argument(string name);

  Argument(string name, int value);

  ~Argument();

  int getValue() const;

  string *getName();

  void setValue(int value);

 private:
  string _name;
  int _value;
  bool _hasValue;
};

class Command {
 public:
  Command();

  explicit Command(char name);

  ~Command();

  char getName();

  void setObject(const string obj);

  string *getObject();

  vector<Argument> *getArguments();

  void addArgument(string argStr);

  void addValueToLastArg(int value);

 private:
  vector<Argument> _arguments;
  // either expression (for find) or file (for load)
  string _object;
  char _name;
};

// valid inputs start with command followed by ONE single object
//  and n arguments:
//   command object --arg0 --arg1 ...
//   command --arg0 obj --arg1 ...
//   ...
class InputParser {
 public:
  InputParser();

  ~InputParser();

  Command *getCommand();

  void parse(const string &input);

  bool validInput();

 private:
  Command _command;

  FRIEND_TEST(InteractiveFinderTest, parseInput);
};

bool isInteger(const string &str);

#endif  // SRC_INTERACTIVEFINDER_H_
