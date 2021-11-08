// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "StringData.hpp"
#include "Timer.hpp"

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

// ____________________________________________________________________________
StringData::StringData() = default;

// ____________________________________________________________________________
StringData::~StringData() = default;

// ____________________________________________________________________________
void StringData::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {"performance", 1, nullptr, 'p'},
            {"output", 1, nullptr, 'o'},
            {nullptr, 0, nullptr, 0}
    };
    optind = 1;
    string performanceExpression;
    while (true) {
        int c = getopt_long(argc, argv, "p:o", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'p':
                performanceExpression = string(optarg);
                break;
            case 'o':
                _output = string(optarg);
            default: break;
        }
    }
    if (optind >= argc) {
        cout << "Missing input file" << endl;
        exit(1);
    }
    readFile(argv[optind]);
    if (!performanceExpression.empty()) {
        performance(performanceExpression);
    }
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
        _data.clear();
    }
    while (!file.eof()) {
        getline(file, line);
        _data.push_back(line);
    }
}

// ____________________________________________________________________________
vector<string> StringData::find(string expression, bool matchCase) const {
    vector<string> results;
    string newStr;
    if (!matchCase) {
        transform(expression.begin(),
                  expression.end(),
                  expression.begin(),
                  ::tolower);
    }
    for (const string& str : _data) {
        newStr = str;
        if (!matchCase) {
            transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
        }
        if (newStr.find(expression) != string::npos) {
            results.push_back(str);
        }
    }
    return results;
}

// ____________________________________________________________________________
void StringData::performance(const string& expression) const {
    Timer timer;
    timer.start();
    vector<string> results = find(expression, true);
    timer.stop();
    cout << "Performance Report (expression: " << expression
        << "):" << endl;
    cout << " total lines:\t" << _data.size() << endl;
    cout << " total matches:\t" << results.size() << endl;
    cout << " query time:\t" << timer.elapsedSeconds() << " s" << endl;
    cout << " time / match:\t" << timer.elapsedSeconds()/results.size()
        << " s" << endl;
}
