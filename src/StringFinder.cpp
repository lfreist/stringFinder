// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "StringFinder.hpp"
#include "Timer.hpp"

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

// ____________________________________________________________________________
StringFinder::StringFinder() = default;

// ____________________________________________________________________________
StringFinder::~StringFinder() = default;

// ____________________________________________________________________________
void StringFinder::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {"performance", 1, nullptr, 'p'},
            {nullptr, 0, nullptr, 0}
    };
    optind = 1;
    string performanceExpression;
    while (true) {
        int c = getopt_long(argc, argv, "p", options, nullptr);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'p':
                performanceExpression = string(optarg);
                break;
            default: break;
        }
    }
    if (optind >= argc) {
        cout << "Missing input file" << endl;
        exit(1);
    }
    readFile(argv[optind]);
    if (!performanceExpression.empty()) {
        measurePerformance(performanceExpression, false);
        measurePerformance(performanceExpression, true);
    }
}

// ____________________________________________________________________________
void StringFinder::readFile(const string& path, bool deleteOld) {
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
vector<string> StringFinder::find(string expression, bool matchCase) const {
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
void StringFinder::measurePerformance(const string& expression,
                                      bool matchCase) const {
    Timer timer;
    timer.start();
    vector<string> results = find(expression, matchCase);
    timer.stop();
    cout << "Performance Report:" << endl;
    cout << "StringFinder.measurePerformance(" << expression  << ", "
        << matchCase << "):" << endl;
    cout << " total lines:\t" << _data.size() << endl;
    cout << " total matches:\t" << results.size() << endl;
    cout << " query time:\t" << timer.elapsedSeconds() << " s" << endl;
    cout << " time / match:\t" << timer.elapsedSeconds()/results.size()
        << " s" << endl;
}
