// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_INTERACTIVEFINDER_HPP_
#define SRC_INTERACTIVEFINDER_HPP_

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "StringFinder.hpp"

using std::string;
using std::vector;

class InteractiveFinder {
 public:
    InteractiveFinder();
    ~InteractiveFinder();

    // only takes input file (optionally)
    void parseCommandLineArguments(int argc, char** argv);

    void run();
 private:
    StringFinder _sf;

    uint8_t parseInput();

    void usage() const;
    void printUsageAndExit() const;
};


class Argument {
public:
    Argument(string name);
    Argument(string name, int value);
    ~Argument();

    int getValue();
    string* getName();
    void setValue(int value);
private:
    string _name;
    int _value;
    bool _hasValue;
};


class Command {
 public:
    Command();
    Command(char name);
    ~Command();

    char getName();
    void setObject(const string obj);
    string* getObject();
    vector<Argument*>* getArguments();
    void addArgument(Argument* arg);
    void addValueToLastArg(int value);
 private:
    vector<Argument*> _arguments;
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

    Command* getCommand();
    void parse(const string &input);
    bool validInput();
private:
    Command _command;

    FRIEND_TEST(InteractiveFinderTest, parseInput);
};

bool isInteger(const string& str);

#endif  // SRC_INTERACTIVEFINDER_HPP_
