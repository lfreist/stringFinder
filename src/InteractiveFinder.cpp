// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "InteractiveFinder.hpp"
#include "StringFinder.hpp"

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::sort;



// ____________________________________________________________________________
InteractiveFinder::InteractiveFinder() = default;

// ____________________________________________________________________________
InteractiveFinder::~InteractiveFinder() = default;

// ____________________________________________________________________________
void InteractiveFinder::parseCommandLineArguments(int argc, char **argv) {
    if (argc == 2) {
        _sf.readFile(argv[optind]);
    } else if (argc > 2) {
        cout << "Too many arguments provided!" << endl;
        printUsageAndExit();
    }
}

// ____________________________________________________________________________
void InteractiveFinder::usage() const {
    cout << "Usage: ./InteractiveFinder [FILE]" << endl;
    cout << " Initialize an interactive shell for 'StringFinder' searching "
            "on [FILE]" << endl;
}

// ____________________________________________________________________________
void InteractiveFinder::interactiveUsage() const {
    cout << "Usage: [COMMAND] {OBJECT} {--ARGUMENT {INT}}S" << endl;
    cout << " Commands:" << endl;
    cout << "  exit    no object    no arguments          exit the interactive"
            " mode" << endl;
    cout << "  find    expression   performance, lines n  search 'expression'"
            " in current loaded file" << endl;
    cout << "  laod    file         no arguments          load 'file'"
            " (overwrite current loaded file)" << endl;
    cout << "  help    no object    no arguments          print this usage"
            " guide" << endl;
    cout << endl;
    cout << " Examples:" << endl;
    cout << "  find expression" << endl;
    cout << "  find \"expre ssion\"" << endl;
    cout << "  find expre-ssion" << endl;
    cout << "  find expression --lines --performance" << endl;
    cout << "  load /path/to/file" << endl;
    cout << endl;
}

// ____________________________________________________________________________
void InteractiveFinder::printUsageAndExit() const {
    usage();
    exit(1);
}

// ____________________________________________________________________________
void InteractiveFinder::run() {
    InputParser ip;
    bool exit = false;

    string input;

    bool matchCase;
    int numberLines;
    int counter = 0;
    vector<const string*> results;
    bool performance;

    while (!exit) {
        cout << "> ";
        getline(cin, input);
        ip.parse(input);
        if (!ip.validInput()) {
            cout << "Input not valid! Find more using 'help'." << endl;
            continue;
        }

        switch (ip.getCommand()->getName()) {
            // exit
            case 0:
                continue;
            case 1:
                cout << "Bye!" << endl;
                exit = true;
                break;
            // find
            case 2:
                matchCase = false;
                numberLines = 5;
                performance = false;
                for (auto arg : *(ip.getCommand()->getArguments())) {
                    if (*(arg->getName()) == "performance") {
                        performance = true;
                        numberLines = 0;
                    } else if (*(arg->getName()) == "matchCase") {
                        matchCase = true;
                    } else if (*(arg->getName()) == "lines") {
                        numberLines = arg->getValue();
                    }
                }
                cout << "Searching for '" << *(ip.getCommand()->getObject())
                  << "'..." << endl;
                if (performance) {
                    _sf.measurePerformance(*(ip.getCommand()->getObject()),
                                           matchCase);
                }
                results = _sf.find(*(ip.getCommand()->getObject()),
                                   matchCase);
                sort(results.begin(), results.end());
                cout << " Found " << results.size() << " matching lines."
                  << endl;
                if (numberLines == 0) {
                    break;
                }
                cout << " Here are the first " << numberLines << ":" << endl;
                counter = 0;
                for (auto result : results) {
                    cout << "   " << counter << ". \t" << *result << endl;
                    numberLines--;
                    counter++;
                    if (numberLines <= 0) {
                        break;
                    }
                }
                break;
            // load
            case 3:
                _sf.readFile(*(ip.getCommand()->getObject()), true);
                break;
            // help
            case 4:
                interactiveUsage();
                break;
            default:
                break;
        }
    }
}


// ____________________________________________________________________________
// ____________________________________________________________________________
InputParser::InputParser() = default;

// ____________________________________________________________________________
InputParser::~InputParser() = default;

