// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "StringData.hpp"

using std::vector;
using std::string;
using std::cerr;
using std::endl;
using std::ifstream;

// ____________________________________________________________________________
StringData::StringData() = default;

// ____________________________________________________________________________
StringData::~StringData() {
    // for (string& str : _data) {
    //     delete str;
    // }
}

// ____________________________________________________________________________
void StringData::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {nullptr, 0, nullptr, 0 }
    };
    optind = 1;
    while (true) {
        int c = getopt_long(argc, argv, "", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            default: break;
        }
    }
    if (optind >= argc) {
        exit(1);
    }
    readFile(argv[optind]);
}

// ____________________________________________________________________________
void StringData::readFile(const string& path, bool deleteOld) {
    string line;
    ifstream file(path.c_str());
    if (!file.is_open()) {
        cerr << "Error: Cannot open file" << path << endl;
        exit(1);
    }
    if (deleteOld) {
        // for (string& str : _data) {
        //     delete str;
        // }
        _data.clear();
    }
    while (!file.eof()) {
        getline(file, line);
        _data.push_back(line);
    }
}

// ____________________________________________________________________________
vector<string> StringData::find(string expression, bool matchCase) {
    vector<string> results;
    string newStr = "";
    if (!matchCase) {
        transform(expression.begin(),
                  expression.end(),
                  expression.begin(),
                  ::tolower);
        // expression = toLower(expression);
    }
    for (string& str : _data) {
        newStr = str;
        if (!matchCase) {
            transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
            // newStr = toLower(newStr);
        }
        if (newStr.find(expression) != string::npos) {
            results.push_back(str);
        }
    }
    return results;
}

// ____________________________________________________________________________
// ____________________________________________________________________________
string toLower(string str) {
    string strLower = "";
    for (char& c : str) {
        if (c >= 65 && c <= 90) {
            strLower += c + 32;
        } else {
            strLower += c;
        }
    }
    return strLower;
}
