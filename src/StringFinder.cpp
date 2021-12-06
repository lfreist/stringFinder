// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <iostream>
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

/*
void mergeArrays(vector<const string*> *out, vector<const string*> *in) {
    long unsigned int in_loc = 0;
    long unsigned int out_loc = 0;
    while (true) {
        if (in_loc >= in->size()) {
            break;
        }
        if (out_loc >= out->size()) {
            out->insert(out->end(), in->begin() + in_loc, in->end());
            break;
        }

        if (in->at(in_loc) < out->at(out_loc)) {
            out->insert(out->begin() + out_loc, in->at(in_loc));
            in_loc++;
            out_loc++;
        } else {
            out_loc++;
        }
    }
}
*/

// find badminton --matchCase --performance takes ~0.23s on wikidata-publications.tsv
void mergeArrays(vector<const string*> *out, vector<const string*> *in) {
    vector<const string*> tmp(out->size() +  in->size());
    std::merge(out->begin(), out->end(), in->begin(), in->end(), tmp.begin());
    *out = std::move(tmp);
}

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
void StringFinder::readFile(const string& path, bool append) {
    cout << "Reading file " << path << endl;
    string line;
    ifstream file(path.c_str());
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << path << endl;
        return;
    }
    if (!append) {
        _data.clear();
    }
    int counter = 0;
    int innerCounter = 1;
    while (!file.eof()) {
        getline(file, line);
        if (line.empty()) {
            continue;
        }
        _data.push_back(line);
        counter++;
        if (counter == 100000) {
            cout << "\r" << innerCounter * counter << " lines"
              << std::flush;
            counter = 0;
            innerCounter++;
        }
    }
    cout << "\r" << dataSize() << " lines" << endl;
    cout << "done" << endl;
}


// ____________________________________________________________________________
vector<const string*> StringFinder::find(string expression, bool matchCase) const {
    #pragma omp declare reduction (merge: vector<const string*> : mergeArrays(&omp_out, &omp_in))
    vector<const string*> results;
    if (!matchCase) {
        transform(expression.begin(), expression.end(), expression.begin(), ::tolower);
    }
    #pragma omp parallel for reduction(merge: results)
    // #pragma omp parallel for
    for (vector<string>::const_iterator it = _data.begin();
        it != _data.end();
        it++) {
        if (!matchCase) {
            string newStr = *it;
            transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
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
void StringFinder::measurePerformance(const string& expression, bool matchCase) const {
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

int StringFinder::dataSize() const {
    return _data.size();
}