// ____________________________________________________________________________
void InputParser::parse(const string &input) {
    if (input.empty()) {
        _command = Command(0);
    }
    bool commandParsed = false;
    bool isArgument = false;
    bool objectParsed = false;
    bool useSpace = false;
    string tmpStr = "";

    Argument* arg = nullptr;

    for (int i = 0; i <= input.size(); i++) {
        if (((input[i] == ' ' || input[i] == '\t') && !useSpace)
            || i == input.size()) {
            if (!commandParsed) {
                if (tmpStr.empty()) {
                    continue;
                }
                if (tmpStr == "exit") {
                    _command = Command(1);
                } else if (tmpStr == "find") {
                    _command = Command(2);
                } else if (tmpStr == "load") {
                    _command = Command(3);
                } else if (tmpStr == "help") {
                    _command = Command(4);
                } else {
                    cout << "Unknown command '" << tmpStr << "'" << endl;
                }
                commandParsed = true;
            } else {
                if (isArgument) {
                    arg = new Argument(tmpStr);
                    _command.addArgument(arg);
                    arg = nullptr;
                    isArgument = false;
                } else if (!objectParsed) {
                    _command.setObject(tmpStr);
                    objectParsed = true;
                } else {
                    if (isInteger(tmpStr)) {
                        _command.addValueToLastArg(std::stoi(tmpStr));
                    } else {
                        cout << "Only integer values ar legal." << endl;
                    }
                }
            }
            tmpStr = "";
        } else if (input[i] == '-' &&
                   (input[i-1] == ' ' || input[i-2] == ' ')) {
            isArgument = true;
        } else if (input[i] == '"') {
            useSpace = !useSpace;
        } else {
                tmpStr += input[i];
        }
    }
}

// ____________________________________________________________________________
Command* InputParser::getCommand() {
    return &(_command);
}

// ____________________________________________________________________________
bool InputParser::validInput() {
    switch (_command.getName()) {
        case 0:
        case 1:
        case 4:
            return true;
        case 2:
        case 3:
            return !(_command.getObject()->empty());
        default:
            return false;
    }
}

// ____________________________________________________________________________
// ____________________________________________________________________________
Command::Command() {
    _name = 0;
    _object = "";
}

// ____________________________________________________________________________
Command::Command(char name) {
    _name = name;
    _object = "";
}

// ____________________________________________________________________________
Command::~Command() {
    for (auto& arg : _arguments) {
        delete arg;
    }
}

// ____________________________________________________________________________
char Command::getName() {
    return _name;
}

// ____________________________________________________________________________
void Command::addArgument(Argument* arg) {
    // exit and help do not take args
    if (_name == 1 || _name == 4) {
        return;
    }
    _arguments.push_back(arg);
}

// ____________________________________________________________________________
void Command::addValueToLastArg(int value) {
    _arguments.back()->setValue(value);
}

// ____________________________________________________________________________
void Command::setObject(const string obj) {
    // exit and help do not take an object
    if (_name == 1 || _name == 4) {
        return;
    }
    _object = obj;
}

// ____________________________________________________________________________
string* Command::getObject() {
    return &(_object);
}

// ____________________________________________________________________________
vector<Argument*>* Command::getArguments() {
    return &(_arguments);
}

// ____________________________________________________________________________
// ____________________________________________________________________________
Argument::Argument(string name) {
    _name = name;
    if (name == "lines") {
        _hasValue = true;
    } else {
        _hasValue = false;
    }
}

// ____________________________________________________________________________
Argument::Argument(string name, int value) {
    _name = name;
    _hasValue = true;
    _value = value;
}

// ____________________________________________________________________________
Argument::~Argument() = default;

// ____________________________________________________________________________
int Argument::getValue() {
    if (!_hasValue) {
        return -1;
    }
    return _value;
}

// ____________________________________________________________________________
string* Argument::getName() {
    return &_name;
}

// ____________________________________________________________________________
void Argument::setValue(int value) {
    if (!_hasValue) {
        cout << _name << " does not has a value." << endl;
    } else {
        _value = value;
    }
}


// ____________________________________________________________________________
// ____________________________________________________________________________
inline bool isInteger(const string& str) {
    if (str.empty() ||
       ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) {
        return false;
    }

    char* p;
    strtol(str.c_str(), &p, 10);

    return (*p == 0);
}
