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
using std::cout;
using std::cerr;
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
    cout << "Reading file " << path << endl;
    string line;
    ifstream file(path.c_str());
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << path << endl;
        return;
    }
    if (deleteOld) {
        _data.clear();
    }
    while (!file.eof()) {
        getline(file, line);
        _data.push_back(line);
    }
    cout << "done" << endl;
}

// ____________________________________________________________________________
vector<const string*> StringFinder::find(string expression,
                                         bool matchCase) const {
    // #pragma omp declare reduction
    // (merge: vector<const string*> : omp_out.insert(omp_out.end(),
    // omp_in.begin(), omp_in.end()))
    // TODO(lfreist): user merge sort here!
    vector<const string*> results;
    if (!matchCase) {
        transform(expression.begin(),
                  expression.end(),
                  expression.begin(),
                  ::tolower);
    }
    // #pragma omp parallel for reduction(merge: results)
    #pragma omp parallel for
    for (vector<string>::const_iterator it = _data.begin();
         it != _data.end();
         it++) {
        if (!matchCase) {
            string newStr = *it;
            transform(newStr.begin(),
                      newStr.end(),
                      newStr.begin(),
                      ::tolower);
            if (newStr.find(expression) != string::npos) {
                #pragma omp critical
                results.push_back(&(*it));
            }
        } else {
            if (it->find(expression) != string::npos) {
                #pragma omp critical
                results.push_back(&(*it));
            }
        }
    }
    return results;
}

// ____________________________________________________________________________
void StringFinder::measurePerformance(const string& expression,
                                      bool matchCase) const {
    Timer timer;
    timer.start();
    vector<const string*> results = find(expression, matchCase);
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
